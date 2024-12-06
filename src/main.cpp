#include <utils.hpp>

// Menu flags
bool detailMode = false;   // Sets what type of mode the admin is in
bool adminFlag = false;    // Flag to indicate if admin is logged in
bool updateDisplay = true; // Flag to indicate when to update the display
int mainMenuIndex = 0;     // To keep track of the current main menu option
int menuLevel = 0;         // 0: Main Menu, 1: Employee List, 2: Employee Details


// Function to handle the admin logged in state
void adminLogged()
{
  // Index to iter through the index
  static int detailIndex = 0;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 300;
  const unsigned long navigationDelay = 1000;

  // Read the joystick values
  int joyX = analogRead(JOYSTICK_URY_PIN);
  int joyY = analogRead(JOYSTICK_URX_PIN);

  // Check if the joystick is moved
  if (joyX < LOWER_JOYSTICK_THRESHOLD && (millis() - lastDebounceTime > navigationDelay))
  {
    if (menuLevel == 0)
    {
      // Move left in main menu
      mainMenuIndex = (mainMenuIndex > 0) ? mainMenuIndex - 1 : 3;
    }
    else if (menuLevel == 1)
    {
      // Move left in employee list
      currentEmployeeIndex = (currentEmployeeIndex > 0) ? currentEmployeeIndex - 1 : uidCount - 1;
    }
    else if (menuLevel == 2)
    {
      // Cycle left through employee details
      detailIndex = (detailIndex > 0) ? detailIndex - 1 : 4;
    }
    // Set the update flag
    updateDisplay = true;
    // Update the debounce time
    lastDebounceTime = millis();
  }
  else if (joyX > UPPER_JOYSTICK_THRESHOLD && (millis() - lastDebounceTime > navigationDelay))
  {
    if (menuLevel == 0)
    {
      // Move right in main menu
      mainMenuIndex = (mainMenuIndex < 3) ? mainMenuIndex + 1 : 0;
    }
    else if (menuLevel == 1)
    {
      // Move right in employee list
      currentEmployeeIndex = (currentEmployeeIndex < uidCount - 1) ? currentEmployeeIndex + 1 : 0;
    }
    else if (menuLevel == 2)
    {
      // Cycle right through employee details
      detailIndex = (detailIndex < 4) ? detailIndex + 1 : 0;
    }
    // Set the update flag
    updateDisplay = true;
    // Update the debounce time
    lastDebounceTime = millis();
  }

  // Check if the joystick is pressed
  if (joyY < LOWER_JOYSTICK_THRESHOLD && (millis() - lastDebounceTime > debounceDelay))
  {
    if (menuLevel == 0)
    {
      // Select option in main menu
      if (mainMenuIndex == 0)
      {
        // Go to employee list
        menuLevel = 1;
      }
      else if (mainMenuIndex == 1)
      {
        addCardAccess();
      }
      else if (mainMenuIndex == 2)
      {
        removeCardAccess();
      }
      else if (mainMenuIndex == 3)
      {
        showTotalNumber();
      }
    }
    else if (menuLevel == 1)
    {
      // Go to employee details
      menuLevel = 2;
    }
    // Set the update flag
    updateDisplay = true;
    // Update the debounce time
    lastDebounceTime = millis();
  }
  else if (joyY > UPPER_JOYSTICK_THRESHOLD && (millis() - lastDebounceTime > debounceDelay))
  {
    if (menuLevel == 2)
    {
      // Go back to employee list
      menuLevel = 1;
    }
    else if (menuLevel == 1)
    {
      // Print the exit message
      lcd.clear();
      lcd.print("Exiting...");
      delay(1000);
      // Go back to main menu
      menuLevel = 0;
    }
    // Set the update flag
    updateDisplay = true;
    // Update the debounce time
    lastDebounceTime = millis();
  }

  // If the display needs to be updated
  if (updateDisplay)
  {
    // Check the menu level
    if (menuLevel == 0)
    {
      // Show main menu options
      lcd.clear();
      switch (mainMenuIndex)
      {
      case 0:
        lcd.print("See Employees");
        lcd.setCursor(3, 1);
        lcd.print("--page 1--");
        break;
      case 1:
        lcd.print("Add Access");
        lcd.setCursor(3, 1);
        lcd.print("--page 2--");
        break;
      case 2:
        lcd.print("Remove Access");
        lcd.setCursor(3, 1);
        lcd.print("--page 3--");
        break;
      case 3:
        lcd.print("Total Number");
        lcd.setCursor(3, 1);
        lcd.print("--page 4--");
        break;
      default:
        lcd.print("Default");
        break;
      }
    }
    else if (uidCount == 0)
    {
      // Show no employees message
      lcd.clear();
      lcd.print("No Employees");
      menuLevel = 1;
    }
    else if (menuLevel == 1)
    {
      // Show employee name from list
      lcd.clear();
      lcd.print(names[uidToIndexMap(authorizedUsers[currentEmployeeIndex].uid)]);
      lcd.setCursor(3, 1);
      lcd.print("--page ");
      lcd.print(currentEmployeeIndex + 1);
      lcd.print("--");
    }
    else if (menuLevel == 2)
    {
      // Show employee details
      lcd.clear();
      switch (detailIndex)
      {
      case 0:
        lcd.print("UID: ");
        lcd.print(authorizedUsers[currentEmployeeIndex].uid);
        break;

      case 1:
        lcd.print("Logged: ");
        lcd.print(authorizedUsers[currentEmployeeIndex].logged ? "Yes" : "No");
        break;

      case 2:
        lcd.print("Reminder:");
        lcd.setCursor(0, 1);
        lcd.print(reminders[currentEmployeeIndex]);
        break;

      case 3:
        lcd.print("Last access:");
        lcd.setCursor(0, 1);
        lcd.print(lastAccess[currentEmployeeIndex]);
        break;

      case 4:
      {
        lcd.print("Last time spent:");
        lcd.setCursor(0, 1);
        String spentTimeString = formatSpentTime(lastTimeSpent[currentEmployeeIndex]);
        lcd.print(spentTimeString);
        break;
      }

      default:
        lcd.print("Default");
        break;
      }
    }
    // Reset the update flag
    updateDisplay = false;
  }
}


