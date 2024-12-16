#include "utils.hpp"
#include <ArduinoJson.h>
#include <ESPmDNS.h>

String lan_ip = "";

// Function to find the server's IP dynamically using mDNS
String findServerIP(const char *hostname)
{
    IPAddress serverIP;
    if (!WiFi.hostByName("flaskserver.local", serverIP))
    {
        Serial.println("DNS resolution failed!");
    }
    else
    {
        Serial.print("Resolved IP: ");
        Serial.println(serverIP.toString());
    }
    Serial.println("mDNS responder started!");

    Serial.print("Resolving hostname: ");
    Serial.println(hostname);

    // Wait for the hostname to resolve
    serverIP = MDNS.queryHost(hostname);
    if (serverIP)
    {
        Serial.print("Resolved IP address: ");
        Serial.println(serverIP.toString());
        return serverIP.toString();
    }
    else
    {
        Serial.println("Failed to resolve hostname!");
        return "";
    }
}

String ip = "192.168.0.144";
const char *ssid = "Tenda_E21800";
const char *password = "evenneed145";
String serverURL = "http://" + ip + ":5000/login_employee";
String serverURLLogInEmployees = "http://" + ip + ":5000/login_employee";
String serverURLLogOutEmployees = "http://" + ip + ":5000/logout_employee";
String serverURLAllUsersDetails = "http://" + ip + ":5000/get_all_users_details";
String serverURLUserGetNameById = "http://" + ip + ":5000/get_user_name/";
String serverURLUserGetReminderById = "http://" + ip + ":5000/get_user_reminder/";
String serverURLUserSetAccess = "http://" + ip + ":5000/set_access/";

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
    lan_ip = WiFi.localIP().toString();
    Serial.println("\nConnected to WiFi!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected!");

}

void sendEmployeeData(String name, String uid, String time, float temperature, float humidity, String reminder, bool inOut)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        if (!inOut) {
            http.begin(serverURLLogInEmployees);
        } else {
            http.begin(serverURLLogOutEmployees);
        }
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
    Serial.println("Fetching all users details from " + serverURLAllUsersDetails);
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
                const char *uid = user["id"];
                bool access = user["access"];

                // print user details
                Serial.print("Received user details: ");
                Serial.print("Name: ");
                Serial.println(name);
                Serial.print("Reminder: ");
                Serial.println(reminder);
                Serial.print("UID: ");
                Serial.println(uid);
                Serial.print("Access: ");
                Serial.println(access);
                
                users_db[i].name = String(name);
                users_db[i].reminder = String(reminder);
                users_db[i].uid = String(uid);
                users_db[i].hasAccess = access;
                users_db[i].logged = false;
                users_db[i].lastTimeSpent = 0;
                users_db[i].lastLogTime = 0;
                i++;
                uidCount++;
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

// Function to get user name by ID using a POST request
String get_user_name_by_id(String id)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLUserGetNameById); // Server URL
        http.addHeader("Content-Type", "application/json");

        // Create a JSON payload
        StaticJsonDocument<200> doc;
        doc["id"] = id;
        String requestBody;
        serializeJson(doc, requestBody);

        // Send POST request
        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0)
        {
            Serial.println("Data received successfully!");

            // Get the response payload
            String payload = http.getString();
            StaticJsonDocument<1024> responseDoc;
            DeserializationError error = deserializeJson(responseDoc, payload);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                http.end();
                return "";
            }

            // Parse the response for the name
            const char *name = responseDoc["name"];
            http.end(); // Close the connection
            return String(name);
        }
        else
        {
            Serial.print("Error receiving data: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close the connection
    }
    else
    {
        Serial.println("WiFi not connected!");
    }

    return "";
}

// Function to get user reminder by ID using a POST request
String get_user_reminder_by_id(String id)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLUserGetReminderById); // Server URL
        http.addHeader("Content-Type", "application/json");

        // Create a JSON payload
        StaticJsonDocument<200> doc;
        doc["id"] = id;
        String requestBody;
        serializeJson(doc, requestBody);

        // Send POST request
        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0)
        {
            Serial.println("Data received successfully!");

            // Get the response payload
            String payload = http.getString();
            StaticJsonDocument<1024> responseDoc;
            DeserializationError error = deserializeJson(responseDoc, payload);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                http.end();
                return "";
            }

            // Parse the response for the reminder
            const char *reminder = responseDoc["reminder"];
            http.end(); // Close the connection
            return String(reminder);
        }
        else
        {
            Serial.print("Error receiving data: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close the connection
    }
    else
    {
        Serial.println("WiFi not connected!");
    }

    return "";
}

void set_access(String id, bool access)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverURLUserSetAccess); // Server URL
        http.addHeader("Content-Type", "application/json");

        // Create a JSON payload
        StaticJsonDocument<200> doc;
        doc["id"] = id;
        doc["access"] = access;
        String requestBody;
        serializeJson(doc, requestBody);

        // Send POST request
        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0)
        {
            Serial.println("Data received successfully!");

            // Get the response payload
            String payload = http.getString();
            StaticJsonDocument<1024> responseDoc;
            DeserializationError error = deserializeJson(responseDoc, payload);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                http.end();
                return;
            }

            // Parse the response for the reminder
            const char *reminder = responseDoc["reminder"];
            http.end(); // Close the connection
        }
        else
        {
            Serial.print("Error receiving data: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close the connection
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}