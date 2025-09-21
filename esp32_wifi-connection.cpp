#include <WiFi.h>

const char* ssid     = "name";       // WiFi nomi
const char* password = "password";   // WiFi paroli

WiFiServer server(80); // to‘g‘ri klass

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi ulandi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin(); // serverni yoqish
}

void loop() {
  WiFiClient client = server.available(); // mijozni kutish
  if (!client) return;

  Serial.println("Yangi mijoz ulandi!");
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("GET /H") >= 0) digitalWrite(5, HIGH);
  if (request.indexOf("GET /L") >= 0) digitalWrite(5, LOW);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("Click <a href=\"/H\">ON</a><br>");
  client.print("Click <a href=\"/L\">OFF</a><br>");
  client.println();

  delay(1);
  client.stop();
  Serial.println("Mijoz uzildi.");
}
