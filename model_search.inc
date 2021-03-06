<?php
namespace tcore;
class model_search {
  private $model;
  private $conditions;
  private $order;
  private $limit;
  private $offset = 0;

  public function __construct($model) {
    $this->model = $model;
  }

  public function where($column, $value, $operation = null) {
    if (!$operation) {
      $operation = '=';
    }
    $this->conditions[] = new model_search_condition($column, $value, $operation);

    return $this;
  }

  public function limit($limit, $offset = 0) {
    $this->limit  = $limit;
    $this->offset = $offset;

    return $this;
  }

  public function order($column, $direction = 'DESC') {
    $this->order[] = array('column' => $column, 'direction' => $direction);

    return $this;
  }

  public function exec() {
    $db_name             = db::get_database_name();
    $digested_conditions = array();
    foreach ((array)$this->conditions as $condition) {
      $digested_conditions[] = $condition->digest();
    }
    $where = implode(" AND ", $digested_conditions);
    if (count($digested_conditions) > 0) {
      $where = "WHERE {$where}";
    }
    $sql_order = '';
    $sql_limit = '';

    // Build ORDER SQL if relevent
    if ($this->order) {
      $orders = array();
      foreach ($this->order as $order) {
        $orders[] = "{$order['column']} {$order['direction']}";
      }
      $sql_order = "ORDER BY " . implode(", ", $orders);
    }

    // Build LIMIT SQL if relevent
    if ($this->limit) {
      $sql_limit = "LIMIT {$this->offset}, {$this->limit}";
    }

    // Build the SQL strings
    $sql_search = "
			SELECT * 
			FROM {$db_name}.{$this->model->get_table()} 
			{$where}
			{$sql_order}
			{$sql_limit}";

    // Get objects
    $result = db::get_instance()->query($sql_search);

    // Catch errors
    if(intval(db::get_instance()->errorCode()) > 0){
      $err_array = db::get_instance()->errorInfo();
      throw new \Exception("SQL SEARCH ERROR #" . db::get_instance()->errorCode() . ": (" . implode(", ", $err_array) . ")");
    }

    // Prep an array for our output
    $rows = array();

    // If any result returned
    if ($result) {
      // Loop over it and extract it into the $rows array
      while ($row = $result->fetchObject(get_class($this->model))) {
        $rows[] = $row;
      }
    }

    return $rows;
  }

  public function execOne() {
    $this->limit(1);
    $results = $this->exec();
    if (isset($results[0])) {
      return $results[0];
    }

    return false;
  }
}

