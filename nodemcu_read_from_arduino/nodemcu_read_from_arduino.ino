#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

WiFiServer server(80);

SoftwareSerial mySerial(D2, D1); // RX, TX

float value1, value2, value3, value4, value5;
char* strtokIndex; 

const int signalPin = D0; // Digital 0

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW); // Start with the pin on

  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");
  
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void readSerialData() {
  if (mySerial.available() > 0) {
    String message = mySerial.readStringUntil('\n');
    Serial.print("Received from Arduino: ");
    Serial.println(message);

    char messageBuffer[message.length() + 1];
    message.toCharArray(messageBuffer, sizeof(messageBuffer));

    strtokIndex = strtok(messageBuffer, ",");
    if (strtokIndex != NULL) value1 = atof(strtokIndex);

    strtokIndex = strtok(NULL, ",");
    if (strtokIndex != NULL) value2 = atof(strtokIndex);

    strtokIndex = strtok(NULL, ",");
    if (strtokIndex != NULL) value3 = atof(strtokIndex);
    
    strtokIndex = strtok(NULL, ",");
    if (strtokIndex != NULL) value4 = atof(strtokIndex);

    strtokIndex = strtok(NULL, ",");
    if (strtokIndex != NULL) value5 = atof(strtokIndex);
  }
}

void handleClientRequest() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {

          // --- CHECK FOR ON/OFF COMMANDS ---
          if (currentLine.startsWith("GET /on")) {
            digitalWrite(signalPin, LOW);
            Serial.println("Pin 0 command: ON");
          } else if (currentLine.startsWith("GET /off")) {
            digitalWrite(signalPin, HIGH);
            Serial.println("Pin 0 command: OFF");
          }

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");

            // --- 1. ADD CSS FOR THE STATUS CIRCLE ---
            client.println("<style>");
            client.println("html, body { font-family: Arial, sans-serif; background-color: #f0f2f5; margin: 0; padding: 20px; text-align: center; }");
            client.println(".card { background: #fff; padding: 25px 35px; border-radius: 15px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); display: inline-block; margin-bottom: 20px; }");
            client.println(".data-item { font-size: 1.2em; color: #333; margin: 12px 0; text-align: left;}");
            client.println(".data-item span { font-weight: bold; color: #0056b3; }");
            client.println("h2 { color: #333; margin-top: 0; display: inline-block; vertical-align: middle; }"); // Adjusted h2 for alignment
            client.println("a { text-decoration: none; }");
            client.println(".button { display: inline-block; background-color: #007bff; color: white; padding: 12px 24px; border: none; border-radius: 5px; font-size: 1em; cursor: pointer; margin: 5px; }");
            client.println(".button-on { background-color: #28a745; }");
            client.println(".button-off { background-color: #dc3545; }");
            client.println(".status-circle { height: 18px; width: 18px; border-radius: 50%; display: inline-block; margin-left: 10px; vertical-align: middle; }");
            client.println(".status-on { background-color: #28a745; }"); // Green for ON
            client.println(".status-off { background-color: #dc3545; }"); // Gray for OFF
            client.println("</style>");

            client.println("</head>");
            client.println("<body>");

            // Sensor Data Card
            client.println("<div class='card'>");
            client.println("<h2>Arduino Sensor Data</h2>");
            client.print("<p class='data-item'>Voltage: <span>");
            client.print(value1, 2);
            client.println(" V</span></p>");
            client.print("<p class='data-item'>Humidity (Ambient): <span>");
            client.print(value2, 2);
            client.println(" %</span></p>");
            client.print("<p class='data-item'>Temp (Ambient): <span>");
            client.print(value3, 2);
            client.println(" &deg;F</span></p>");
            client.print("<p class='data-item'>Temp (Miner): <span>");
            client.print(value4, 2);
            client.println(" &deg;F</span></p>");
            client.print("<p class='data-item'>Temp (Inverter): <span>");
            client.print(value5, 2);
            client.println(" &deg;F</span></p>");
            client.println("<button class='button' onclick=\"window.location.href='/';\">Refresh Data</button>");
            client.println("</div><br>");

            // --- 2. ADD LOGIC TO DISPLAY THE STATUS CIRCLE ---
            client.println("<div class='card'>");
            client.print("<h2>Inverter Control</h2>");

            // Check pin state and print the correct color class
            if (digitalRead(signalPin) == HIGH) {
              client.println("<span class='status-circle status-off'></span>");
            } else {
              client.println("<span class='status-circle status-on'></span>");
            }

            client.println("<br>"); // Add a line break for spacing
            client.println("<a href='/on'><button class='button button-on'>ON</button></a>");
            client.println("<a href='/off'><button class='button button-off'>OFF</button></a>");
            client.println("</div>");

            client.println("</body>");
            client.println("</html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}

void loop() {
  readSerialData();
  handleClientRequest();
}