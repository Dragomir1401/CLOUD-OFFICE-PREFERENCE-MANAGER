#include "utils.hpp"

const char *ssid = "Tenda_E21800";
const char *password = "evenneed145";
const char *serverURL = "http://192.168.0.165:5000";
const char *serverURLLogEmployees = "http://192.168.0.165:5000/log_employee";

void connectToWiFi()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected!");

}

void sendEmployeeData(String name, String uid, String time, float temperature, float humidity, String reminder)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLLogEmployees);
        http.addHeader("Content-Type", "application/json");

        // Include the time field in the JSON payload
        String jsonData = "{\"name\":\"" + name + "\","
                                                  "\"uid\":\"" +
                          uid + "\","
                                "\"time\":\"" +
                          time + "\","
                                 "\"temperature\":" +
                          String(temperature) + ","
                                                "\"humidity\":" +
                          String(humidity) + ","
                                             "\"reminder\":\"" +
                          reminder + "\"}";

        int httpResponseCode = http.POST(jsonData);
        if (httpResponseCode > 0)
        {
            Serial.println("Data sent successfully!");
            String response = http.getString();
            Serial.println(response);
        }
        else
        {
            Serial.print("Error sending data: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}
