#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>
#include "arduino_secrets.h"
#include "modifiable.h"
#include <DHT.h>
#include <math.h>

#define DHTPIN 2  //Digital Pin 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);



typedef struct {
    int R;
    int G;
    int B;
} ColorValues;

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
int status = WL_IDLE_STATUS;

WiFiServer server(80);
WiFiClient wificlient;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

char mqtt_topic[] = MQTT_TOPIC;

void setup(){
    Serial.begin(115200);
    delay(1000);
    WiFi.setHostname(HOST_NAME);
    startWifi();
    client.setServer(mqtt_server, mqtt_port);
    //client.publish("student/CASA0014/light/33/brightness/", "{\"brightness\": 50}");
    Serial.println("setup complete");
    dht.begin();
  
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }

    if (WiFi.status() != WL_CONNECTED){
        startWifi();
    }

    client.loop();

    // get temperature & humidity 
    float temperature = dht.readTemperature(false);

    float humidity = dht.readHumidity(false);

    Serial.println(temperature);
    Serial.println(humidity);

    sendmqtt(temperature, humidity);

    Serial.println("sent a message");
    delay(1500);
}

void startWifi(){
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.print("Trying to connect to: ");
        Serial.println(ssid);
        for (int i = 0; i < n; ++i){
            String availablessid = WiFi.SSID(i);
            if (availablessid.equals(ssid)) {
                Serial.print("Connecting to ");
                Serial.println(ssid);
                WiFi.begin(ssid, password);
                while (WiFi.status() != WL_CONNECTED) {
                    delay(600);
                    Serial.print(".");
                }
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("Connected to " + String(ssid));
                    break;
                } else {
                    Serial.println("Failed to connect to " + String(ssid));
                }
            } else {
                Serial.print(availablessid);
                Serial.println(" - this network is not in my list");
            }

        }
    }


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

}

void reconnectMQTT() {
    if (WiFi.status() != WL_CONNECTED){
        startWifi();
    }

    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "LuminaSelector";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");

            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

//The readings of temperature and humidity are used to determine the RGBW of the light
ColorValues decide_message(float temperature, float humidity){
    ColorValues cv;
    //Convert to Fahrenheit and calculate the heat index
    float temperatureF = temperature * 9.0 / 5.0 + 32.0;
    float heatIndex = -42.379 
                    + 2.04901523 * temperatureF 
                    + 10.14333127 * humidity 
                    - 0.22475541 * temperatureF * humidity 
                    - 0.00683783 * temperatureF * temperatureF 
                    - 0.05481717 * humidity * humidity 
                    + 0.00122874 * temperatureF * temperatureF * humidity 
                    + 8.5282E-4 * temperatureF * humidity * humidity 
                    - 1.99E-6 * temperatureF * temperatureF * humidity * humidity;
                    
    Serial.println(heatIndex);

    // Range of Heat index
    float minRange = 80.0;
    float maxRange = 137.0;

    // Limit the heat index to the defined range
    if (heatIndex < minRange) heatIndex = minRange;
    if (heatIndex > maxRange) heatIndex = maxRange;

    float normalised = (heatIndex - minRange) / (maxRange - minRange);
    Serial.println(normalised);

   
    // cv.R = (int)(normalised * 255); 
    // cv.G = (int)((1.0 - fabs(normalised - 0.5) * 2.0) * 255); 
    // cv.B = (int)((1.0 - normalised) * 255);

    //The normalized heat index values are mapped to RGB
    //Gradient: blue (lowest) → green (lower) →yello(higer) → red (highest)
    cv.R = (int)(pow(normalised, 0.5) * 255);
    cv.G = (int)(sin(normalised * M_PI) * 255);
    cv.B = (int)(pow(1.0 - normalised, 2.0) * 255);

    //Make sure the value of RGBW does not exceed the upper limit of 255
    if (cv.R > 255) cv.R = 255;
    if (cv.G > 255) cv.G = 255;
    if (cv.B > 255) cv.B = 255;

    return cv;
}



void sendmqtt(float temperature, float humidity){
    char mqtt_message_1[100];
    char mqtt_message_2[100];
    char mqtt_message_3[100];

    //get RGB value 
    ColorValues cv = decide_message(temperature, humidity);
    
    sprintf(mqtt_message_3,"{\"brightness\": %d}", 119);

    client.publish("student/CASA0014/light/12/brightness/", mqtt_message_3);


    for (int i = ID_START; i <= ID_END/2; i++) {
        sprintf(mqtt_message_1, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}, ", i, cv.R, cv.G, cv.B, 0);
        sprintf(mqtt_message_2, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", ID_END-i , cv.R, cv.G, cv.B, 0);
        
        delay(800);


        Serial.println(mqtt_topic);
        Serial.println(mqtt_message_1);
        Serial.println(mqtt_message_2);

        if (client.publish(mqtt_topic, mqtt_message_1) && client.publish(mqtt_topic, mqtt_message_2)) {
            Serial.println("Message published");
        } else {
            Serial.println("Failed to publish message");
        }

    }
    delay(2000);
    for(int j = 0; j < 12; j++){
      // send a message to update the light
      char mqtt_message[100];
      sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": 0, \"G\": 0, \"B\": 0, \"W\": 0}", j);

      if (client.publish(mqtt_topic , mqtt_message, false)) {
        Serial.println("Message published");
      } else {
        Serial.println("Failed to publish message");
      }

    }
}

