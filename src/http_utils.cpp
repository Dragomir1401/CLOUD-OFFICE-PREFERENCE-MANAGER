#include "utils.hpp"
#include <ArduinoJson.h>

const char *ssid = "Tenda_E21800";
const char *password = "evenneed145";
const char *serverURL = "http://192.168.0.165:5000";
const char *serverURLLogEmployees = "http://192.168.0.165:5000/log_employee";
const char *serverURLNamesReminders = "http://192.168.0.165:5000/get_users_names_reminders";

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

void sendEmployeeData(char name[], String uid, String time, float temperature, float humidity, char reminder[])
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLLogEmployees);
        http.addHeader("Content-Type", "application/json");

        // Include the time field in the JSON payload
        String jsonData = "{\"name\":\"" + String(name) + "\","
                                                          "\"uid\":\"" +
                          uid + "\","
                                "\"time\":\"" +
                          time + "\","
                                 "\"temperature\":" +
                          String(temperature) + ","
                                                "\"humidity\":" +
                          String(humidity) + ","
                                             "\"reminder\":\"" +
                          String(reminder) + "\"}";

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

// Function to fetch employee names and reminders from the server
void getEmployeeNamesAndReminders(char reminders[MAX_UIDS][100], char names[MAX_UIDS][100], int &userCount)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLNamesReminders); // Server URL
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0)
        {
            Serial.println("Data received successfully!");

            // Create a buffer to hold the response
            String payload = http.getString();

            // Parse the JSON response using ArduinoJson
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                http.end();
                return;
            }

            // Parse the JSON array into names and reminders
            userCount = doc.size(); // Set the number of users

            if (userCount > MAX_UIDS)
            {
                Serial.println("Error: Too many users in the response.");
                http.end();
                return;
            }

            for (int i = 0; i < userCount; i++)
            {
                const char *name = doc[i]["name"];
                const char *reminder = doc[i]["reminder"];

                // Copy names and reminders into char arrays (ensure memory bounds)
                strncpy(names[i], name, 100);         // Avoid buffer overflow
                strncpy(reminders[i], reminder, 100); // Avoid buffer overflow
            }

            Serial.println("Names and reminders parsed successfully.");
        }
        else
        {
            Serial.print("Error receiving data: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close the HTTP connection
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}