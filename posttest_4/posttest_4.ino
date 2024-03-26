#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "Universitas Mulawarman";
const char* password = "";

// Initialize Telegram BOT
#define BOTtoken "5049346753:AAFpcl158jbt5YBv_dD7vJUfzafPuflWc7k"  // your Bot Token (Get from Botfather)
#define DHTPIN D6
#define LED1 D1
#define LED2 D2
#define LED3 D3
#define LED4 D4

#define DHTTYPE DHT11
// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "-4195546758"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
bool ledState = LOW;

DHT dht(DHTPIN, DHTTYPE);

// Track which user controls which light
String user1ControlledLight = "1082616660";
String user2ControlledLight = "779444516";
String user3ControlledLight = "937267310";
String prevMessage = "";

void handleLEDMessage(String chat_id, String from_name) {
  String welcome = "Pilih salah satu lampu bang " + from_name + ":\n";
  welcome += prevMessage = "on" ? "Lampu akan dinyalakan:\n" : "Lampu akan dimatikan:\n";
  welcome += "/led1 untuk menyalakan LED 1 \n";
  welcome += "/led2 untuk menyalakan LED 2 \n";
  welcome += "/led3 untuk menyalakan LED 3 \n";
  welcome += "/led4 untuk menyalakan LED 4 \n";
  bot.sendMessage(chat_id, welcome, ""); 
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
 Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    Serial.println(chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Selamat datang abangku, " + from_name + ".\n";
      welcome += "Gunakan perintah dibawah ini untuk mengendalikan LED.\n\n";
      welcome += "/on untuk menyalakan LED \n";
      welcome += "/off untuk mematikan LED \n";
      welcome += "/show untuk lihat kepemilikan LED \n";
      welcome += "/dht untuk meminta status DHT (Suhu dan Kelembapan)";
      bot.sendMessage(chat_id, welcome, "");
    }

    if(text == "/on" || text == "/off") {
      handleLEDMessage(chat_id, from_name);
      prevMessage = text;
    }

    if(text == "/show") {
      String welcome = "Kepemilikan LED: \n\n";
      welcome += "LED1 milik Ananta\n";
      welcome += "LED2 milik Bayu\n";
      welcome += "LED3 milik Ilham\n";
      welcome += "LED4 MILIK KITA SEMUA";
      bot.sendMessage(chat_id, welcome, "");
    }

    // LIGHT LED
    if (prevMessage == "/on") {
      String from_id = bot.messages[i].from_id;
      if(text == "/led1") {
        if(from_id == user1ControlledLight) {
          bot.sendMessage(chat_id, "LED 1 sudah menyala abangku", "");
          digitalWrite(LED1, HIGH);
        } else {
          bot.sendMessage(chat_id, "LED 1 bukan punya kau", "");
        }
      }

      if(text == "/led2") {
        if(from_id == user2ControlledLight) {
          bot.sendMessage(chat_id, "LED 2 sudah menyala abangku", "");
          digitalWrite(LED2, HIGH);
        } else {
          bot.sendMessage(chat_id, "LED 2 bukan punya kau", "");
        }
      }

      if(text == "/led3") {
        if(from_id == user3ControlledLight) {
          bot.sendMessage(chat_id, "LED 3 sudah menyala abangku", "");
          digitalWrite(LED3, HIGH);
        } else {
          bot.sendMessage(chat_id, "LED 3 bukan punya kau", "");
        }
      }

      if(text == "/led4") {
        bot.sendMessage(chat_id, "LED 4 sudah menyala abangku", "");
        digitalWrite(LED4, HIGH);
      } 
    }

    if (prevMessage == "/off") {
      String from_id = bot.messages[i].from_id;
      if(text == "/led1") {
        if(from_id == user1ControlledLight) {
          bot.sendMessage(chat_id, "LED 1 sudah mati abangku", "");
          digitalWrite(LED1, LOW);
        } else {
          bot.sendMessage(chat_id, "LED 1 bukan punya kau", "");
        }
      }

      if(text == "/led2") {
        if(from_id == user2ControlledLight) {
          bot.sendMessage(chat_id, "LED 2 sudah mati abangku", "");
          digitalWrite(LED2, LOW);
        } else {
          bot.sendMessage(chat_id, "LED 2 bukan punya kau", "");
        }
      }

      if(text == "/led3") {
        if(from_id == user3ControlledLight) {
          bot.sendMessage(chat_id, "LED 3 sudah mati abangku", "");
          digitalWrite(LED3, LOW);
        } else {
          bot.sendMessage(chat_id, "LED 3 bukan punya kau", "");
        }
      }

      if(text == "/led4") {
        bot.sendMessage(chat_id, "LED 4 sudah mati abangku", "");
        digitalWrite(LED4, LOW);
      } 
    }

    if (text == "/dht") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      String message = "";
      message += "Suhu : "+ String(t) + "°C\n";
      message += "Kelembapan : "+ String(h) + " %";
      bot.sendMessage(chat_id, message, "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  dht.begin();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED1, ledState);
  digitalWrite(LED2, ledState);
  digitalWrite(LED3, ledState);
  digitalWrite(LED4, ledState);
  
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
