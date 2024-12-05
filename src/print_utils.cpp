#include <utils.hpp>

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
