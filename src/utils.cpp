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

// Function to calculate the number of days in a month
int daysInMonth(int month, int year)
{
    // Check the month and return the number of days
    switch (month)
    {
    case 1:
        return 31;
    case 2:
        return 28;
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    default:
        return 0;
    }
}

// Function to convert a date to an integer
int dateToInt(const RtcDateTime &dt)
{
  // Define the Unix epoch
  const int EPOCH_YEAR = 1970;

  // Calculate the number of seconds in each component
  int seconds = 0;

  // Calculate months
  for (int month = 1; month < dt.Month(); month++)
  {
    seconds += daysInMonth(month, dt.Year()) * 24 * 3600;
  }

  // Calculate days
  seconds += (dt.Day() - 1) * 24 * 3600;

  // Calculate hours, minutes, and seconds
  seconds += dt.Hour() * 3600;
  seconds += dt.Minute() * 60;
  seconds += dt.Second();

  return seconds;
}


// Function to print a message on the LCD
void printStringOnLCD(const char *message)
{
  // Number of characters per row
  int lcdWidth = 16;

  // Clear the LCD
  lcd.clear();

  // Print the first row
  for (int i = 0; i < lcdWidth && i < strlen(message); i++)
  {
    // Print char by char
    lcd.setCursor(i, 0);
    lcd.print(message[i]);
  }

  // Print the second row if the message is longer than the first row
  if (strlen(message) > lcdWidth)
  {
    for (int i = 0; i < lcdWidth && (lcdWidth + i) < strlen(message); i++)
    {
      // Print char by char
      lcd.setCursor(i, 1);
      lcd.print(message[lcdWidth + i]);
    }
  }
}

// Function to convert a UID to a string
String convertUID(MFRC522 &mfrc522)
{
    // Read the UID from the card
    String readUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        // Convert the UID to a string
        readUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
    }
    readUID.toUpperCase();

    return readUID;
}

// Function to convert a UID to an index based on authorized users
int uidToIndex(String uid)
{
    for (int i = 0; i < uidCount; i++)
    {
        if (authorizedUsers[i].uid == uid)
        {
            return i;
        }
    }

    return -1;
}

// Function to add card access
void addCardAccess()
{
  lcd.clear();
  lcd.print("Scan new card...");
  // Wait for a new card to be present
  while (!mfrc522.PICC_IsNewCardPresent())
  {
    // Check if the joystick goes to exit
    int joyY = analogRead(JOYSTICK_URX_PIN);
    if (joyY > UPPER_JOYSTICK_THRESHOLD)
    {
      // Exit add card mode
      lcd.clear();
      lcd.print("Exiting...");
      delay(1000);
      return;
    }

    // Wait for a new card to be present
    delay(100);
  }

  // Read the card serial
  if (mfrc522.PICC_ReadCardSerial())
  {
    // Convert the UID to a string
    String newUID = convertUID(mfrc522);
    // Get the uid associated index
    int target = uidToIndex(newUID);
    if (target >= 0)
    {
      // Check if the user already exists
      for (int index = 0; index < uidCount; index++)
      {
        if (index == target)
        {
          lcd.clear();
          lcd.print("User Already");
          lcd.setCursor(0, 1);
          lcd.print("Exists");
          delay(2000);
          return;
        }
      }
    }

    // Add the new card to the list
    authorizedUsers[uidCount].uid = newUID;
    authorizedUsers[uidCount].logged = false;
    logTimes[uidCount] = 0;
    lastTimeSpent[uidCount] = 0;
    lastAccess[uidCount] = "No Access";
    uidCount++;

    // Print the card added message
    lcd.clear();
    lcd.print("Card Added:");
    lcd.setCursor(0, 1);
    lcd.print(newUID);
    delay(2000);
  }
  else
  {
    // Print the card read error message
    lcd.clear();
    lcd.print("Card Read Error");
    delay(2000);
  }
}

