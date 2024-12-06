#include "utils.hpp"
#include <ArduinoJson.h>

const char *ssid = "Tenda_E21800";
const char *password = "evenneed145";
const char *serverURL = "http://192.168.0.165:5000";
const char *serverURLLogEmployees = "http://192.168.0.165:5000/log_employee";
const char *serverURLAllUsersDetails = "http://192.168.0.165:5000/get_all_users_details";
const char *serverURLUserGetNameById = "http://192.168.0.165:5000/get_user_name/";
const char *serverURLUserGetReminderById = "http://192.168.0.165:5000/get_user_reminder/";

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

// Function to fetch employee names and reminders from the server
void fillAllUsersDetails(user users_db[]) {
    // Do a request to /get_all_users_details to get all the users details to fill the users_db array
    // data comes as "users": [] json array
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLAllUsersDetails); // Server URL
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
            JsonArray users = doc["users"];
            int i = 0;
            for (JsonVariant user : users)
            {
                const char *name = user["name"];
                const char *reminder = user["reminder"];
                const char *uid = user["uid"];
                const char *access = user["access"];
                users_db[i].name = String(name);
                users_db[i].reminder = String(reminder);
                users_db[i].uid = String(uid);
                users_db[i].hasAccess = (strcmp(access, "true") == 0);
                users_db[i].logged = false;
                users_db[i].lastTimeSpent = 0;
                users_db[i].lastLogTime = 0;
                i++;
            }
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

String get_user_name_by_id(String id)
{
    // Do a call to /get_user_name/<uid> to get the user name
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLUserGetNameById + id); // Server URL
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
                return "";
            }

            // Parse the JSON array into names and reminders
            const char *name = doc["name"];
            return String(name);
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

    return "";
}

String get_user_reminder_by_id(String id)
{
    // Do a call to /get_user_reminder/<uid> to get the user reminder
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLUserGetReminderById + id); // Server URL
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
                return "";
            }

            // Parse the JSON array into names and reminders
            const char *reminder = doc["reminder"];
            return String(reminder);
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

    return "";
}