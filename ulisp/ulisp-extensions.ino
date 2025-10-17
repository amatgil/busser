/*
 User (that's me, casenc! :D) Extensions
*/
  
// ============= DEFINITIONS ===============

// the actual code

// This is a Google Trust Services cert, the root Certificate Authority that
// signed the server certificate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Jan 28 00:00:42 2028 GMT
const char *rootCACertificate =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX\n"
  "MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\n"
  "CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx\n"
  "NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\n"
  "GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0\n"
  "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube\n"
  "Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e\n"
  "WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd\n"
  "BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd\n"
  "BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN\n"
  "l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw\n"
  "Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v\n"
  "Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG\n"
  "SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ\n"
  "odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY\n"
  "+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs\n"
  "kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep\n"
  "8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1\n"
  "vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl"
  "-----END CERTIFICATE-----";


const char* timeServer = "time.google.com";
const char* ep = "https://api.tmb.cat/v1/itransit/bus/parades/1225?app_key=8504ae3a636b155724a1c7e140ee039f&app_id=4c132798";
unsigned long setClock() {
  configTime(0, 0, timeServer);

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  char buf[26];
  Serial.print(asctime_r(&timeinfo, buf));

  time_t now;
  time(&now);
  return now;
}

/*
// the function i want is actually called `now` like in the example lmao
// returns received timestamp
object *fn_now_ntp_setup (object *args, object *env) {
  if (0) error2(PSTR("Wifi not set-up yet! Please (wifi-connect SSID PASSWD)"));

  configTime(0, 0, "pool.ntp.org");

  //timeClient.begin();
  return tee;
}

object *fn_now_ntp (object *args, object *env) {
  if (0) error2(PSTR("Run (now-ntp-setup) first"));
  //timeClient.update();
  return number(timeClient.getEpochTime());
}
*/
WiFiMulti WiFiMulti;

object *fn_fetch_setup(object *args, object *env) {
  WiFi.mode(WIFI_STA);
  return number(setClock());
}

object *fn_fetch_tmb(object *args, object *env) {
  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setCACert(rootCACertificate);

    int ret_httpCode;
    String ret_payload;
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, ep)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else error2(PSTR(https.errorToString(httpCode).c_str()));
        https.end();
      } else error2(PSTR("[HTTPS] Unable to connect"));

      // End extra scoping block
    }
    delete client;
  } else error2(PSTR("Unable to create client"));

  return number(42);
}


// ============= SYMBOL NAMES ===============
/*
const char stringnow_ntp[]       PROGMEM = "now-ntp";
const char stringnow_ntp_setup[] PROGMEM = "now-ntp-setup";
*/
const char stringfetch_setup[] PROGMEM = "fetch-setup";
const char stringfetch_tmb[]   PROGMEM = "fetch-tmb";

// ============= DOCUMENTATION STRINGS ===============
/*
const char docnow_ntp[] PROGMEM = "(now-ntp)\n"
  "Returns the current second (approximately) by pinging an NTP\n"
  "server, in the form of a unix timestamp";

const char docnow_ntp_setup[] PROGMEM = "(now-ntp-setup)\n"
  "Fetch data from NTP server\n"
  "ASSUMES --> WIFI <-- HAS BEEN SET-UP!! ";
*/
const char docfetch_setup[] PROGMEM = "(fetch-setup)\n"
  "Prep the https fetcher";

const char docfetch_tmb[]   PROGMEM = "(fetch-setup)\n"
  "Prep the https fetcher";

// ============= SYMBOL LOOKUP TABLE ===============
const tbl_entry_t lookup_table2[] PROGMEM = {
  /*
  { stringnow_ntp,       fn_now_ntp,       0200, docnow_ntp       },
  { stringnow_ntp_setup, fn_now_ntp_setup, 0200, docnow_ntp_setup },
  */
  { stringfetch_setup, fn_fetch_setup, 0200, docfetch_setup },
  { stringfetch_tmb,   fn_fetch_tmb,   0200, docfetch_tmb },
};

// ============= Table cross-reference functions (no touchy!) ===============

tbl_entry_t *tables[] = {lookup_table, lookup_table2};
const unsigned int tablesizes[] = { arraysize(lookup_table), arraysize(lookup_table2) };

const tbl_entry_t *table (int n) {
  return tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}
