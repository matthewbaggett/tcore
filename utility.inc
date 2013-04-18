<?php

function t($string, $replacements = null){
  return \tcore\view::translate($string, $replacements);
}

function l($name_for_link, $controller = null, $action = null){
  return \tcore\view::link($name_for_link, $controller, $action);
}

function u($controller = null, $action = null, $params = null){
  return \tcore\view::url($controller, $action, $params);
}