// Function to remove card access
void removeCardAccess()
{
  // Print the remove card message
  lcd.clear();
  lcd.print("Scan card to");
  lcd.setCursor(0, 1);
  lcd.print("Remove");

  // Wait for a new card to be present
  while (!mfrc522.PICC_IsNewCardPresent())
  {
    // Check if the joystick goes to exit
    int joyY = analogRead(JOYSTICK_URX_PIN);
    if (joyY > UPPER_JOYSTICK_THRESHOLD)
    {
      // Exit remove card mode
      lcd.clear();
      lcd.print("Exiting...");
      delay(1000);
      return;
    }
    // Wait for a new card to be present
    delay(100);
  }

  // Read the card serial
  if (mfrc522.PICC_ReadCardSerial())
  {
    String removeUID = convertUID(mfrc522);
    // Get the uid associated index
    int index = uidToIndex(removeUID);
    if (index >= 0)
    {
      // Remove the card from the list for shifting
      for (int i = index; i < uidCount - 1; i++)
      {
        authorizedUsers[i] = authorizedUsers[i + 1];
        logTimes[i] = logTimes[i + 1];
        lastTimeSpent[i] = lastTimeSpent[i + 1];
        reminders[i] = reminders[i + 1];
      }
      uidCount--;

      // Print the card removed message
      lcd.clear();
      lcd.print("Card Removed:");
      lcd.setCursor(0, 1);
      lcd.print(removeUID);
      delay(2000);
    }
    else
    {
      // Print the user not found message
      lcd.clear();
      lcd.print("User Not Found");
      delay(2000);
    }
  }
  else
  {
    // Print the card read error message
    lcd.clear();
    lcd.print("Card Read Error");
    delay(2000);
  }
}

// Function to show the total number of users
void showTotalNumber()
{
  // Print the total number of users
  lcd.clear();
  lcd.print("Total Users: ");
  lcd.setCursor(0, 1);
  lcd.print(uidCount);

  // Wait for the joystick to exit
  while (true)
  {
    int joyY = analogRead(JOYSTICK_URX_PIN);
    if (joyY > UPPER_JOYSTICK_THRESHOLD)
    {
      // Exit total number mode
      lcd.clear();
      lcd.print("Exiting...");
      delay(1000);
      return;
    }
    delay(100);
  }
}

// Function to calculate the percentage change in time spent
float calculateTimeSpentPercentage(int lastTimeSpent, int currentTimeSpent)
{
    // If last time spent is zero
    if (lastTimeSpent == 0)
    {
        if (currentTimeSpent == 0)
        {
            // No change if both are zero
            return 0.0;
        }
        else
        {
            // If lastTimeSpent is zero and currentTimeSpent is not zero, it's 100% more
            return 100.0;
        }
    }

    // Calculate the percentage change and return it
    float percentageChange = ((float)(currentTimeSpent - lastTimeSpent) / lastTimeSpent) * 100.0;
    return percentageChange;
}

// Function to format the spent time
String formatSpentTime(int totalSeconds)
{
    // Calculate the hours, minutes, and seconds
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    char timeString[20];

    // Format the time string and return it
    snprintf(timeString, sizeof(timeString), "%02dh:%02dm:%02ds", hours, minutes, seconds);
    return String(timeString);
}

// Function to read the time from the RTC
RtcDateTime readQuartzTime()
{
    // Read the time from the RTC
    RtcDateTime now = Rtc.GetDateTime();

    // Check if the time is valid
    if (!now.IsValid())
    {
        Serial.println("RTC lost confidence in the DateTime!");
    }

    return now;
}

// Function to set the date and time
void setDateTime()
{
  // Set the date and time to the current time
  // Format: (year, month, day, hour, minute, second)
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);
}

// Function to display the exit time
void displayExitTime(RtcDateTime now, int index)
{
  // Get the current time
  int seconds = dateToInt(now);
  int spentTime = seconds - logTimes[index];

  // Convert the spent time to a string
  String spentTimeString = formatSpentTime(spentTime);
  float difference = calculateTimeSpentPercentage(lastTimeSpent[index], spentTime);

  // Set last time spent to the current time spent
  lastTimeSpent[index] = spentTime;

  // Print the spent time
  lcd.clear();
  lcd.print("Spent time ");
  lcd.setCursor(0, 1);
  lcd.print(spentTimeString);
  delay(3000);

  // Print the difference in time spent from last time
  lcd.clear();
  lcd.print("From last time ");
  lcd.setCursor(0, 1);
  lcd.print(difference);
  if (difference >= 0)
  {
    lcd.print("% increase");
  }
  else
  {
    lcd.print("% decrease");
  }
  delay(4000);
}

// Function to print the idle message
void printIdle()
{
  // Static variables to keep track of the last toggle time and the message to show
  static unsigned long lastToggleTime = 0;
  static bool showScanMessage = true;

  // Cycle the scan card message with the total number of users after 4 seconds
  if (millis() - lastToggleTime >= 4000)
  {
    // Set the last toggle time
    lastToggleTime = millis();
    showScanMessage = !showScanMessage;

    lcd.clear();
    // Print the scan card message or the total number of users
    if (showScanMessage)
    {
      lcd.print("Scan Card");
    }
    else
    {
      int loggedUsersCount = 0;
      for (int i = 0; i < uidCount; i++)
      {
        if (authorizedUsers[i].logged)
        {
          loggedUsersCount++;
        }
      }
      lcd.print("Users logged: ");
      lcd.print(loggedUsersCount);
    }
  }
}

