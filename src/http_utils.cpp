#include "utils.hpp"
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>

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

const char *cacert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDjDCCAnSgAwIBAgIUCg+kiyL1pVzbDrs/CCuTtBi6q/0wDQYJKoZIhvcNAQEL\n"
    "BQAwVTELMAkGA1UEBhMCUk8xDjAMBgNVBAgMBUJ1emF1MQ4wDAYDVQQHDAVCdXph\n"
    "dTEOMAwGA1UECgwFUFJJb1QxFjAUBgNVBAMMDTE5Mi4xNjguMC4xMzQwHhcNMjUw\n"
    "MTA0MjMxMDU1WhcNMjYwMTA0MjMxMDU1WjBVMQswCQYDVQQGEwJSTzEOMAwGA1UE\n"
    "CAwFQnV6YXUxDjAMBgNVBAcMBUJ1emF1MQ4wDAYDVQQKDAVQUklvVDEWMBQGA1UE\n"
    "AwwNMTkyLjE2OC4wLjEzNDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
    "ANLCHE/36uOTFxT/c9RPch6VbETf8Yhil6bNk9UJjZP474l7f4DX+3baPZvAjupg\n"
    "kF6L98+IvmYeeDsab85D+PZqGHw9/SbxE1V8YEt0b8BsjvStKv45tu1w7AZgPWbU\n"
    "I19jGFKVuu2SAvcN7G0iRFG9DIdgsGC3WqcElpDVALRP1k2jySsPMnPftBihiWCD\n"
    "metc7kwMxDSdyS5BT6QcYNwoRDdmlbHfbaa83Wf40VISr9p/INxDfQ3IL6OaZWYe\n"
    "Wqz4uY3dyJ04x7KokHpZVNjaCHcEHsuuadASSh+XIYRq0wcLjfs+Ag/gHb9Bp1al\n"
    "7+P4OJn2gPdsngGkUmGNXrMCAwEAAaNUMFIwCwYDVR0PBAQDAgQwMBMGA1UdJQQM\n"
    "MAoGCCsGAQUFBwMBMA8GA1UdEQQIMAaHBMCoAIYwHQYDVR0OBBYEFFJ5f/J7E2ZN\n"
    "k0FOeBAHeuhPHZX7MA0GCSqGSIb3DQEBCwUAA4IBAQBjjzZrkaq+LKIGWoxYViLe\n"
    "gujb90Q/g0NEg1WmSrEABqwz4bh+hKJrRLD77ZUqBb1wYa/gef8PSRdtJou6rFhb\n"
    "5lvz93FMt6AY0JXkxjsHImDk4H7N+/D/bkBADdG/8VOIi4I4qstBvhHltU3aUKZc\n"
    "YCbAO3cH5YAmEFQOZJTuNwK943ZvAupgR36X9GJHEgnHKJMzmv9qN3xwaJp94fEb\n"
    "I0qjSWSWG1Y0sdB7pSqI3cmzTq1HS8pCDE72SUfxErZGrSb89mOmvUE8MjktEAMD\n"
    "95euPzndy1SOJSOCYrf6Nzi/aHIZsvCGWOhs3TS9Zm+GdgUwuohySGq4sSV/9wT6\n"
    "-----END CERTIFICATE-----\n";

String ip = "192.168.0.134";
const uint16_t port = 5001;
const char *ssid = "ANDREI";
const char *password = "gomoescu";
String httpsIp = "https://" + ip;
String serverURL = httpsIp + ":5000/login_employee";
String serverURLLogInEmployees = httpsIp + ":5000/login_employee";
String serverURLLogOutEmployees = httpsIp + ":5000/logout_employee";
String serverURLAllUsersDetails = httpsIp + ":5000/get_all_users_details";
String serverURLUserGetNameById = httpsIp + ":5000/get_user_name/";
String serverURLUserGetReminderById = httpsIp + ":5000/get_user_reminder/";
String serverURLUserSetAccess = httpsIp + ":5000/set_access/";
String serverURLUserGetUserPreferences = httpsIp + ":5000/get_user_preferences/";

