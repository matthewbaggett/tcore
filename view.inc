<?php 
namespace tcore;
class view{
	private $_seo_sitename;
	private $_seo_title;
	private $_seo_author;
	private $_seo_description;
	
	private $_file_template = 'default.phtml';
	private $_page_template = 'index.index.phtml';
	
	public function get_sitename()					{ return $this->_seo_sitename;	}
	public function get_title()						{ return $this->_seo_title;	}
	public function get_author()					{ return $this->_seo_author; }
	public function get_description()				{ return $this->_seo_description; }
	public function get_generator()					{ return "Turbo Core - FourOneOne"; }
	
	public function set_sitename($sitename)			{ $this->_seo_sitename = $sitename; 		return $this; }
	public function set_title($title)				{ $this->_seo_title = $title; 				return $this; }
	public function set_author($author)				{ $this->_seo_author = $author; 			return $this; }
	public function set_description($description)	{ $this->_seo_description = $description; 	return $this; }
	
	public function set_file_template($file_template) { $this->_file_template = $file_template;	return $this; }
	public function set_page_template($page_template) { $this->_page_template = $page_template;	return $this; }
	
	public function get_file_template()				{ return dirname(__FILE__) . "/../../templates/layout/{$this->_file_template}";	}
	public function get_page_template()				{ return dirname(__FILE__) . "/../../templates/pages/{$this->_page_template}";	}
	
	public function get_body_classes()				{ return implode(" ", $this->get_body_classes_array()); }
	
	
	public function render(){
		ob_start();

		require($this->get_file_template());
		$html = ob_get_contents();
		ob_end_clean();
		@header('Content-type: text/html; charset=UTF-8') ;
		echo $html;
	}
	
	public function render_page(){
		
		require($this->get_page_template());
	}
	
	public function get_body_classes_array(){
		$route = \tcore\tcore::get_route();
		$classes = array();
		$classes[] = $route['controller'];
		$classes[] = $route['action'];
		$classes[] = $route['controller'].'-'.$route['action'];
		
		foreach($classes as &$value){
			$value = strtolower(str_replace(" ","-",$value));
		}
		return $classes;
	}
	
	public function get_w3c_validator_url(){
		return "http://validator.w3.org/check?uri={$_SERVER['HTTP_HOST']}&charset=%28detect+automatically%29&doctype=Inline&group=0";
	}
}