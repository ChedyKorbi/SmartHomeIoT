// Include required libraries
#include <DHT.h>            // Library for DHT sensor (temperature and humidity)
#include <ESP8266WiFi.h>     // Library for ESP8266 WiFi connection
#include <PubSubClient.h>    // Library for MQTT communication
#include <FirebaseESP8266.h> // Library for Firebase communication

// WiFi credentials for connecting to the local WiFi network
const char* ssid = "TORKHA";      // WiFi SSID
const char* password = "98302402"; // WiFi password

// MQTT server configuration
const char* mqttServer = "192.168.122.220";  // IP address of MQTT broker
const int mqttPort = 1884;                   // MQTT broker port

// Firebase configuration
#define FIREBASE_HOST "your_RDB_URL"           // Firebase Realtime Database URL
#define FIREBASE_API_KEY "your_firebaseRDB_key" // Firebase API key
FirebaseData firebaseData;                    // Firebase data instance
FirebaseAuth firebaseAuth;                    // Firebase authentication instance
FirebaseConfig firebaseConfig;                // Firebase config instance

// Pin configuration for various sensors and actuators
#define DHTPIN D4            // Pin connected to DHT sensor
#define DHTTYPE DHT11        // Type of DHT sensor (DHT11)
#define Pir_led_PIN  D3      // Pin connected to the PIR sensor LED
#define MOVEMENT_LED_PIN D2  // Pin for the movement indication LED
#define MQ2_PIN A0           // Pin for the gas sensor (MQ2)
#define PIR_PIN D1           // Pin for the PIR motion sensor
#define TRIG_PIN D6          // Pin for the ultrasonic sensor trigger
#define ECHO_PIN D5          // Pin for the ultrasonic sensor echo
#define toggle D7            // Pin for controlling the LED state
#define BUZZER_PIN D8        // Pin for the buzzer

// Create an instance of the DHT sensor class
DHT dht(DHTPIN, DHTTYPE);

// Sensor threshold and state variables
int gasThreshold = 200;  // Threshold for gas sensor activation (buzzer)
int gasValue = 0;        // Variable to store the gas sensor value
long duration;           // Variable for the duration of the ultrasonic pulse
int distance;            // Variable to store the distance value from the ultrasonic sensor
int pirState = LOW;      // PIR sensor state (motion detected or not)
bool ledState = LOW;     // Initial state of the LED (OFF)

// Create an instance of PubSubClient for MQTT communication
WiFiClient espClient;        // WiFi client instance for MQTT
PubSubClient client(espClient);  // MQTT client instance

// Topics for MQTT communication
const char* gasTopic = "home/sensors/gas";              // Topic for gas sensor data
const char* tempHumidTopic = "home/sensors/tempHumid";  // Topic for temperature and humidity data
const char* distanceTopic = "home/sensors/distance";    // Topic for distance data
const char* motionTopic = "home/sensors/motion";        // Topic for motion (PIR) data
const char* ledControlTopic = "home/control/led";       // Topic for controlling the LED

// Setup function runs once during startup
void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  
  // Initialize the DHT sensor
  dht.begin();

  // Pin configuration for various sensors and actuators
  pinMode(MOVEMENT_LED_PIN, OUTPUT);  // Set movement LED pin as output
  pinMode(Pir_led_PIN, OUTPUT);       // Set PIR LED pin as output
  pinMode(MQ2_PIN, INPUT);            // Set gas sensor pin as input
  pinMode(TRIG_PIN, OUTPUT);          // Set trigger pin for ultrasonic sensor as output
  pinMode(ECHO_PIN, INPUT);           // Set echo pin for ultrasonic sensor as input
  pinMode(PIR_PIN, INPUT);            // Set PIR sensor pin as input
  pinMode(toggle, OUTPUT);            // Set LED control pin as output
  pinMode(BUZZER_PIN, OUTPUT);        // Set buzzer pin as output

  // Initialize WiFi, MQTT, and Firebase
  setupWiFi();
  client.setServer(mqttServer, mqttPort);  // Set MQTT server IP and port
  client.setCallback(callback);           // Set the callback function for MQTT

  // Initialize Firebase configuration
  firebaseConfig.host = FIREBASE_HOST;          // Set Firebase host URL
  firebaseConfig.api_key = FIREBASE_API_KEY;    // Set Firebase API key
  Firebase.begin(&firebaseConfig, &firebaseAuth);  // Initialize Firebase
}

