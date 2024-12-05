#include <utils.hpp>

// Define global variables
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
user authorizedUsers[MAX_UIDS];
int logTimes[MAX_UIDS];
int lastTimeSpent[MAX_UIDS];
String lastAccess[MAX_UIDS];
const char *reminders[MAX_UIDS];
const char *names[MAX_UIDS];
int uidCount = 0;
int currentEmployeeIndex = 0;
ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);
DHT dht(DHTPIN, DHTTYPE);
