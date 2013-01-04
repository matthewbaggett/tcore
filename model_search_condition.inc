<?php 
namespace tcore;
class model_search_condition{
	private $column;
	private $value;
	private $operation;
	
	public function __construct($column, $value, $operation){
		$this->column 		= $column;
		$this->value 		= $value;
		$this->operation 	= $operation;
	}
	
	public function digest(){
		return "`{$this->column}` {$this->operation} '{$this->value}'";
	}
}