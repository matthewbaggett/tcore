<?php
namespace tcore;

class tcore {

  static $view;
  static $route;

  static public function run() {

    \tcore\tcore::log("tcore::run");

    // Run initialisation
    self::init();

    // Process routing
    self::route();

    // Render
    self::render();

    // Take captured result..
    $output = ob_get_contents();
    ob_end_clean();

    // And spit it at the user.
    echo $output;
  }

  static public function configure(){
    // Start buffering output
    ob_start();

    // Load Configs.
    $machinename = strtolower(gethostname());
    if (file_exists("../config/{$machinename}.inc")) {
      require_once("../config/{$machinename}.inc");
    }
    require_once("../config/default.inc");

    foreach (scandir("../models/") as $file) {
      switch ($file) {
        case '.':
        case '..':
        case '.svn':
          break;
        default:
          $class = str_replace(".inc", "", $file);
          require_once("../models/{$file}");
          new $class();
      }
    }

  }

  static public function init() {
    if (!self::$view instanceof \tcore\view) {
      self::$view = new view();
      session_cache_expire(180);
      session_start();
      self::$route['controller'] = 'index';
      self::$route['action']     = 'index';
    }
  }

  static public function route() {
    self::log("tcore::route");

    $sans_params = explode("?", $_SERVER['REQUEST_URI'], 2);
    $route       = array_filter(explode("/", ltrim($sans_params[0], "/")));
    if (isset($route[0])) {
      self::$route['controller'] = $route[0];
    }
    if (isset($route[1])) {
      self::$route['action'] = $route[1];
    }

    // Convert underscores into hyphens..
    self::$route['controller'] = str_replace("_", "-", self::$route['controller']);
    self::$route['action']     = str_replace("_", "-", self::$route['action']);

    // Process controller, action & view names
    $controller = '';
    $action     = '';

    // Take each word of the action & controller name, and camelcase it.
    foreach (explode("-", self::$route['controller']) as $word) {
      $controller .= ucfirst($word);
    }
    foreach (explode("-", self::$route['action']) as $word) {
      $action .= ucfirst($word);
      $view_name_segments[] = $word;
    }
    $view_name = implode("-", $view_name_segments);

    // Set Controller & Action
    self::$route['controller'] = $controller;
    self::$route['action']     = $action;

    // Finalise the route.
    $controller = "{$controller}Controller";
    $action     = ucfirst(self::$route['action']) . "Action";
    require_once("../controllers/{$controller}.inc");


    // Calculate the view's file name
    $view_file = sprintf(
      '%s.%s.phtml',
      strtolower(self::$route['controller']),
      strtolower($view_name)
    );

    self::$view->set_file_template("default.phtml");
    self::$view->set_page_template($view_file);

    $oController = new $controller(self::$view);
    $oController->$action();
  }

  static public function render() {
    self::log("tcore::render");
    $output = self::$view->render();
    echo $output;
  }

  /**
   * @return \tcore\view
   */
  static function get_view() {
    self::init();

    return self::$view;
  }

  static function get_route() {
    return self::$route;
  }

  /**
   * Set a message for the user to read on next page load
   *
   * @param string $message
   * @param string $type alert|error|info|success
   *
   * @return boolean
   */
  static function set_message($message, $type = "success") {
    $oMessage               = new \StdClass();
    $oMessage->message      = $message;
    $oMessage->type         = $type;
    $_SESSION['messages'][] = $oMessage;

    return true;
  }

  static function get_messages() {
    $messages = $_SESSION['messages'];
    unset($_SESSION['messages']);

    return $messages;
  }

  static function log($item, $label = null, $named_logfile = "tcore.log") {
    \fb::log($item, $label);

    if($label){
      $log_row = "$label : " . var_export($item, true);
    }else{
      $log_row = var_export($item, true);
    }
    $timestamp = "[" . date("Y-m-d H:i:s") . "]";
    file_put_contents(TCORE_LOGS . "/{$named_logfile}", "{$timestamp} {$log_row}\n\n", FILE_APPEND);
  }
}