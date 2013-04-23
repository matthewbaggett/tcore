<?php
namespace tcore;

class model {
  const TYPE_INT      = "INT(10)";
  const TYPE_LONGINT  = "INT(32)";
  const TYPE_STR      = "VARCHAR(255)";
  const TYPE_TEXT     = "TEXT";
  const TYPE_DATE     = 'DATETIME';
  const TYPE_COORD    = 'DECIMAL(18,12)';
  const TYPE_BLOB     = "BLOB";
  const TYPE_LONGBLOB = "LONGBLOB";
  const TYPE_ON_OFF   = "ENUM('On', 'Off')";

  private $_supported_types = array(
    model::TYPE_INT,
    model::TYPE_LONGINT,
    model::TYPE_STR,
    model::TYPE_TEXT,
    model::TYPE_DATE,
    model::TYPE_COORD,
    model::TYPE_BLOB,
    model::TYPE_LONGBLOB,
    model::TYPE_ON_OFF,
  );

  protected $_table;
  private $_dirty = false;

  public static function factory() {
    $name = get_called_class();

    return new $name();
  }

  /**
   * New instance instantiated
   * Work out the correct table name by pluralising the model's name
   * Initialise the MySQL table, if neccisary.
   */
  public function __construct() {
    $model_name   = get_called_class();
    $this->_table = inflect::pluralize($model_name);
    $this->_table_init();
  }

  /**
   * Called to construct the table from the model.
   */
  private function _table_construct() {
    $db                  = new db();
    $sql_construct_table = $this->_table_construct_sql();
    tcore::log($sql_construct_table);
    $db->exec($sql_construct_table);
  }

  /**
   * Called on model initialisation after __construct
   */
  private function _table_init() {
    if (!$this->_table_exists()) {
      $this->_table_construct();
    }
  }

  /**
   * Get the name of the table.
   *
   * @return string
   */
  public function get_table() {
    return $this->_table;
  }

