<?php
/*
 * Bootstrap.
 * 1. Require external components
 * 2. Define Paths.
 *
 * We will be booted from elsewhere.
 */
// Fire PHP
require_once("../lib/firephp/lib/FirePHPCore/fb.php");

// Krumo
require_once("../lib/krumo/class.krumo.php");

// Pretty exceptions
require_once("../lib/pretty-exceptions/Library/Phalcon/Utils/PrettyExceptions.php");

// Require in PHP SEC lib
require_once("../lib/tcore/lib/phpseclib/Math/BigInteger.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/AES.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/DES.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/Hash.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/Random.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/RC4.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/Rijndael.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/RSA.php");
require_once("../lib/tcore/lib/phpseclib/Crypt/TripleDES.php");
require_once("../lib/tcore/lib/phpseclib/File/ANSI.php");
require_once("../lib/tcore/lib/phpseclib/File/ASN1.php");
require_once("../lib/tcore/lib/phpseclib/File/X509.php");
require_once("../lib/tcore/lib/phpseclib/Net/SSH1.php");
require_once("../lib/tcore/lib/phpseclib/Net/SSH2.php");
require_once("../lib/tcore/lib/phpseclib/Net/SFTP.php");

// Require internal components
require_once("db.inc");
require_once("inflect.inc");
require_once("model.inc");
require_once("model_search.inc");
require_once("model_search_condition.inc");
require_once("tcore.inc");
require_once("view.inc");
require_once("utility.inc");

require_once("../controllers/BaseController.inc");

// Define roots
define("TCORE_PUBLIC_ROOT", $_SERVER['DOCUMENT_ROOT'] . "/");
define("TCORE_PRIVATE_ROOT", TCORE_PUBLIC_ROOT . "../");
define("TCORE_LOGS", TCORE_PRIVATE_ROOT . "logs/");
attempt_define("TCORE_LOGGING_FILE_ENABLED", false);
attempt_define("TCORE_LOGGING_FIREPHP_ENABLED", false);
attempt_define("TCORE_LOGGING_MAX_EVENTS_PER_PAGE", 100);

// Set up exception handling

/**
 * Sets the exception handler
 */
//set_exception_handler(function($e)
//{
//  $p = new \Phalcon\Utils\PrettyExceptions();
//  $p->handle($e);
//  exit;
//});

/**
 * Sets the error handler
 */
//set_error_handler(function($errorCode, $errorMessage, $errorFile, $errorLine)
//{
//  $p = new \Phalcon\Utils\PrettyExceptions();
//  $p->handleError($errorCode, $errorMessage, $errorFile, $errorLine);
//  exit;
//});
