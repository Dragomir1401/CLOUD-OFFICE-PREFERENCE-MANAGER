#ifndef UTILS_HPP
#define UTILS_HPP

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

// Pin defines for ESP
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define SS_PIN 21
#define RST_PIN 22

#define RED_PIN 17   // RGB LED Red
#define GREEN_PIN 16 // RGB LED Green
#define BLUE_PIN 15   // RGB LED Blue

#define BUZZER_PIN 5 // Buzzer

#define NOTE_C4 261
#define NOTE_E4 329
#define NOTE_G4 392
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784

#define MAX_UIDS 10

#define JOYSTICK_SW_PIN 27  // Joystick Switch
#define JOYSTICK_URX_PIN 12 // Joystick URX
#define JOYSTICK_URY_PIN 14 // Joystick URY

#define RTC_CLK_PIN 33 // RTC CLK
#define RTC_DAT_PIN 32 // RTC DAT
#define RTC_RST_PIN 35 // RTC RST

#define LCD_SDA_PIN 25 // LCD SDA
#define LCD_SCL_PIN 26 // LCD SCL

#define TEMPERATURE_PIN 13 // Temperature sensor

#define ADMIN_UID "53F7CA0E"

#define LOWER_JOYSTICK_THRESHOLD 500
#define UPPER_JOYSTICK_THRESHOLD 3500

// User struct
struct user
{
    String uid;
    bool logged;
};

// Extern declarations for global variables
extern MFRC522 mfrc522;
extern LiquidCrystal_I2C lcd;
extern user authorizedUsers[MAX_UIDS];
extern int logTimes[MAX_UIDS];
extern int lastTimeSpent[MAX_UIDS];
extern String lastAccess[MAX_UIDS];
extern const char *reminders[MAX_UIDS];
extern const char *names[MAX_UIDS];
extern int uidCount;
extern int currentEmployeeIndex;
extern ThreeWire myWire;
extern RtcDS1302<ThreeWire> Rtc;

// Function declarations
int daysInMonth(int month, int year);
int dateToInt(const RtcDateTime &dt);
void printStringOnLCD(const char *message);
String convertUID(MFRC522 &mfrc522);
int uidToIndex(String uid);
void addCardAccess();
void removeCardAccess();
void showTotalNumber();
float calculateTimeSpentPercentage(int lastTimeSpent, int currentTimeSpent);
String formatSpentTime(int totalSeconds);
RtcDateTime readQuartzTime();
void setDateTime();
void displayExitTime(RtcDateTime now, int index);
void printIdle();
String timeToString(const RtcDateTime &dt);
int uidToIndexMap(String uid);
bool isAuthorizedUID(String uid);
void turnOffLEDs();
void goodbyeMelody();
void adminGoodbyeMelody();
void adminAccessMelody();
void accessGrantedMelody();
void accessDeniedMelody();

#endif // UTILS_HPP
