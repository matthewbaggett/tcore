<?php
// Define roots
define("TCORE_PUBLIC_ROOT", $_SERVER['DOCUMENT_ROOT'] . "/");
define("TCORE_PRIVATE_ROOT", TCORE_PUBLIC_ROOT . "../");
define("TCORE_LOGS", TCORE_PRIVATE_ROOT . "logs/");

// Require external components
require_once("../lib/firephp/lib/FirePHPCore/fb.php");

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

function attempt_define($key, $value) {
  if (!defined($key)) {
    define($key, $value);
  }
}
