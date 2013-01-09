<?php 
namespace tcore;

class db extends \PDO{
	static private $instance;
	
	static private $hostname;
	static private $username;
	static private $password;
	static private $database;
	
	static public function config($hostname, $username, $password, $database){
		self::$hostname = $hostname;
		self::$username = $username;
		self::$password = $password;
		self::$database = $database;
	}

	public static function get_instance(){
		if(!self::$instance){
			self::$instance = new db();
		}
		return self::$instance;
	}
	
	public static function get_database_name(){
		return self::$database;
	}
	
	function __construct(){
		parent::__construct('mysql:dbname='.self::$database.';host='.self::$hostname, self::$username, self::$password);
		$this->setAttribute(\PDO::ATTR_STATEMENT_CLASS, array('\tcore\db_statement', array($this)));
	}
	
	public function exec ($statement) {
		\tcore\tcore::log("SQL: " . $statement);
		parent::exec($statement);
	}
}

class db_statement extends \PDOStatement
{
	public $dbh;

	protected function __construct($dbh)
	{
		$this->dbh = $dbh;
		$this->setFetchMode(\PDO::FETCH_OBJ);
	}

	public function foundRows()
	{
		$rows = $this->dbh->prepare('SELECT found_rows() AS rows', array(\PDO::MYSQL_ATTR_USE_BUFFERED_QUERY => TRUE));
		$rows->execute();
		$rowsCount = $rows->fetch(\PDO::FETCH_OBJ)->rows;
		$rows->closeCursor();
		return $rowsCount;
	}
}