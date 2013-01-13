<?php 
// Require external components
require_once("../lib/firephp/lib/FirePHPCore/fb.php");
require_once("../lib/eden/library/eden.php");
require_once("../lib/eden/library/eden/oauth2.php");
require_once("../lib/eden/library/eden/google.php");

// Require internal components
require_once("db.inc");
require_once("inflect.inc");
require_once("model.inc");
require_once("model_search.inc");
require_once("model_search_condition.inc");
require_once("tcore.inc");
require_once("view.inc");

require_once("../controllers/BaseController.inc");

function attempt_define($key, $value){
	if(!defined($key)){
		define($key,$value);
	}
}

// Load Configs.
$machinename = strtolower(gethostname());
if(file_exists("../config/{$machinename}.inc")){
	require_once("../config/{$machinename}.inc");
}
require_once("../config/default.inc");

foreach(scandir("../models/") as $file){
	switch($file){
		case '.':
		case '..':
		case '.svn':
			break;
		default:
			require_once("../models/{$file}");
	}
}
