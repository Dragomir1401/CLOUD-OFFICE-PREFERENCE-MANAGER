
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