void handleUpdateAction(bool access, String temp, String hum, String reminder, String name)
{
    updateReceived();

    lcd.clear();
    lcd.print("Name: ");
    lcd.setCursor(0, 1);
    lcd.print(name);
    delay(2000);

    // Print on lcd the update one screen at a time
    lcd.clear();
    lcd.print("Access: ");
    lcd.setCursor(0, 1);
    lcd.print(access);
    delay(2000);

    lcd.clear();
    lcd.print("Preferences: ");
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(temp);
    lcd.print(" H: ");
    lcd.print(hum);
    delay(2000);

    lcd.clear();
    lcd.print("Reminder: ");
    lcd.setCursor(0, 1);
    lcd.print(reminder);
    delay(2000);
}

void handleUpdate()
{
    if (server.hasArg("plain"))
    {
        String payload = server.arg("plain");
        Serial.println("Received update: " + payload);

        // Parse the JSON payload
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.f_str());
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        // Extract type and message
        const char *type = doc["type"];
        bool access = doc["access"];
        const char *name = doc["name"];
        const char *temp = doc["preferences"]["temperature"];
        const char *hum = doc["preferences"]["humidity"];
        const char *reminder = doc["reminder"];

        // Handle the update
        handleUpdateAction(access, String(temp), String(hum), String(reminder), String(name));

        // Respond to the server
        server.send(200, "text/plain", "Update received");
    }
    else
    {
        server.send(400, "text/plain", "No payload received");
    }
}

void connectToWiFi()
{
    Serial.println("Connecting to WiFi...");

    // Start WiFi connection
    WiFi.begin(ssid, password);

    // Timeout mechanism to avoid infinite loop
    unsigned long startAttemptTime = millis();
    const unsigned long wifiTimeout = 15000; // 15 seconds timeout

    // Keep checking the connection status with timeout
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout)
    {
        delay(500);
        Serial.print(".");
    }

    // Check if connection was successful
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnected to WiFi!");
        Serial.print("ESP32 IP Address: ");
        Serial.println(WiFi.localIP());

        // client.setCACert(cacert);
        client.setInsecure();

        // Test HTTPS connection
        if (client.connect(ip.c_str(), port))
        {
            Serial.println("Connected to HTTPS server!");
        }
        else
        {
            Serial.println("Failed to connect to HTTPS server.");
        }

        // Define HTTP POST endpoint for updates
        server.on("/event", HTTP_POST, handleUpdate);

        // Start the local HTTP server
        server.begin();
        Serial.println("HTTP server started");
    }
    else
    {
        // Connection failed
        Serial.println("\nFailed to connect to WiFi.");
        Serial.println("Restarting...");
        ESP.restart(); // Restart the ESP32 to attempt reconnection
    }
}

void sendEmployeeData(String name, String uid, String time, float temperature, float humidity, String reminder, bool inOut)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClientSecure client;
        client.setInsecure();
        // client.setCACert(cacert);

        // Determine the appropriate endpoint based on the inOut flag
        String endpoint = inOut ? "/logout_employee/" : "/login_employee/";

        if (client.connect(ip.c_str(), port))
        {
            Serial.println("Connected to HTTPS server!");

            // Create the JSON payload
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

            // Construct the HTTP POST request
            String request = "POST " + endpoint + " HTTP/1.1\r\n";
            request += "Host: " + String(ip) + "\r\n";
            request += "Content-Type: application/json\r\n";
            request += "Content-Length: " + String(jsonData.length()) + "\r\n";
            request += "Connection: close\r\n\r\n";
            request += jsonData;

            // Send the request
            client.print(request);

            // Wait for the server response
            while (client.connected() && !client.available())
            {
                delay(100); // Small delay to wait for the response
            }

            // Read the HTTP headers
            while (client.available())
            {
                String line = client.readStringUntil('\n');
                if (line == "\r")
                {
                    // End of headers
                    break;
                }
                Serial.println(line); // Log headers for debugging
            }

            // Read the body of the response
            String response = "";
            while (client.available())
            {
                response += client.readString(); // Read the response body
            }
            Serial.println("Response body:");
            Serial.println(response);

            client.stop(); // Close the connection
        }
        else
        {
            Serial.println("Failed to connect to HTTPS server.");
        }
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}

