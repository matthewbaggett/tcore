<?php
function t($string, $replacements = null){
  return view::translate($string, $replacements);
}