  private function _table_exists() {
    $db                 = db::get_instance();
    $table_exists_check = $db->query("
				SELECT table_name
				FROM information_schema.tables
				WHERE table_schema = '" . db::get_database_name() . "'
				AND table_name = '{$this->_table}';
				");
    if ($table_exists_check->foundRows() > 0) {
      return true;
    } else {
      return false;
    }
  }

  /**
   * Get column's type.
   * Determine what type the column is.
   *
   * @param string $column name of column
   *
   * @throws \exception
   * @return string
   */
  private function _get_column_type($column) {
    // Determine the type for the column, and set a sane default otherwise.
    // INT for key, STR for everything else.
    $type_property = "_{$column}_type";
    if (property_exists($this, $type_property)) {
      $type = $this->$type_property;
    } elseif ($column == $this->_get_id_column()) {
      $type = model::TYPE_INT;
    } else {
      $type = model::TYPE_STR;
    }

    // Check to see if the type requested is supported.
    if (in_array($type, $this->_supported_types)) {
      return $type;
    } else {
      throw new \exception("Sorry, but '{$column}' is an unsupported type!");
    }
  }

  /**
   * calculate the CONSTRUCT line for a given column
   *
   * @param string $column name of column
   *
   * @return string
   */
  private function _table_construct_sql_column($column) {
    $type           = $this->_get_column_type($column);
    $nullable       = $this->_get_id_column() == $column ? 'NOT NULL' : 'NULL';
    $auto_increment = $this->_get_id_column() == $column ? 'AUTO_INCREMENT' : '';

    return trim("`{$column}` {$type} {$nullable} {$auto_increment}");
  }

  /**
   * calculate the CREATE TABLE SQL block for a given models
   *
   * @return string CREATE TABLE statement.
   */
  private function _table_construct_sql() {
    $db_name = db::get_database_name();
    $sql     = "CREATE TABLE {$db_name}.`{$this->_table}` (\n";
    $key     = $this->_get_id_column();
    foreach ($this->_get_columns() as $column) {
      $sql .= "  {$this->_table_construct_sql_column($column)},\n";
    }
    $sql .= " PRIMARY KEY (`{$this->_get_id_column()}`) );\n";

    return $sql;
  }


  /**
   * called on model destruction
   */
  public function __destruct() {
    #$this->save();
  }

  /**
   * Save the Model
   *
   * @return \tcore\model
   */
  public function save() {
    if ($this->_is_dirty()) {
      $this->_save();
    }

    return $this;
  }

  /**
   * Return the state of the object - Dirty or Clean.
   * Clean objects are as-they-were from the database.
   *
   * @return boolean
   */
  protected function _is_dirty() {
    if ($this->_dirty) {
      return true;
    }

    return false;
  }

  /**
   * Called by model::save(), actually begins the savedown to DB.
   *
   * @return boolean
   */
  private function _save() {

    if ($this->get_id()) {
      $sql = $this->_save_update();
    } else {
      $sql = $this->_save_insert();
    }
    $this->_dirty = false;

    return true;
  }

  /**
   * Create an UPDATE statement and execute it.
   */
  private function _save_update() {
    $db_name = db::get_database_name();
    foreach ($this->_get_columns() as $column) {
      $safe   = db::get_instance()->quote($this->$column);
      $sets[] = "`$column` = {$safe}";
    }
    $sets_result = implode(", ", $sets);
    $sql_update  = "UPDATE {$db_name}.{$this->_table} SET {$sets_result} WHERE `{$this->_get_id_column()}` = '{$this->get_id()}';";

    // Update the row
    $response = db::get_instance()->exec($sql_update);

    \tcore::log($sql_update, "Updating " . get_called_class() . " id = " . $this->get_id());
  }

  /**
   * Create an INSERT statement and execute it, then reload this object from the database
   * We do this so anything that gets AUTOINCREMENT'd or DEFAULT'd in gets read back into the model.
   */
  private function _save_insert() {
    $db_name = db::get_database_name();

    // Remove the key column, for it is auto_incremented.
    $columns_array = $this->_get_columns();
    $index_of_key  = array_search($this->_get_id_column(), $columns_array);
    unset($columns_array[$index_of_key]);

    // Messilly sanitise the data input.
    $columns_values = $this->_get_values($columns_array);
    foreach ($columns_values as &$value) {
      $value = db::get_instance()->quote($value);
    }

    // Build the insert query.
    $columns    = "`" . implode("`, `", $columns_array) . "`";
    $values     = implode(", ", $columns_values);
    $sql_insert = "INSERT INTO {$db_name}.{$this->_table} ($columns) VALUES ($values);";

    // Insert the row
    $response = db::get_instance()->exec($sql_insert);

    // Set the ID of this object to be the lastInsertId from the DB layer
    $this->set_id(db::get_instance()->lastInsertId());

    \tcore::log($sql_insert, "Inserting " . get_called_class() . " id = " . $this->get_id());

    // Reload this model object.
    $this->reload();
  }

  /**
   * Load a model object by its primary key.
   *
   * @param integer|string $id
   *
   * @return \tcore\model
   */
  public function load_by_id($id) {
    $db_name    = db::get_database_name();
    $sql_select = "SELECT * FROM {$db_name}.{$this->_table} WHERE `{$this->_get_id_column()}` = '{$id}';";
    $result     = db::get_instance()->query($sql_select)->fetchObject(get_called_class());

    return $result;
  }

  /**
   * Reload this object.
   *
   * @return \tcore\model
   */
  public function reload() {
    return $this->load_by_id($this->get_id());
  }

  /**
   * Magic getter/setter logic.
   *
   * @param magic     get/set name called $name
   * @param variables passed $arguments
   *
   * @throws \exception
   * @return \tcore\\tcore\model
   */
  public function __call($name, $arguments) {
    if (substr($name, 0, 4) == "get_") {
      return $this->_get(substr($name, 4));
    }
    if (substr($name, 0, 4) == "set_") {
      return $this->_set(substr($name, 4), $arguments);
    }
    throw new \exception("Cannot execute unknown method: {$name}");
  }

  /**
   * Handle magic set operations
   *
   * @param parameter name $name
   * @param variables passed $arguments
   *
   * @throws \exception
   * @return \tcore\model
   */
  protected function _set($name, $args) {
    if (property_exists($this, $name)) {

      if ($this->$name == $args[0]) {
        // If the parameter is equal, we wont bother marking ourselves as dirty
      } else {
        $type = $this->_get_column_type($name);
        if ($type == model::TYPE_DATE) {
          $time  = strtotime($args[0]);
          $value = date("Y-m-d H:i:s", $time);
        } elseif ($type == model::TYPE_ON_OFF) {
          if ($args[0] == true) {
            $value = "On";
          } else {
            $value = "Off";
          }
        } else {
          $value = $args[0];
        }
        $this->$name  = $value;
        $this->_dirty = true;
      }

      return $this;
    } else {
      throw new \exception("Cannot set {$name} - Does not exist on object " . get_called_class());
    }
  }

  /**
   * Handle magic get operations
   *
   * @param mixed $name
   */
  protected function _get($name) {
    return $this->$name;
  }

  /**
   * Get a list of columns in this object.
   *
   * @return Array An array of column names as strings
   */
  protected function _get_columns() {
    $columns = array();
    foreach (get_object_vars($this) as $class_var => $x) {
      if (substr($class_var, 0, 1) !== "_") {
        $columns[] = $class_var;
      }
    }

    return $columns;
  }

  /**
   * Get a list of the values from the columns in the model
   *
   * @param $columns_array
   *
   * @return Array
   */
  protected function _get_values($columns_array = null) {
    if (!$columns_array) {
      $columns_array = $this->_get_columns();
    }
    $values = array();
    foreach ($columns_array as $column) {
      $values[] = $this->$column;
    }

    return $values;
  }

  /**
   * Get the name of the ID column for this model
   *
   * @throws \exception
   */
  protected function _get_id_column() {
    if (property_exists($this, '_key')) {
      return $this->_key;
    } else {
      throw new \exception("Cannot find a key column in the model for " . get_called_class());
    }
  }

  /**
   * Get the ID for this model instance.
   *
   * @return integer|string
   */
  public function get_id() {
    $id_column = $this->_get_id_column();

    return $this->$id_column;
  }

  /**
   * Set the ID for this model instance
   *
   * @param integer|string $id
   *
   * @return \tcore\model
   */
  public function set_id($id) {
    $id_column        = $this->_get_id_column();
    $this->$id_column = $id;

    return $this;
  }

  /**
   * Start a search on this type of active record
   *
   * @return \tcore\model_search
   */
  static public function search() {
    $class = get_called_class();

    return new \tcore\model_search(new $class);
  }



}