void fetchUserPreferences(String uid, float &temperature, float &humidity)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClientSecure client;
        client.setInsecure();
        // client.setCACert(cacert);

        if (client.connect(ip.c_str(), port))
        {
            Serial.println("Connected to HTTPS server!");

            // Create the JSON payload
            StaticJsonDocument<200> doc;
            doc["id"] = uid;
            String requestBody;
            serializeJson(doc, requestBody);

            // Construct the HTTP POST request
            String request = "POST /get_user_preferences/ HTTP/1.1\r\n";
            request += "Host: " + String(ip) + "\r\n";
            request += "Content-Type: application/json\r\n";
            request += "Content-Length: " + String(requestBody.length()) + "\r\n";
            request += "Connection: close\r\n\r\n";
            request += requestBody;

            // Send the request
            client.print(request);

            // Wait for the server response
            while (client.connected() && !client.available())
            {
                delay(100); // Small delay to wait for the response
            }

            // Read the HTTP headers
            while (client.available())
            {
                String line = client.readStringUntil('\n');
                if (line == "\r")
                {
                    // End of headers
                    break;
                }
                Serial.println(line); // Log headers for debugging
            }

            // Read the body of the response
            String response = "";
            while (client.available())
            {
                response += client.readString(); // Read the response body
            }
            Serial.println("Response body:");
            Serial.println(response);

            // Parse the JSON response
            StaticJsonDocument<1024> responseDoc; // Adjust size if needed
            DeserializationError error = deserializeJson(responseDoc, response);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                return;
            }

            // Parse the preferences: temperature and humidity
            temperature = responseDoc["preferences"]["temperature"];
            humidity = responseDoc["preferences"]["humidity"];
            Serial.print("Parsed Preferences - Temperature: ");
            Serial.println(temperature);
            Serial.print("Parsed Preferences - Humidity: ");
            Serial.println(humidity);

            client.stop(); // Close the connection
        }
        else
        {
            Serial.println("Failed to connect to HTTPS server.");
        }
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}

void fillAllUsersDetails(user users_db[])
{
    Serial.println("Fetching all users details from " + serverURLAllUsersDetails);

    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClientSecure client;
        client.setInsecure();
        // client.setCACert(cacert);

        if (client.connect(ip.c_str(), port))
        {
            Serial.println("Connected to HTTPS server!");

            // Construct the HTTP GET request
            String request = "GET /get_all_users_details HTTP/1.1\r\n";
            request += "Host: " + String(ip) + "\r\n";
            request += "Connection: close\r\n\r\n";
            client.print(request);

            // Wait for the server response
            while (client.connected() && !client.available())
            {
                delay(100); // Small delay to wait for response
            }

            // Read the HTTP headers
            while (client.available())
            {
                String line = client.readStringUntil('\n');
                if (line == "\r")
                {
                    // End of headers
                    break;
                }
                Serial.println(line); // Log headers for debugging
            }

            // Read the body of the response
            String response = "";
            while (client.available())
            {
                response += client.readString(); // Read the response body
            }
            Serial.println("Response body:");
            Serial.println(response);

            // Parse the JSON response
            StaticJsonDocument<2048> doc; // Adjust size as needed
            DeserializationError error = deserializeJson(doc, response);

            if (error)
            {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.f_str());
                return;
            }

            // Parse the JSON array into users_db
            JsonArray users = doc["users"];
            int i = 0;
            for (JsonVariant user : users)
            {
                const char *name = user["name"];
                const char *reminder = user["reminder"];
                const char *uid = user["id"];
                bool access = user["access"];

                // Print user details
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
                users_db[i].lastLogTime = "";
                users_db[i].lastLogTimeInt = 0;
                i++;
                uidCount++;
            }

            client.stop(); // Close the connection
        }
        else
        {
            Serial.println("Failed to connect to HTTPS server.");
        }
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