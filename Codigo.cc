#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = ""; // Coloque a senha do Wi-Fi se tiver
const char* mqtt_server = "broker.hivemq.com";

WiFiClient WOKWI_Client;
PubSubClient client(WOKWI_Client);

String my_payload;
String my_topic;

const int led1 = 2;
const int led2 = 15;
const int led3 = 19;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Concatenar o payload recebido em uma string
  String concatenando_payload = "";
  for (int i = 0; i < length; i++) {
    concatenando_payload += (char)payload[i];
  }
  Serial.println(concatenando_payload);

  // Atualizar variáveis globais
  my_topic = String(topic);
  my_payload = concatenando_payload;
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Tente conectar com o ID do cliente MQTT
    if (client.connect("WOKWI_Client")) {
      Serial.println("connected");
      // Se conectado, inscreve no tópico
      client.subscribe("olha-para-ca-iot-fazenda");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void wifi_on() {
  if (WiFi.status() == WL_CONNECTED)
    digitalWrite(led1, HIGH);
  else
    digitalWrite(led1, LOW);
}

void publica_mqtt() {
  // Publica o estado dos LEDs nos tópicos
  client.publish("meu_topico0011", String(digitalRead(led1) ? "ON" : "OFF").c_str());
  client.publish("meu_topico0012", String(digitalRead(led2) ? "ON" : "OFF").c_str());
  delay(1000);
}

void broker_on() {
  if (client.connected())
    digitalWrite(led2, HIGH);
  else
    digitalWrite(led2, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void recebe_dados() {
  if (my_payload == "A")
    digitalWrite(led3, HIGH);
  else if (my_payload == "B")
    digitalWrite(led3, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  wifi_on();
  broker_on();
  recebe_dados();

  client.loop();
}
