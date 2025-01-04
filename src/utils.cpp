#include <utils.hpp>

// Define global variables
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
user users_db[MAX_UIDS];
int uidCount = 0;
int currentEmployeeIndex = 0;
ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);
DHT dht(DHTPIN, DHTTYPE);
String lan_ip = "";
WebServer server(80);