// Convert a time to a string
String timeToString(const RtcDateTime &dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());

  return String(datestring);
}

// Function to map an UID to an index based on card users
int uidToIndexMap(String uid)
{
  if (uid == "E3E40B2F")
  {
    return 0;
  }
  else if (uid == "E37A082F")
  {
    return 1;
  }
  else if (uid == "42487441")
  {
    return 2;
  }
  else if (uid == "53F7CA0E")
  {
    return 3;
  }
  else
  {
    return -1;
  }
}

// Function to check if a UID is authorized
bool isAuthorizedUID(String uid)
{
  for (int i = 0; i < uidCount; i++)
  {
    if (authorizedUsers[i].uid == uid)
    {
      return true;
    }
  }
  return false;
}

// Function to turn off all LEDs
void turnOffLEDs()
{
  Serial.print("Setting GPIO pin: ");
  Serial.println(GREEN_PIN);
  digitalWrite(GREEN_PIN, LOW);
  Serial.print("Setting GPIO pin: ");
  Serial.println(RED_PIN);
  digitalWrite(RED_PIN, LOW);
  Serial.print("Setting GPIO pin: ");
  Serial.println(BLUE_PIN);
  digitalWrite(BLUE_PIN, LOW);
}

void turnOffLedsAndBuzzer()
{
  // Turn off all LEDs
  turnOffLEDs();

  // Turn off the buzzer
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("Setting GPIO pin: ");
  Serial.println(BUZZER_PIN);
}

void playMelody(int note, int duration, int delayP)
{
  tone(BUZZER_PIN, note, duration);
  delay(delayP);
}

// Function to play the goodbye melody
void goodbyeMelody()
{
  // Ensure buzzer and LEDs is off initially
  turnOffLedsAndBuzzer();

  // Turn the LED blue
  digitalWrite(BLUE_PIN, HIGH);

  // Play the melody
  playMelody(NOTE_G5, 200, 250);
  playMelody(NOTE_E5, 200, 250);
  playMelody(NOTE_C5, 200, 250);

  turnOffLedsAndBuzzer();
}

void turnLedYellow()
{
  digitalWrite(RED_PIN, HIGH);   // Fully turn on the red LED
  digitalWrite(GREEN_PIN, HIGH); // Fully turn on the green LED
  digitalWrite(BLUE_PIN, HIGH);   // Turn off the blue LED
}

// Function to play the admin goodbye melody
void adminGoodbyeMelody()
{
  // Turn the LED blue
  turnOffLedsAndBuzzer();

  // Turn the LED yellow
  turnLedYellow();

  // Play the melody
  playMelody(NOTE_C5, 200, 250);
  playMelody(NOTE_G5, 200, 250);
  playMelody(NOTE_E5, 200, 250);

  turnOffLedsAndBuzzer();
}


// Function to play the admin access melody
void adminAccessMelody()
{
  // Turn off all LEDs
  turnOffLedsAndBuzzer();

  // Turn the LED yellow
  turnLedYellow();

  // Play the melody
  playMelody(NOTE_G5, 250, 350);
  playMelody(NOTE_E5, 250, 350);

  // Stop leds and buzzer
  turnOffLedsAndBuzzer();
}

// Function to play the access granted melody
void accessGrantedMelody()
{
  // Ensure buzzer is off initially
  turnOffLedsAndBuzzer();
  digitalWrite(GREEN_PIN, HIGH); // Turn green LED on
  Serial.println("Setting GPIO pin: ");
  Serial.println(GREEN_PIN);

  // Play the melody
  playMelody(NOTE_G5, 250, 350);
  playMelody(NOTE_E5, 250, 350);

  turnOffLedsAndBuzzer();
}

// Function to play the access denied melody
void accessDeniedMelody()
{
  // Turn the LED red
  turnOffLedsAndBuzzer();

  // Turn the LED red
  digitalWrite(RED_PIN, HIGH);
  Serial.println("Setting GPIO pin: ");
  Serial.println(RED_PIN);

  // Play the melody
  playMelody(NOTE_G4, 150, 150);
  playMelody(NOTE_C4, 150, 150);

  turnOffLedsAndBuzzer();
}
