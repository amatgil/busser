/*
 User (that's me, casenc! :D) Extensions
*/
  
// ============= DEFINITIONS ===============

// the actual code

const char* timeServer = "time.google.com";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, timeServer, 0, 30000);

// the function i want is actually called `now` like in the example lmao
// returns received timestamp
object *fn_now_ntp_setup (object *args, object *env) {
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (/*client.status()*/ 0 != WL_CONNECTED) {
    timeClient.begin();
  } else {
    error2(PSTR("Wifi not set-up yet! Please (wifi-connect SSID PASSWD)"));
    return nil;
  }
  return tee;
}

object *fn_now_ntp (object *args, object *env) {
  (void) env;
  timeClient.update();
  //timeClient.update();
  /*if(!getLocalTime(&timeinfo)){
    return error2(PSTR("Failed to obtain time"));
  }*/

  /*
    static unsigned long Offset;
    unsigned long now = millis()/1000;
    int nargs = listlength(args);

  // Set time
  if (nargs == 3) {
    Offset = (unsigned long)((checkinteger(first(args))*60 + checkinteger(second(args)))*60
      + checkinteger(third(args)) - now);
  } else if (nargs > 0) error2(PSTR("wrong number of arguments"));
  
  // Return time
  unsigned long secs = Offset + now;
  object *seconds = number(secs%60);
  object *minutes = number((secs/60)%60);
  object *hours = number((secs/3600)%24);
  return cons(hours, cons(minutes, cons(seconds, NULL)));
  */
  //return number(timeClient.getEpochTime());
  return number(timeClient.getEpochTime());
}

// ============= SYMBOL NAMES ===============
const char stringnow_ntp[]       PROGMEM = "now-ntp";
const char stringnow_ntp_setup[] PROGMEM = "now-ntp-setup";

// ============= DOCUMENTATION STRINGS ===============
const char docnow_ntp[] PROGMEM = "(now-ntp)\n"
  "Returns the current second (approximately) by pinging an NTP\n"
  "server, in the form of a unix timestamp";

const char docnow_ntp_setup[] PROGMEM = "(now-ntp-setup)\n"
  "Fetch data from NTP server\n"
  "ASSUMES --> WIFI <-- HAS BEEN SET-UP!! ";

// ============= SYMBOL LOOKUP TABLE ===============
const tbl_entry_t lookup_table2[] PROGMEM = {
  { stringnow_ntp,       fn_now_ntp,       0200, docnow_ntp       },
  { stringnow_ntp_setup, fn_now_ntp_setup, 0200, docnow_ntp_setup },
};

// Table cross-reference functions

tbl_entry_t *tables[] = {lookup_table, lookup_table2};
const unsigned int tablesizes[] = { arraysize(lookup_table), arraysize(lookup_table2) };

const tbl_entry_t *table (int n) {
  return tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}
