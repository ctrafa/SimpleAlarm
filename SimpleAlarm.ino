// Load libraries
#include <Ethernet.h>
#include <PubSubClient.h>

EthernetClient ethClient;

byte mac[]    = {0x23, 0x4a, 0xbe, 0x53, 0x32, 0xfb };
char macstr[] = "234abe5632fb";

// Update this value to an appropriate open IP on your local network
byte ip[]     = {192, 168, 0, 67};
byte nameService[]     = {192, 168, 0, 6};
byte gateway[]     = {192, 168, 0, 1};
byte subnet[]     = {255, 255, 255, 0};


//Bluemix config
#define ORG ""// Your IoT Foundation Organization
#define DEVICE_TYPE "arduino"
#define DEVICE_ID "myarduino"

char SERVER[] = ORG ".messaging.internetofthings.ibmcloud.com"; //Define Bluemix server
int MQTT_PORT = 1883; //Define Bluemix server port
char CLIENT_ID[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID; //Define Bluemix client
char AUTHMETHOD[] = "use-token-auth"; //Define Bluemix authentication method
char AUTHTOKEN[] = "hxTTSTurXR?hVk@c)a"; //Define Bluemix authentication token
char PUBLISH_TOPIC[] = "iot-2/evt/status/fmt/json"; //Define Watson IoT Platform publish topic
char SUBSCRIBE_TOPIC[] = "iot-2/cmd/alert/fmt/json";//Define Watson IoT Platform subscribe topic

void messageArrived(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message Received **************");
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
}
PubSubClient client(SERVER, MQTT_PORT, messageArrived, ethClient);

int led1 = 5;
int ldrpin = A3;
int delaytime = 1000;

int ldr;

void setup()
{
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  if (Ethernet.begin(mac) == 0)  // Start in DHCP Mode
  {
    Serial.println("Failed to configure DHCP, using Static Mode");
    // If DHCP Mode failed, start in Static Mode
    Ethernet.begin(mac, ip, nameService, gateway, subnet);
  }

  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  reconnect();
}

void loop(void)
{
  if (!client.connected()) reconnect();
  readSensors();
  client.publish(PUBLISH_TOPIC, (char*) buildJson().c_str()); // Send a message to IoT Platform
  delay(delaytime);
  client.loop();
}


String buildJson() {
  String data = "{";
  data += "\n";
  data += "\"d\": {";
  data += "\n";
  data += "\"device\": \"Arduino\",";
  data += "\n";
  data += "\"ldr\": ";
  data += (int)ldr;
  data += "\n";
  data += "}";
  data += "\n";
  data += "}";
  return data;
}

void readSensors ()
{
  ldr = analogRead(ldrpin);


  Serial.println("*********************************************");
  Serial.print("LDR = ");
  Serial.println(ldr);
}

void reconnect() {



  while (!client.connected()) { // If the Mqtt Client is not connected, so try to connect
    Serial.print("MQTT connection...");
    if (client.connect(CLIENT_ID, AUTHMETHOD, AUTHTOKEN)) { // Try to connect the Mqtt Client
      Serial.println("connected");
      client.subscribe(SUBSCRIBE_TOPIC);
    } else {
      Serial.println("failed, rc=" + client.state());
     
      byte server[] = { 64, 233, 187, 99 };
      if (ethClient.connect(server, 80)) {
        Serial.println("trying to connected to google.com");
      } else {
        Serial.println("unable to connect to google.com");
      }
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
}
