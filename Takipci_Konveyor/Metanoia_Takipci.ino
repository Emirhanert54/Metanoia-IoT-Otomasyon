#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
 const char* ssid = "Emirhan";
const char* password = "Emirhan54";
const char* mqtt_server = "broker.hivemq.com";
 WiFiClient espClient;
PubSubClient client(espClient);
 Servo govdeServo;
Servo omuzServo;
Servo dirsekServo;
Servo kiskacServo;
 const int govdePin = 25;
const int omuzPin = 26;
const int dirsekPin = 27;
const int kiskacPin = 16;
 const int potGovde = 32;
const int potOmuz = 33;
const int potDirsek = 34;
const int potKiskac = 35;
 // MERKEZ KONUM
int aciGovde = 57;
int aciOmuz = 180;
int aciDirsek = 111;
int aciKiskac = 1;
 bool otomatikCalisiyor = false;
bool potModuAktif = true;
 void logYaz(String mesaj) {
  client.publish("metanoia/sistem_log", mesaj.c_str());
}
 void aciYayinla() {
  client.publish("metanoia/aci/govde", String(aciGovde).c_str());
  client.publish("metanoia/aci/omuz", String(aciOmuz).c_str());
  client.publish("metanoia/aci/dirsek", String(aciDirsek).c_str());
  client.publish("metanoia/aci/kiskac", String(aciKiskac).c_str());
}
 void servoYaz(Servo &servo, int &aciDegiskeni, int yeniAci) {
  yeniAci = constrain(yeniAci, 0, 180);
  servo.write(yeniAci);
  aciDegiskeni = yeniAci;
  aciYayinla();
  delay(20);
}
 void merkezeDon() {
  servoYaz(govdeServo, aciGovde, 57);
  servoYaz(omuzServo, aciOmuz, 180);
  servoYaz(dirsekServo, aciDirsek, 111);
  servoYaz(kiskacServo, aciKiskac, 1);
}
 void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("WiFi baglaniyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi baglandi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
 void otomatikSekans() {
  otomatikCalisiyor = true;
  potModuAktif = false;
  logYaz("Robot kol otomatik isleme basladi.");
  // 1) Merkez
  logYaz("Robot kol merkez konuma geliyor.");
  merkezeDon();
  delay(1000);
  // 2) Egilme / nesneye yaklasma
  logYaz("Robot kol nesneye yaklasiyor.");
  servoYaz(govdeServo, aciGovde, 61);
  servoYaz(omuzServo, aciOmuz, 143);
  servoYaz(dirsekServo, aciDirsek, 109);
  servoYaz(kiskacServo, aciKiskac, 1);
  delay(1200);
  // 3) Govdeyi dondurerek nesneyi ittir
  logYaz("Robot kol nesneyi ittiriyor.");
  servoYaz(govdeServo, aciGovde, 106);
  servoYaz(omuzServo, aciOmuz, 133);
  servoYaz(dirsekServo, aciDirsek, 103);
  servoYaz(kiskacServo, aciKiskac, 1);
  delay(1500);
  // 4) Tekrar merkez
  logYaz("Robot kol tekrar merkez konuma donuyor.");
  merkezeDon();
  delay(1000);
  logYaz("Robot kol isini bitirdi. Bant calisabilir.");
  client.publish("metanoia/durum", "BITTI");
  otomatikCalisiyor = false;
  potModuAktif = true;
}
 void manuelPotKontrol() {
  if (!potModuAktif || otomatikCalisiyor) return;
  int yeniGovde = map(analogRead(potGovde), 0, 4095, 0, 180);
  int yeniOmuz = map(analogRead(potOmuz), 0, 4095, 0, 180);
  int yeniDirsek = map(analogRead(potDirsek), 0, 4095, 0, 180);
  int yeniKiskac = map(analogRead(potKiskac), 0, 4095, 0, 180);
  if (abs(yeniGovde - aciGovde) > 5) servoYaz(govdeServo, aciGovde, yeniGovde);
  if (abs(yeniOmuz - aciOmuz) > 5) servoYaz(omuzServo, aciOmuz, yeniOmuz);
  if (abs(yeniDirsek - aciDirsek) > 5) servoYaz(dirsekServo, aciDirsek, yeniDirsek);
  if (abs(yeniKiskac - aciKiskac) > 5) servoYaz(kiskacServo, aciKiskac, yeniKiskac);
}
 void callback(char* topic, byte* payload, unsigned int length) {
  String mesaj = "";
  for (int i = 0; i < length; i++) {
    mesaj += (char)payload[i];
  }
  mesaj.trim();
  String kanal = String(topic);
  Serial.print("MQTT geldi | Topic: ");
  Serial.print(kanal);
  Serial.print(" | Mesaj: ");
 Serial.println(mesaj);
  if (kanal == "metanoia/komut" && mesaj == "AL") {
    otomatikSekans();
  }
  else if (kanal == "metanoia/mod") {
    if (mesaj == "POT") {
      potModuAktif = true;
      logYaz("Potansiyometre modu aktif.");
    }
  else if (mesaj == "NODE") {
      potModuAktif = false;
      logYaz("Node-RED kontrol modu aktif.");
    }
  }
  else if (kanal == "metanoia/kontrol/govde") {
    potModuAktif = false;
    servoYaz(govdeServo, aciGovde, mesaj.toInt());
  }
  else if (kanal == "metanoia/kontrol/omuz") {
    potModuAktif = false;
    servoYaz(omuzServo, aciOmuz, mesaj.toInt());
  }
  else if (kanal == "metanoia/kontrol/dirsek") {
    potModuAktif = false;
    servoYaz(dirsekServo, aciDirsek, mesaj.toInt());
  }
  else if (kanal == "metanoia/kontrol/kiskac") {
    potModuAktif = false;
    servoYaz(kiskacServo, aciKiskac, mesaj.toInt());
  }
}
 void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT baglaniyor... ");
   String clientId = "MetanoiaRobotKol-";
    clientId += String(random(1000, 9999));
 if (client.connect(clientId.c_str())) {
      Serial.println("baglandi");
       client.subscribe("metanoia/komut");
      client.subscribe("metanoia/mod");
      client.subscribe("metanoia/kontrol/govde");
      client.subscribe("metanoia/kontrol/omuz");
      client.subscribe("metanoia/kontrol/dirsek");
      client.subscribe("metanoia/kontrol/kiskac");
      logYaz("Robot kol ESP32 baglandi.");
      merkezeDon();
      aciYayinla();
    }
    else {
      Serial.print("hata: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}
 void setup() {
  Serial.begin(115200);
  pinMode(potGovde, INPUT);
  pinMode(potOmuz, INPUT);
  pinMode(potDirsek, INPUT);
  pinMode(potKiskac, INPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  govdeServo.setPeriodHertz(50);
  omuzServo.setPeriodHertz(50);
  dirsekServo.setPeriodHertz(50);
  kiskacServo.setPeriodHertz(50);
  govdeServo.attach(govdePin, 500, 2400);
  omuzServo.attach(omuzPin, 500, 2400);
  dirsekServo.attach(dirsekPin, 500, 2400);
  kiskacServo.attach(kiskacPin, 500, 2400);
  merkezeDon();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
   client.loop();
  manuelPotKontrol();
  delay(20);



 //Konveyör Bant Sistemi Kodu
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Emirhan";
const char* password = "Emirhan54";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = 5;
const int echoPin = 18;
const int in3Pin = 26;
const int in4Pin = 27;

const int ESIK_MESAFE = 15;

bool kolMesgul = false;
unsigned long sonAlgilamaZamani = 0;
const unsigned long beklemeSuresi = 5000;

void logYaz(const char* mesaj) {
  client.publish("metanoia/sistem_log", mesaj);
}

void bantCalistir() {
  digitalWrite(in3Pin, HIGH);
  digitalWrite(in4Pin, LOW);
}

void bantDurdur() {
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
}

int mesafeOku() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long sure = pulseIn(echoPin, HIGH, 30000);

  if (sure == 0) return 999;

  int mesafe = sure * 0.034 / 2;
  return mesafe;
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mesaj = "";
  for (int i = 0; i < length; i++) mesaj += (char)payload[i];
  mesaj.trim();

  if (String(topic) == "metanoia/durum" && mesaj == "BITTI") {
    kolMesgul = false;
    sonAlgilamaZamani = millis();

    bantCalistir();
    logYaz("Robot kol isini bitirdi. Bant calisiyor.");
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "MetanoiaLider-";
    clientId += String(random(1000, 9999));

    if (client.connect(clientId.c_str())) {
      client.subscribe("metanoia/durum");
      logYaz("Lider ESP32 baglandi. Bant calisiyor.");
    } else {
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  bantCalistir();
}

void loop() {
  if (!client.connected()) reconnect();

  client.loop();

  if (!kolMesgul && millis() - sonAlgilamaZamani > beklemeSuresi) {
    int mesafe = mesafeOku();

    Serial.print("Mesafe: ");
    Serial.println(mesafe);

    if (mesafe > 0 && mesafe <= ESIK_MESAFE) {
      bantDurdur();

      kolMesgul = true;
      sonAlgilamaZamani = millis();

      client.publish("metanoia/komut", "AL");
      logYaz("Nesne algilandi, bant durdu. Kol cagirildi.");
    }
  }

  delay(100);
}

