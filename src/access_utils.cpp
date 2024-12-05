#include <utils.hpp>

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
                strcpy(names[i], names[i + 1]);
                strcpy(reminders[i], reminders[i + 1]);
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

