#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>
#include "arduino_secrets.h"
#include "modifiable.h"
#include <DHT.h>


#define DHTPIN 2  //Digital Pin 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

typedef struct {
    int R;
    int G;
    int B;
    int W;
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
    Serial.println("setup complete");
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

    sendmqtt(temperature, humidity);

    Serial.println("sent a message");
    delay(10000);
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
    //Calculate the value of RGBW, the calculation method can be replaced and changed, TEMPERATURE_SCALER, HUMIDITY_SCALER, RED, GREEN, WHITE are defined in midifiable.h, the number can be changed
    cv.R = (int)((temperature * TEMPERATURE_SCALER + humidity * HUMIDITY_SCALER) * RED); 
    cv.G = (int)((temperature * TEMPERATURE_SCALER + humidity * HUMIDITY_SCALER) * GREEN);    
    cv.B = (int)((temperature * TEMPERATURE_SCALER + humidity * HUMIDITY_SCALER) * BLUE);
    cv.W = (int)((temperature * TEMPERATURE_SCALER + humidity * HUMIDITY_SCALER) * WHITE);

    //Make sure the value of RGBW does not exceed the upper limit of 255
    if (cv.R > 255) cv.R = 255;
    if (cv.G > 255) cv.G = 255;
    if (cv.B > 255) cv.B = 255;
    if (cv.W > 255) cv.W = 255;

    return cv;
}



void sendmqtt(float temperature, float humidity){
    char mqtt_message[100];

    //get RGBW value 
    ColorValues cv = decide_message(temperature, humidity);

    //send message and change colour of pixel
    for (int i=ID_START; i <= ID_END; i++){
        sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", i, cv.R, cv.G, cv.B, cv.W);
    }
    Serial.println(mqtt_topic);
    Serial.println(mqtt_message);
    

    if (client.publish(mqtt_topic, mqtt_message)) {
        Serial.println("Message published");
    } else {
        Serial.println("Failed to publish message");
    }

}
