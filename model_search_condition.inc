<?php
namespace tcore;
class model_search_condition {
  private $column;
  private $value;
  private $operation;

  public function __construct($column, $value, $operation) {
    $this->column    = $column;
    $this->value     = $value;
    $this->operation = $operation;
  }

  public function digest() {
    switch(strtoupper($this->operation)){
      case 'IN':
        echo "`{$this->column}` IN ( '". implode("', '", $this->value) . " ')";
        return "`{$this->column}` IN ( '". implode("', '", $this->value) . " ')";
      default:
        return "`{$this->column}` {$this->operation} '{$this->value}'";
    }

  }
}