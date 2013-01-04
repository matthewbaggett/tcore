<?php 
namespace tcore;

class tcore{
	
	static $view;
	static $route;
	
	static public function run(){
		\tcore\tcore::log("tcore::run");
		self::init();
		self::route();
		self::render();
	}
	
	static public function init(){
		if(!self::$view instanceof \tcore\view){
			self::$view = new view();
			session_start();
			self::$route['controller'] = 'index';
			self::$route['action'] = 'index';
		}
	}
	
	static public function route(){
		self::log("tcore::route");

		$sans_params = explode("?",$_SERVER['REQUEST_URI'],2);
		$route = array_filter(explode("/",ltrim($sans_params[0],"/")));
		if(isset($route[0])){
			self::$route['controller'] = $route[0];
		}
		if(isset($route[1])){
			self::$route['action'] = $route[1];
		}
		
		// Process controller and action names
		$controller = '';
		$action = '';
		foreach(explode("-",self::$route['controller']) as $word){
			$controller .= ucfirst($word);
		}
		foreach(explode("-",self::$route['action']) as $word){
			$action .= ucfirst($word);
		}
		self::$route['controller'] = $controller;
		self::$route['action'] = $action;
		 
		// Finalise the route.
		$controller = "{$controller}Controller";
		$action = ucfirst(self::$route['action'])."Action";
		require_once("../controllers/{$controller}.inc");
		$oController = new $controller(self::$view);
		$oController->$action();
		
		self::$view->set_file_template("default.phtml");
		self::$view->set_page_template(strtolower(self::$route['controller']).".".strtolower(self::$route['action']).".phtml");
	}
	
	static public function render(){
		self::log("tcore::render");
		$output = self::$view->render();
		echo $output;
	}

	static function log($message){
		\fb::log($message);
	}
	
	/**
	 * @return \tcore\view
	 */
	static function get_view(){
		self::init();
		return self::$view;
	}
}