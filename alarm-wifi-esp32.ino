#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Reemplazar con sus datos de su red WIFI
const char* ssid = "TU-WIFI";
const char* password = "PASSWORD-WIFI";

// Inicializar BOT Telegram
#define BOTtoken "TELEGRAM-BOT-ID"  // Tu Bot Token (Obtener de Botfather)

// Usa @myidbot para averiguar el chat ID
// Tambien necesita hacer click en "start" antes de enviarle mensajes el bot
#define CHAT_ID "TU-CHAT-ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int buzPin = 25;
const int motionSensor = 27; // Sensor PIR
bool motionDetected = false;

// Indica cuando se detecta movimiento
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario No Autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/alarma_on") {
      bot.sendMessage(chat_id, "Alarma activada", "");
      digitalWrite(buzPin, HIGH);
    }
    
    if (text == "/alarma_off") {
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      digitalWrite(buzPin, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  pinMode(buzPin, OUTPUT);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "Movimiento detectado!!", "");
    motionDetected = false;
  }
}