#include <WiFi.h>
#include <MQTT.h>

// =============================================
// KONFIGURASI — sesuaikan sebelum upload ke ESP32
// =============================================
const char* WIFI_SSID     = "NAMA_WIFI_KAMU";      // Ganti dengan nama WiFi kamu
const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";   // Ganti dengan password WiFi kamu
const char* mqtt_server   = "broker.hivemq.com";    // MQTT broker gratis, tidak perlu diganti
const char* topic         = "USERNAME/rc/rc-car-controller/car"; // Ganti USERNAME dengan username kamu

WiFiClient espClient;
MQTTClient client;

const int in1 = 26;
const int in2 = 25;
const int in3 = 14;
const int in4 = 32;

void stopCar()  { digitalWrite(in1,LOW);  digitalWrite(in2,LOW);  digitalWrite(in3,LOW);  digitalWrite(in4,LOW); }
void forward()  { digitalWrite(in1,LOW);  digitalWrite(in2,HIGH); digitalWrite(in3,LOW);  digitalWrite(in4,HIGH); }
void backward() { digitalWrite(in1,HIGH); digitalWrite(in2,LOW);  digitalWrite(in3,HIGH); digitalWrite(in4,LOW); }
void left()     { digitalWrite(in1,LOW);  digitalWrite(in2,LOW);  digitalWrite(in3,LOW);  digitalWrite(in4,HIGH); }
void right()    { digitalWrite(in1,LOW);  digitalWrite(in2,HIGH); digitalWrite(in3,LOW);  digitalWrite(in4,LOW); }

void messageReceived(String &topic, String &payload) {
  Serial.println("Perintah: " + payload);
  if      (payload == "F") forward();
  else if (payload == "B") backward();
  else if (payload == "L") left();
  else if (payload == "R") right();
  else if (payload == "S") stopCar();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Menghubungkan ke MQTT...");
    String clientId = "MobilRC-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topic);
      Serial.println("Terhubung!");
    } else {
      Serial.println("Gagal, coba lagi 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  stopCar();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung! IP: " + WiFi.localIP().toString());

  client.begin(mqtt_server, espClient);
  client.onMessage(messageReceived);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
