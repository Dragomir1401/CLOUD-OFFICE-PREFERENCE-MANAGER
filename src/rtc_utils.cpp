#include <utils.hpp>

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



