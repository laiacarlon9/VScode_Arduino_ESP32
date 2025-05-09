#include <Arduino.h>
#line 1 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
#include <WiFi.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include "MPU9250.h"

// Add a device ID
const int deviceId = 1; // Has to be a number

// Wi-Fi credentials
const char *ssid = "STRONG_ATRIA_AY4U_2.4";
const char *password = "3HCG3ykAUc";

// UDP settings
IPAddress receiverESP32IP(192, 168, 67, 70); // IP address of the receiver ESP32 - CHANGE THIS!
IPAddress receiverComputerIP(192, 168, 67, 66); // IP address of your computer - CHANGE THIS!
const int udpPort = 12345;

// UDP object
WiFiUDP udp;

// MPU-9250 object
MPU9250 mpu;
// Orientation data
float roll = 0.0, pitch = 0.0, yaw = 0.0;

#line 26 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void connectToWiFi();
#line 37 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void updateOrientation();
#line 45 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void sendOrientationUDP();
#line 65 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void setup();
#line 88 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void loop();
#line 26 "C:\\Users\\puigm\\OneDrive - Universitat de Barcelona\\Documents\\02_Docencia_Manel\\01_Eines_HW_SW\\03_SW\\11_Arduino_VScode\\ESP32_Tutorial\\VScode_Arduino_ESP32\\Programs\\WiFi_SocketUDP_2_ESP32\\ESP32_Sender\\ESP32_Sender.ino"
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.println("IP Address: " + WiFi.localIP().toString());
}

void updateOrientation() {
  if (mpu.update()) {
    yaw = -mpu.getYaw();
    pitch = -mpu.getPitch();
    roll = mpu.getRoll();
  }
}

void sendOrientationUDP() {
  // Prepara les dades a enviar en format JSON
  String json = "{";
  json += "\"device\":" + String(deviceId) + ",";
  json += "\"roll\":" + String(roll, 2) + ",";
  json += "\"pitch\":" + String(pitch, 2) + ",";
  json += "\"yaw\":" + String(yaw, 2);
  json += "}";
  // Print to Serial Monitor
  Serial.println("Sending data: " + json);
  // Send to ESP32
  udp.beginPacket(receiverESP32IP, udpPort);
  udp.print(json);
  udp.endPacket();
  // Send to Computer
  udp.beginPacket(receiverComputerIP, udpPort);
  udp.print(json);
  udp.endPacket();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  // Inicialitza el MPU-9250
  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }
  Serial.println("MPU connected");
  delay(2000);

  // Connecta a la xarxa Wi-Fi
  connectToWiFi();

  // Comença UDP
  udp.begin(udpPort);
  Serial.println("UDP initialized.");
}

void loop() {
  updateOrientation(); // Actualitza les dades del sensor
  sendOrientationUDP(); // Envia les dades al receptor via UDP
  delay(10); // Ajusta la freqüència d'enviament si cal
}

