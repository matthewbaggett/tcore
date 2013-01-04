<?php 
namespace tcore;

class model{
	const TYPE_INT = "INT(10)";
	const TYPE_STR = "VARCHAR(255)";
	const TYPE_TEXT = "TEXT";
	const TYPE_DATE = 'DATETIME';
	const TYPE_COORD = 'DECIMAL(18,12)';
	
	protected $_table;
	private $_dirty = false;
	
	public static function factory(){
		$name = get_called_class();
		return new $name();
	}
	
	public function __construct(){
		$object_name = get_called_class();
		$this->_table = inflect::pluralize($object_name);
		$this->_table_init();
	}
	
	private function _table_exists(){
		$db = db::get_instance();
		$table_exists_check = $db->query("
				SELECT table_name
				FROM information_schema.tables
				WHERE table_schema = '".db::get_database_name()."'
				AND table_name = '{$this->_table}';
				");
		if($table_exists_check->foundRows() > 0){
			return TRUE;
		}else{
			return FALSE;
		}
	}
	
	private function _get_column_type($column){
		$type_property = "_{$column}_type";
		if(property_exists($this,$type_property)){
			$type = $this->$type_property;
		}elseif($column == $this->_get_id_column()){
			$type = model::TYPE_INT;
		}else{
			$type = model::TYPE_STR;
		}
		if(in_array($type, array(model::TYPE_INT, model::TYPE_STR, model::TYPE_TEXT, model::TYPE_DATE, model::TYPE_COORD))){
			return $type;
		}else{
			throw new \exception("Sorry, but '{$column}' is an unsupported type!");
		}
	}
	
	private function _table_construct_sql_column($column){
		$type = $this->_get_column_type($column);
		$nullable = $this->_get_id_column()==$column?'NOT NULL':'NULL';
		$auto_increment = $this->_get_id_column()==$column?'AUTO_INCREMENT':'';
		return trim("`{$column}` {$type} {$nullable} {$auto_increment}");
	}
	
	private function _table_construct_sql(){
		$db_name = db::get_database_name();
		$sql  = "CREATE TABLE {$db_name}.`{$this->_table}` (\n";
		$key = $this->_get_id_column();
		foreach($this->_get_columns() as $column){
			$sql .= "  {$this->_table_construct_sql_column($column)},\n";
		}
		$sql .= " PRIMARY KEY (`{$this->_get_id_column()}`) );\n";
		return $sql;
	}
	
	private function _table_construct(){
		$db = new db();
		$sql_construct_table = $this->_table_construct_sql();
		tcore::log($sql_construct_table);
		$db->exec($sql_construct_table);
	}
	
	private function _table_init(){
		if(!$this->_table_exists()){
			$this->_table_construct();
		}
	}
	
	public function __destruct(){
		#$this->save();
	}
	
	public function save(){
		if($this->_is_dirty()){
			$this->_save();
			
		}
		return $this;
	}
	
	private function _is_dirty(){
		if($this->_dirty){
			return TRUE;
		}
		return FALSE;
	}
	
	private function _save(){
		
		if($this->get_id()){
			$sql = $this->_save_update();
		}else{
			$sql = $this->_save_insert();
		}
		$this->_dirty = false;	
		
		return TRUE;
	}
	
	private function _save_update(){
		$db_name = db::get_database_name();
		foreach($this->_get_columns() as $column){
			$sets[] = "`$column` = '{$this->$column}'";
		}
		$sets_result = implode(", ", $sets);
		$sql_update = "UPDATE {$db_name}.{$this->_table} SET {$sets_result} WHERE `{$this->_get_id_column()}` = '{$this->get_id()}';";
		
		// Update the row
		$response = db::get_instance()->exec($sql_update);
	}
	
	private function _save_insert(){
		$db_name = db::get_database_name();
		
		// Remove the key column, for it is auto_incremented.
		$columns_array = $this->_get_columns();
		$index_of_key = array_search($this->_get_id_column(), $columns_array);
		unset($columns_array[$index_of_key]);
		
		// Build the insert query.
		$columns = "`" . implode("`, `",$columns_array) . "`";
		$values =  "'" . implode("', '",$this->_get_values($columns_array)) . "'";
		$sql_insert = "INSERT INTO {$db_name}.{$this->_table} ($columns) VALUES ($values);";	
		
		// Insert the row
		$response = db::get_instance()->exec($sql_insert);
		
		$this->set_id(db::get_instance()->lastInsertId());
		
		$this->reload();
	}
	
	public function load_by_id($id){
		$db_name = db::get_database_name();
		$sql_select = "SELECT * FROM {$db_name}.{$this->_table} WHERE `{$this->_get_id_column()}` = '{$id}';";
		$result = db::get_instance()->query($sql_select)->fetchObject(get_called_class());
		return $result;
	}
	
	public function reload(){
		return $this->load_by_id($this->get_id());
	}
	
	public function __call($name, $arguments){
		if(substr($name,0,4) == "get_"){
			return $this->_get(substr($name,4));
		}
		if(substr($name,0,4) == "set_"){
			return $this->_set(substr($name,4),$arguments);
		}
		throw new \exception("Cannot execute unknown method: {$name}");
	}
	
	protected function _set($name,$args){
		if(property_exists($this, $name)){
			if($this->$name == $args[0]){
				
			}else{
				if($this->_get_column_type($name) == model::TYPE_DATE){
					$time = strtotime($args[0]);
					$value = date("Y-m-d H:i:s", $time);
				}else{
					$value = $args[0];
				}
				$this->$name = $value;
				$this->_dirty = true;
			}
			return $this;
		}else{
			throw new \exception("Cannot set {$name} - Does not exist on object " . get_called_class());
		}
	}
	
	protected function _get($name){
		return $this->$name;
	}
	
	protected function _get_columns(){
		$columns = array();
		foreach(get_object_vars($this) as $class_var => $x){
			if(substr($class_var,0,1) !== "_"){
				$columns[] = $class_var;
			}
		}
		return $columns;
	}
	
	protected function _get_values($columns_array = null){
		if(!$columns_array){
			$columns_array = $this->_get_columns();
		}
		$values = array();
		foreach($columns_array as $column){
			$values[] = $this->$column;
		}
		return $values;
	}
	
	protected function _get_id_column(){
		if(property_exists($this, '_key')){
			return $this->_key;
		}else{
			$columns = $this->_get_columns();
		}
	}
	
	public function get_id(){
		$id_column = $this->_get_id_column();
		return $this->$id_column;
	}
	
	public function set_id($id){
		$id_column = $this->_get_id_column();
		$this->$id_column = $id;
		return $this;
	}
	
	public function search(){
		return new model_search($this);
	}	
	
	public function get_table(){
		return $this->_table;
	}
}

