<?php
namespace tcore;
class view {
  private $_seo_sitename;
  private $_seo_title;
  private $_seo_author;
  private $_seo_description;

  private $_file_template = 'default.phtml';
  private $_page_template = 'index.index.phtml';

  public function get_sitename() { return $this->_seo_sitename; }

  public function get_title() { return $this->_seo_title; }

  public function get_author() { return $this->_seo_author; }

  public function get_description() { return $this->_seo_description; }

  public function get_generator() { return "Turbo Core - FourOneOne"; }

  public function set_sitename($sitename) {
    $this->_seo_sitename = $sitename;

    return $this;
  }

  public function set_title($title) {
    $this->_seo_title = $title;

    return $this;
  }

  public function set_author($author) {
    $this->_seo_author = $author;

    return $this;
  }

  public function set_description($description) {
    $this->_seo_description = $description;

    return $this;
  }

  public function set_file_template($file_template) {
    $this->_file_template = $file_template;

    return $this;
  }

  public function set_page_template($page_template) {
    $this->_page_template = $page_template;

    return $this;
  }

  public function get_file_template() { return dirname(__FILE__) . "/../../templates/layout/{$this->_file_template}"; }

  public function get_page_template() { return dirname(__FILE__) . "/../../templates/pages/{$this->_page_template}"; }

  public function get_body_classes() { return implode(" ", $this->get_body_classes_array()); }


  public function render() {
    ob_start();

    require($this->get_file_template());
    $html = ob_get_contents();
    ob_end_clean();
    @header('Content-type: text/html; charset=UTF-8');
    echo $html;
  }

  public function render_page() {
    require($this->get_page_template());
  }

  public function get_body_classes_array() {
    $route     = \tcore\tcore::get_route();
    $classes   = array();
    $classes[] = $route['controller'];
    $classes[] = $route['action'];
    $classes[] = $route['controller'] . '-' . $route['action'];

    foreach ($classes as &$value) {
      $value = strtolower(str_replace(" ", "-", $value));
    }

    return $classes;
  }

  public function get_w3c_validator_url() {
    return "http://validator.w3.org/check?uri={$_SERVER['HTTP_HOST']}&charset=%28detect+automatically%29&doctype=Inline&group=0";
  }

  public function url($controller, $action, $params = null) {
    $param_strings = array();
    if (is_array($params)) {
      foreach ($params as $key => $value) {
        $param_strings[] = "{$key}={$value}";
      }
    }
    if (count($param_strings) > 0) {
      $str_params = "?" . implode("&", $param_strings);
    } else {
      $str_params = null;
    }
    if(!$action && $params !== null){
      $action = "Index";
    }
    if($action || $str_params){
      $url = "/{$controller}/{$action}/{$str_params}";
    }elseif($action && !$str_params){
      $url = "/{$controller}/{$action}";
    }else{
      $url = "/{$controller}";
    }
    return $url;
  }

  public function redirect($controller, $action, $params = null) {
    header("Location: {$this->l($controller, $action, $params)}");
    exit;
  }

  public function slugify($string){
    $string = strtolower($string);
    $string = preg_replace("/[^A-Za-z0-9 ]/", '', $string);
    $string = str_replace(" ", "-", $string);
    return $string;
  }

  public function l($name_for_link, $controller, $action = null){
    return $this->link($name_for_link, $controller, $action);
  }

  public function link($name_for_link, $controller, $action = null){
    $url = $this->url($controller,$action);
    $classes = array();
    $classes[] = $this->slugify($name_for_link);
    echo "<!-- comparisonzzz" . trim($url, "/") . " == " . trim($_SERVER['REQUEST_URI'], "/") . " -->\n";
    if(trim($url, "/") == trim($_SERVER['REQUEST_URI'], "/")){
      $classes[] = "active";
    }
    return "<a href=\"" . $url . "\" class=\"" . implode(" ", $classes) . "\">{$name_for_link}</a>\n";
  }

  public function t($string, $replacements = null){
    return view::translate($string, $replacements);
  }

  static public function translate($string, $replacements = null){
    $output = $string;
    if($replacements !== null){
      if(count($replacements) > 0){
        foreach($replacements as $original => $replacement){
          $output = str_replace($original, $replacement, $output);
        }
      }
    }
    return $output;
  }

  public function table($data){
    $table = "Table goes here.";
    $table.= "<table>";
    $table.= "  <thead>";
    $table.= "  </thead>";
    $table.= "</table>";
    return $table;
  }
}