// Setup function to initialize the system
void setup()
{
  // Start serial communication at 9600 baud.
  Serial.begin(115200);

  // Initiate I2C bus
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);

  // Initiate SPI bus
  SPI.begin();

  // Initiate MFRC522
  mfrc522.PCD_Init();

  // Initiate RTC
  Rtc.Begin();

  // Initiate LCD
  lcd.init();

  // Turn on the backlight
  lcd.backlight();

  // Set the pins for the LEDs and buzzer
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Setup the LED control
  setupLEDControl();

  // Ensure buzzer is off initially
  digitalWrite(BUZZER_PIN, HIGH);

  // Turn off all LEDs initially
  turnOffLEDs();

  // Print a message to the serial monitor
  Serial.println("Approximate your card to the reader...");

  // Set the correct date and time
  setDateTime();

  // Initialize the DHT sensor
  dht.begin();

  // Connect to WiFi
  connectToWiFi();

  // Initialize joystick pins
  pinMode(JOYSTICK_URX_PIN, INPUT);
  pinMode(JOYSTICK_URY_PIN, INPUT);
  pinMode(JOYSTICK_SW_PIN, INPUT);

  // Set the reminders and names for employees
  getEmployeeNamesAndReminders(reminders, names, uidCount);
}


// Main loop function
void loop()
{
  // Check if admin is logged in
  if (adminFlag)
  {
    adminLogged();
  }

  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent())
  {
    // Select one of the cards
    if (mfrc522.PICC_ReadCardSerial())
    {
      // Print the UID of the card
      Serial.print("Card UID:");
      String readUID = convertUID(mfrc522);
      Serial.println(readUID);

      // Check if the card is the admin card
      if (readUID == ADMIN_UID)
      {
        // If admin was not logged in, grant access
        if (!adminFlag)
        {
          Serial.println("Admin Access Granted");
          lcd.clear();
          lcd.print("Admin Access");
          lcd.setCursor(0, 1);
          lcd.print("Granted");
          adminAccessMelody();
          adminFlag = true;
          delay(2000);
        }
        // If admin was logged in, exit
        else
        {
          Serial.println("Admin Exited");
          lcd.clear();
          lcd.print("Admin Exited");
          adminGoodbyeMelody();
          adminFlag = false;
          delay(2000);
        }

        // Turn off LEDs after admin access
        turnOffLEDs();
        return;
      }

      // Don't allow other users to scan if admin is logged in
      if (adminFlag)
      {
        return;
      }

      // Compare the read UID with the stored UID
      if (isAuthorizedUID(readUID))
      {
        // Convert the UID to an index
        int index = uidToIndex(readUID);

        // Check if the user is logged in
        if (!authorizedUsers[index].logged)
        {
          RtcDateTime now = Rtc.GetDateTime();
          String nowString = timeToString(now);
          float temperature = 0, humidity = 0;
          get_temperature_humidity(temperature, humidity);

          sendEmployeeData(names[uidToIndexMap(readUID)], readUID, nowString, temperature, humidity, reminders[uidToIndexMap(readUID)]);

          // Log the time of access
          lastAccess[index] = nowString;
          Serial.println(lastAccess[index]);
          authorizedUsers[index].logged = true;
          logTimes[index] = dateToInt(now);

          // Display the access granted message
          Serial.println("Access Granted");
          lcd.clear();
          lcd.print("Access Granted");
          lcd.setCursor(0, 1);
          lcd.print(nowString);
          accessGrantedMelody();
          delay(1000);

          // Display the welcome message
          lcd.clear();
          lcd.print("Welcome");
          Serial.println("Welcome");
          lcd.setCursor(0, 1);
          lcd.print(names[uidToIndexMap(readUID)]);
          Serial.println(names[uidToIndexMap(readUID)]);
          delay(2000);

          // Display the reminder message
          lcd.clear();
          lcd.print("Reminder");
          Serial.println("Reminder");
          delay(2000);

          // Display the reminder message
          printStringOnLCD(reminders[uidToIndexMap(readUID)]);
          Serial.println(reminders[uidToIndexMap(readUID)]);
          delay(2000);

          // Display the temperature and humidity
          print_temperature_humidity(temperature, humidity);

          // Delay for 2 seconds
          delay(3000);
        }
        else
        {
          // Log the time of exit
          RtcDateTime now = Rtc.GetDateTime();
          String nowString = timeToString(now);
          Serial.println(nowString);
          authorizedUsers[index].logged = false;

          // Display the log out messages
          Serial.println("Logging out...");
          lcd.clear();
          lcd.print("Log out at ");
          lcd.setCursor(0, 1);
          lcd.print(nowString);
          goodbyeMelody();
          delay(2000);
          displayExitTime(now, index);
        }
      }
      else
      {
        // Display the access denied message
        Serial.println("Access Denied");
        lcd.clear();
        lcd.print("Access Denied");
        accessDeniedMelody();
      }
      // Turn off LEDs after access
      turnOffLEDs();
    }
  }

  if (adminFlag)
  {
    // If admin is logged in, don't print idle message
    return;
  }

  // Print idle message
  printIdle();
}
