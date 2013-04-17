<?php
function t($string, $replacements = null){
  return \tcore\view::translate($string, $replacements);
}