// Function to connect to WiFi
void setupWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);  // Attempt to connect to the WiFi network
  
  // Wait for the WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi.");  // WiFi connection successful
}

// Main loop function runs repeatedly
void loop() {
  // Ensure MQTT client is connected
  if (!client.connected()) {
    reconnectMQTT();  // Reconnect MQTT if disconnected
  }
  client.loop();  // Handle MQTT communication

  // Gas sensor reading and processing
  gasValue = analogRead(MQ2_PIN);  // Read gas sensor value
  digitalWrite(MOVEMENT_LED_PIN, gasValue > gasThreshold ? HIGH : LOW);  // Turn on LED if gas value exceeds threshold
  
  // Check if gas value exceeds the threshold and activate buzzer
  if (gasValue > gasThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);  // Turn on buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // Turn off buzzer
  }

  // Publish gas sensor data to MQTT and Firebase
  Serial.print("Gas sensor value: ");
  Serial.println(gasValue);
  char gasStr[10];
  itoa(gasValue, gasStr, 10);  // Convert gas value to string
  client.publish(gasTopic, gasStr);  // Publish to MQTT
  Firebase.setInt(firebaseData, "/sensors/gas", gasValue);  // Publish to Firebase

  // Temperature and humidity reading
  float temperature = dht.readTemperature();  // Read temperature
  float humidity = dht.readHumidity();       // Read humidity

  // Check if the temperature and humidity readings are valid
  if (!isnan(temperature) && !isnan(humidity)) {
    // Publish data to MQTT
    String tempHumidMessage = "Temp: " + String(temperature, 2) + " C, Humidity: " + String(humidity, 2) + " %";
    Serial.println(tempHumidMessage);
    client.publish(tempHumidTopic, tempHumidMessage.c_str());  // Publish temperature and humidity to MQTT

    // Publish data to Firebase
    Firebase.setFloat(firebaseData, "/sensors/temperature", temperature);  // Publish temperature to Firebase
    Firebase.setFloat(firebaseData, "/sensors/humidity", humidity);  // Publish humidity to Firebase
  } else {
    Serial.println("Failed to read from DHT sensor!");  // If sensor readings fail
  }

  // Ultrasonic sensor reading
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);  // Measure pulse duration
  distance = duration * 0.034 / 2;    // Calculate distance

  // Control LED based on distance
  digitalWrite(MOVEMENT_LED_PIN, distance < 100 ? HIGH : LOW);
  
  // Publish distance data to MQTT and Firebase
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  char distStr[10];
  itoa(distance, distStr, 10);  // Convert distance to string
  client.publish(distanceTopic, distStr);  // Publish to MQTT
  Firebase.setInt(firebaseData, "/sensors/distance", distance);  // Publish to Firebase

  // PIR sensor (motion detection)
  pirState = digitalRead(PIR_PIN);  // Read PIR sensor value
  digitalWrite(Pir_led_PIN, pirState == HIGH ? HIGH : LOW);  // Turn on LED if motion is detected
  client.publish(motionTopic, pirState == HIGH ? "Motion detected" : "No motion detected");  // Publish motion state to MQTT
  Serial.println(pirState == HIGH ? "Motion detected" : "No motion detected");  // Print to serial monitor
  Firebase.setString(firebaseData, "/sensors/motion", pirState == HIGH ? "Motion detected" : "No motion detected");  // Publish to Firebase

  delay(1000);  // Wait for 1 second before repeating
}

// Function to reconnect to the MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(ledControlTopic);  // Subscribe to LED control topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);  // Wait 5 seconds before retrying
    }
  }
}

// Callback function for handling incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // If the message is for controlling the LED, toggle its state
  if (strcmp(topic, ledControlTopic) == 0) {
    if ((char)payload[0] == '1') {
      digitalWrite(toggle, HIGH);  // Turn LED on
    } else {
      digitalWrite(toggle, LOW);   // Turn LED off
    }
  }
}
