## README - Smart Home Surveillance System

---

## Table of Contents

1. **General Presentation**  
2. **Objectives and Requirements**  
3. **System Architecture**  
4. **Used Sensors and Use Cases**  
5. **Technologies and Protocols**  
6. **Node-RED Dashboard**  
7. **Challenges and Solutions**  
8. **Future Improvements**  
9. **Conclusion**

---

## 1. General Presentation

This project implements a **Smart Home Surveillance System** utilizing **IoT sensors** and **MQTT** communication. It focuses on real-time monitoring of environmental and security conditions in a home, using sensors for **gas detection**, **temperature**, **humidity**, **motion**, and **distance**.

### Main Objective

The goal is to enhance **home security** and monitor **environmental conditions** through real-time surveillance, with a **user-friendly dashboard** accessible via **Node-RED**.

---

## 2. Objectives and Requirements

### Objectives

1. **Automation of Surveillance**: Provide real-time data on environmental conditions (gas, temperature, humidity) and detect intrusions.
2. **Improved Security**: Protect residents from potential dangers (e.g., gas leaks, intrusions).
3. **Efficient Communication**: Transmit data reliably between sensors and the dashboard via MQTT.
4. **User-Friendly Dashboard**: Display real-time data and provide visual alerts for quick interpretation.

### System Requirements

- **Danger Detection**: Quickly identify hazardous gas levels.
- **Indoor Comfort**: Continuously monitor temperature and humidity levels for optimal indoor climate.
- **Security**: Detect and alert on suspicious motion, ensuring prompt response.
- **User Interface**: Present data in a simple format with gauges and graphs, along with critical alerts for immediate action.

---

## 3. System Architecture

The system is based on a modular architecture, consisting of the following components:

1. **ESP8266**: The microcontroller gathers data from sensors and sends it to the **MQTT broker**.
2. **MQTT Broker (Mosquitto)**: Acts as the communication hub, managing data distribution between sensors and the dashboard.
3. **Node-RED**: Used to create the **dashboard interface**, which displays real-time data and sends alerts when thresholds are exceeded.
4. **Sensors**:  
   - **MQ2** (Gas Sensor)  
   - **DHT11** (Temperature & Humidity Sensor)  
   - **PIR** (Motion Sensor)  
   - **Ultrasonic** (Distance Sensor)
5. **Wi-Fi Local Network**: Ensures smooth communication between all components.

### Data Flow Diagram

1. Sensors collect data and send it to the **ESP8266**.
2. The **ESP8266** sends this data to the **MQTT broker**.
3. **Node-RED**, acting as an MQTT client, subscribes to topics and displays data on the dashboard.

![Architecture Diagram](path-to-your-image.png)

---

## 4. Used Sensors and Use Cases

![Sensors Overview](https://prod-files-secure.s3.us-west-2.amazonaws.com/59326c51-4463-4275-b0f1-39e5ae86d368/608264a4-6733-432a-b1a2-6adcb92f1699/Screenshot_2024-11-13_110026.jpg)

### 4.1 MQ2 Gas Sensor

- **Description**: The **MQ2** sensor detects the presence of combustible gases such as **propane**, **butane**, and **methane**.
- **Use Case**: Used for **gas leak detection**, the system provides **visual** or **audible alerts** when gas levels exceed a critical threshold.
- **Location**: Typically placed near **kitchens** or **appliances** using gas for cooking.

### 4.2 DHT11 Temperature and Humidity Sensor

- **Description**: This sensor measures **ambient temperature** and **humidity levels** in the environment.
- **Use Case**: Monitors indoor climate for **comfort** and can trigger actions such as starting **ventilation systems** if certain thresholds are exceeded.
- **Location**: Commonly placed in **living rooms** or **bedrooms** for consistent indoor climate control.

### 4.3 PIR Motion Sensor

- **Description**: The **PIR (Passive Infrared)** motion sensor detects movement in its field of view, ideal for **security surveillance**.
- **Use Case**: Used for detecting unauthorized **motion** or intrusions, sending alerts if any unusual movement is detected in **doorways** or **hallways**.
- **Location**: Installed in **entryways**, **hallways**, or **other monitored areas**.

### 4.4 Ultrasonic Distance Sensor

- **Description**: The **ultrasonic sensor** measures distance using **sound waves**, detecting objects or movements within its range.
- **Use Case**: Used for **measuring distances** in surveillance areas, such as ensuring that no objects are blocking sensors or detecting changes in the environment.
- **Location**: Placed in areas where proximity measurements are required, such as **doorways** or **windows**.

---

## 5. Technologies and Protocols

- **IoT Sensors**: Various sensors (MQ2, DHT11, PIR, Ultrasonic) are used to gather real-time data about the home’s environmental and security status.
- **MQTT (Message Queuing Telemetry Transport)**: Lightweight messaging protocol that ensures reliable communication between **sensors**, the **MQTT broker**, and **Node-RED** dashboard. 🌐
- **Node-RED**: A powerful tool for **visual programming** and creating dashboards, using **MQTT client** to display the data on customizable gauges and alert systems.
- **ESP8266**: A **Wi-Fi enabled microcontroller** that sends sensor data to the MQTT broker, enabling wireless communication.

---

## 6. Node-RED Dashboard

The **Node-RED dashboard** is the heart of the system’s **user interface**. Here, data from all sensors is displayed visually, with real-time readings of temperature, humidity, gas levels, and motion detection.

- **Real-Time Data Display**: The dashboard shows live updates, with **gauges**, **graphs**, and **alerts** for easy monitoring.
- **Alert System**: If any sensor reading exceeds a defined threshold (e.g., high gas levels or motion detected), the dashboard sends an **alert** (visual or audible).
- **Easy Customization**: Users can adjust threshold values, monitor specific sensors, and add or remove components from the dashboard as needed.

---

## 7. Challenges and Solutions

### 7.1 **Sensor Calibration and Accuracy**
- **Challenge**: Ensuring that the sensors provide accurate readings under various environmental conditions.
- **Solution**: Calibration of sensors for **temperature**, **humidity**, and **gas detection** was performed periodically to ensure reliable data.

### 7.2 **Network Stability**
- **Challenge**: Maintaining a stable communication network between devices.
- **Solution**: Ensured that the system was running on a **reliable Wi-Fi network** with sufficient coverage and bandwidth to avoid data loss.

### 7.3 **Real-Time Alerts**
- **Challenge**: Providing real-time alerts without causing **false alarms**.
- **Solution**: Fine-tuned **sensor thresholds** and incorporated logic to avoid unnecessary alerts, ensuring only critical situations triggered notifications.

---

## 8. Future Improvements

- **Integration with Smart Home Devices**: Future iterations of the system could integrate with other **smart home devices** such as smart lights, thermostats, or security cameras for a more comprehensive surveillance solution.
- **AI-based Threat Detection**: Implementing **AI algorithms** for smarter motion detection, such as distinguishing between pets and humans, to reduce false alarms.
- **Cloud Integration**: Syncing data to the cloud for remote monitoring and long-term data storage for historical analysis and reporting.

---

## 9. Conclusion

The **Smart Home Surveillance System** leverages IoT technology to enhance **home security** and **monitor environmental conditions**. Through reliable sensor data and an intuitive **Node-RED** dashboard, homeowners can stay informed about critical conditions and receive timely alerts for potential hazards. The system is modular, scalable, and adaptable for further advancements in smart home technology. 🏠🔐🌍

---

Enjoy the peace of mind knowing your home is safe and secure, thanks to the Smart Home Surveillance System! 🚀
