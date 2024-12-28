
#include "utils.hpp"

void get_temperature_humidity(float &temperature, float &humidity)
{
    // Read temperature as Celsius
    temperature = dht.readTemperature();
    // Read humidity
    humidity = dht.readHumidity();

    // Check if the readings are valid
    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Print temperature and humidity
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
}

void showSettingPreferences(float currentTemp, float currentHumid, float preferedTemp, float preferedHumid)
{
    lcd.clear();
    lcd.print("Curr T: ");
    lcd.print(currentTemp);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Curr H: ");
    lcd.print(currentHumid);
    lcd.print(" %");

    delay(1500);

    lcd.clear();
    lcd.print("Pref T: ");
    lcd.print(preferedTemp);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Pref H: ");
    lcd.print(preferedHumid);
    lcd.print(" %");

    delay(1500);
    lcd.clear();

    // Print the difference between the current and preferred temperature
    float diffTemp = abs(preferedTemp - currentTemp);
    float diffHumid = abs(preferedHumid - currentHumid);

    // Truncate the diffs to 1 decimal place
    diffTemp = roundf(diffTemp * 10) / 10;
    diffHumid = roundf(diffHumid * 10) / 10;

    if (currentTemp < preferedTemp)
    {
        lcd.print("+ T with: ");
        lcd.print(diffTemp);
        lcd.print(" C");
    }
    else if (currentTemp > preferedTemp)
    {
        lcd.print("- T with: ");
        lcd.print(diffTemp);
        lcd.print("C");
    }
    else
    {
        lcd.print("T is OK");
    }
    lcd.setCursor(0, 1);

    if (currentHumid < preferedHumid)
    {
        lcd.print("+ H with: ");
        lcd.print(diffHumid);
        lcd.print("%");
    }
    else if (currentHumid > preferedHumid)
    {
        lcd.print("- H with: ");
        lcd.print(diffHumid);
        lcd.print("%");
    }
    else
    {
        lcd.print("H is OK");
    }
}