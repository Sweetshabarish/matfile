#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h>

// =====================================================
// WIFI CREDENTIALS
// =====================================================

const char* ssid = "Shabarish1";
const char* password = "h53egu4p";

// =====================================================
// WEB SERVER
// =====================================================

WebServer server(80);

// =====================================================
// PCA9685
// =====================================================

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

#define SERVO_FREQ 50
#define CH0   0
#define CH12 12
#define CH15 15

#define SERVO_MIN  600
#define SERVO_MAX  2400

// =====================================================
// VARIABLES
// =====================================================

int lastCommand = -1;

// =====================================================
// CONVERT MICROSECONDS TO PCA9685 TICKS
// =====================================================

uint16_t usToTicks(uint16_t us) {
  return (us * 4096) / 20000;
}

// =====================================================
// SERVO POSITIONS
// =====================================================

uint16_t pos180 = usToTicks(SERVO_MAX);
uint16_t pos0   = usToTicks(SERVO_MIN);

// =====================================================
// HANDLE COMMAND
// =====================================================

void handleCommand() {

  if (!server.hasArg("val")) {
    server.send(400, "text/plain", "NO COMMAND");
    return;
  }

  int cmd = server.arg("val").toInt();

  Serial.print("\n[ESP32] Received Command: ");
  Serial.println(cmd);

  // =========================================
  // IGNORE DUPLICATES
  // =========================================

  if (cmd == lastCommand) {
    Serial.println("[ESP32] Duplicate Command Ignored");
    server.send(200, "text/plain", "IGNORED");
    return;
  }

  lastCommand = cmd;

  // =========================================
  // ACTIONS
  // =========================================

  if (cmd == 3) {

    Serial.println("[ESP32] PRONATION");

    pca.setPWM(CH0, 0, pos180);

    delay(5000);
  }

  else if (cmd == 4) {

    Serial.println("[ESP32] SUPINATION");

    pca.setPWM(CH0, 0, pos0);

    delay(5000);
  }

  else if (cmd == 5) {

    Serial.println("[ESP32] HAND CLOSE");

    pca.setPWM(CH12, 0, pos0);
    pca.setPWM(CH15, 0, pos0);

    delay(5000);
  }

  else if (cmd == 6) {

    Serial.println("[ESP32] HAND OPEN");

    pca.setPWM(CH12, 0, pos180);
    pca.setPWM(CH15, 0, pos180);

    delay(5000);
  }

  else {

    Serial.println("[ESP32] INVALID / IGNORED");
  }

  Serial.println("[ESP32] DONE");

  server.send(200, "text/plain", "DONE");
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // I2C
  Wire.begin(21, 22);

  // PCA9685
  pca.begin();
  pca.setPWMFreq(SERVO_FREQ);

  delay(1000);

  // =========================================
  // WIFI
  // =========================================

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // =========================================
  // SERVER ROUTE
  // =========================================

  server.on("/cmd", handleCommand);

  server.begin();

  Serial.println("HTTP Server Started");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();
}
