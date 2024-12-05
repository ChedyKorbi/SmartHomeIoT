#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FirebaseESP8266.h>

// WiFi credentials
const char* ssid = "TORKHA";
const char* password = "98302402";

// MQTT server configuration
const char* mqttServer = "192.168.122.220";  // Update to your MQTT broker IP
const int mqttPort = 1884;  // Port for MQTT

// Firebase configuration
#define FIREBASE_HOST "https://smarthome-cbe91-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_API_KEY "nMG7aSskAxl0RpUxWsYFwv1aonZH5bXe1R7KXxsA"
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// Sensor and LED pins
#define DHTPIN D4
#define DHTTYPE DHT11
#define Pir_led_PIN  D3
#define MOVEMENT_LED_PIN D2
#define MQ2_PIN A0 
#define PIR_PIN D1
#define TRIG_PIN D6
#define ECHO_PIN D5
#define toggle D7 
#define BUZZER_PIN D8  // Buzzer pin

DHT dht(DHTPIN, DHTTYPE);

// Sensor threshold and state variables
int gasThreshold = 200;  // Gas threshold for buzzer activation
int gasValue = 0;
long duration;
int distance;
int pirState = LOW;
bool ledState = LOW;  // Initial state of the LED

// Create an instance of PubSubClient for MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Topics
const char* gasTopic = "home/sensors/gas";
const char* tempHumidTopic = "home/sensors/tempHumid";
const char* distanceTopic = "home/sensors/distance";
const char* motionTopic = "home/sensors/motion";
const char* ledControlTopic = "home/control/led";  // Topic for LED control
 
void setup() {
  Serial.begin(9600);
  dht.begin();

  // Pin setup
  pinMode(MOVEMENT_LED_PIN, OUTPUT);
  pinMode(Pir_led_PIN, OUTPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(toggle, OUTPUT);  // LED control pin
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer pin

  // Initialize WiFi, MQTT, and Firebase
  setupWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Firebase configuration
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.api_key = FIREBASE_API_KEY;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
}

void setupWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi.");
}

void loop() {
  // Ensure the MQTT client is connected
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Gas sensor reading
  gasValue = analogRead(MQ2_PIN);
  digitalWrite(MOVEMENT_LED_PIN, gasValue > gasThreshold ? HIGH : LOW);
  
  // Check if gas value exceeds threshold and activate buzzer
  if (gasValue > gasThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);  // Turn on buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
  }

  // Publish gas sensor data to MQTT and Firebase
  Serial.print("Gas sensor value: ");
  Serial.println(gasValue);
  char gasStr[10];
  itoa(gasValue, gasStr, 10);
  client.publish(gasTopic, gasStr);
  Firebase.setInt(firebaseData, "/sensors/gas", gasValue);

  // Temperature and humidity reading
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    // Publish to MQTT
    String tempHumidMessage = "Temp: " + String(temperature, 2) + " C, Humidity: " + String(humidity, 2) + " %";
    Serial.println(tempHumidMessage);
    client.publish(tempHumidTopic, tempHumidMessage.c_str());

    // Publish to Firebase
    Firebase.setFloat(firebaseData, "/sensors/temperature", temperature);
    Firebase.setFloat(firebaseData, "/sensors/humidity", humidity);
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

   digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  digitalWrite(MOVEMENT_LED_PIN, distance < 100 ? HIGH : LOW);
  
  // Publish distance data to MQTT and Firebase
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  char distStr[10];
  itoa(distance, distStr, 10);
  client.publish(distanceTopic, distStr);
  Firebase.setInt(firebaseData, "/sensors/distance", distance);

  // PIR sensor (motion detection)
  pirState = digitalRead(PIR_PIN);
  digitalWrite(Pir_led_PIN, pirState == HIGH ? HIGH : LOW);
  client.publish(motionTopic, pirState == HIGH ? "Motion detected" : "No motion detected");
  Serial.println(pirState == HIGH ? "Motion detected" : "No motion detected");
  Firebase.setString(firebaseData, "/sensors/motion", pirState == HIGH ? "Motion detected" : "No motion detected");

  delay(1000);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(ledControlTopic);  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Handle LED control based on "ON" or "OFF" message
  if (String(topic) == ledControlTopic) {
    if (message == "ON") {
      ledState = HIGH;  // Set LED to ON
      digitalWrite(toggle, ledState);
      Serial.println("LED turned ON");
    } else if (message == "OFF") {
      ledState = LOW;  // Set LED to OFF
      digitalWrite(toggle, ledState);
      Serial.println("LED turned OFF");
    }

    // Send confirmation back to the MQTT topic
    client.publish(ledControlTopic, ledState ? "ON" : "OFF");
  }
}
