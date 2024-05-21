#include <OneWire.h>
#include <DallasTemperature.h>

/* Define for PIR sensor */
int input_PIR = 5; // PIR sensor D1
int state_PIR = LOW;
int val_PIR = 0;

/* Define for DS18B20 sensor */
// Data wire is connected to digital pin D2
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
int tempMax = 29;
float temp_1;
int tempCurrent = 0;

/* Define for relay */
#define RelayPin1 0  //D3 => Quạt 1
#define RelayPin2 2  //D4 => Quạt 2
#define RelayPin3 14 //D5 => Quạt 3
#define RelayPin4 12 //D6 => Quạt 4
#define RelayPin5 13 //D7 => Đèn 
#define RelayPin6 15 //D8 => Phun sương

#define VPIN_BUTTON_0    V0
#define VPIN_BUTTON_1    V1
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3
#define VPIN_BUTTON_4    V5
#define VPIN_BUTTON_5    V6

/* define for Blynk */
#define BLYNK_TEMPLATE_ID "TMPL6-LEsOnO1"
#define BLYNK_TEMPLATE_NAME "lockhongkhi"
#define BLYNK_AUTH_TOKEN "Hdj0oqMd4jKpmzh2uRtsgolZ0O1agE2D"
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
// Your WiFi credentials.
char ssid[] = "Thi Buoi";
char pass[] = "0906531928";
BlynkTimer timer;

int firstDetect = 1;
int firstTimeON = 1;
int firstTimeOFF = 1;

int autoMode = 0; /* 0: manual, 1: auto */

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
}

BLYNK_WRITE(V0)
{
  int relay1State = param.asInt();
  Serial.print("V0 in blynk server is: ");
  Serial.println(relay1State);
  digitalWrite(RelayPin1, relay1State);
}

BLYNK_WRITE(V1)
{
  int relay2State = param.asInt();
  Serial.print("V1 in blynk server is: ");
  Serial.println(relay2State);
  digitalWrite(RelayPin2, relay2State);
}

BLYNK_WRITE(V2)
{
  int relay3State = param.asInt();
  Serial.print("V2 in blynk server is: ");
  Serial.println(relay3State);
  digitalWrite(RelayPin3, relay3State);
}

BLYNK_WRITE(V3)
{
  int relay4State = param.asInt();
  Serial.print("V3 in blynk server is: ");
  Serial.println(relay4State);
  digitalWrite(RelayPin4, relay4State);
}

BLYNK_WRITE(V5)
{
  int relay5State = param.asInt();
  Serial.print("V5 in blynk server is: ");
  Serial.println(relay5State);
  digitalWrite(RelayPin5, relay5State);
}

BLYNK_WRITE(V6)
{
  int relay6State = param.asInt();
  Serial.print("V6 in blynk server is: ");
  Serial.println(relay6State);
  digitalWrite(RelayPin6, relay6State);
}

BLYNK_WRITE(V8)
{
  autoMode = param.asInt();
  Serial.print("V8 in blynk server is: ");
  Serial.println(autoMode);
}

BLYNK_WRITE(V9)
{
  tempMax = param.asInt();
  Serial.print("V9 in blynk server is: ");
  Serial.println(tempMax);
}

void myTimerEvent()
{
  Blynk.virtualWrite(V4, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  sensors.begin(); // init for ds18b20

  pinMode(input_PIR, INPUT);
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);

  // connect to wifi.
  WiFi.begin(ssid, pass);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(RelayPin1, 1);
    digitalWrite(RelayPin2, 1);
    digitalWrite(RelayPin3, 1);
    digitalWrite(RelayPin4, 1);
    digitalWrite(RelayPin5, 1);
    digitalWrite(RelayPin6, 1);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  delay(2000);
  checkDetect();
  tempCurrent = getDataTemp();
  handleWhenDetectedMotion();
  if (autoMode == 1) {
    Serial.println("Mode Auto");
    handleWhenOverTempMax(tempCurrent);
  }
  if (Blynk.connected()) {
    Serial.println("NodeMCU is connected to Blynk app.");
  } else {
    Serial.println("NodeMCU is not connected to Blynk app.");
    digitalWrite(RelayPin1, 1);
    digitalWrite(RelayPin2, 1);
    digitalWrite(RelayPin3, 1);
    digitalWrite(RelayPin4, 1);
    digitalWrite(RelayPin5, 1);
    digitalWrite(RelayPin6, 1);
  }
}

void checkDetect() {
  val_PIR = digitalRead(input_PIR);
  Serial.print("val_PIR:");
  Serial.println(val_PIR);
  if ((val_PIR == 1) && (firstDetect == 1)) {
    firstTimeOFF = 1;
    firstDetect = 0;
    Blynk.virtualWrite(V7, "Yes");
  } if (val_PIR == 0) {
    firstTimeON = 1;
    firstDetect = 1;
    Blynk.virtualWrite(V7, "No");
  }
}

void handleWhenDetectedMotion() {
  if (val_PIR == 1) {
    openFullRelay();
  } else {
    closeFullRelay();
  }
}

void openFullRelay() {
  Serial.println("openFullRelay");
  if ((val_PIR == 1) && (firstTimeON == 1)) {
    digitalWrite(RelayPin1, 0);
    digitalWrite(RelayPin2, 0);
    digitalWrite(RelayPin3, 0);
    digitalWrite(RelayPin4, 0);

    firstTimeON = 0;

    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
  }

}

void closeFullRelay() {
  Serial.println("closeFullRelay");
  if ((val_PIR == 0) && (firstTimeOFF == 1)) {
    digitalWrite(RelayPin1, 1);
    digitalWrite(RelayPin2, 1);
    digitalWrite(RelayPin3, 1);
    digitalWrite(RelayPin4, 1);

    firstTimeOFF = 0;

    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V2, 1);
    Blynk.virtualWrite(V3, 1);
  }

}

void handleWhenOverTempMax(int tempCurrent) {
  if (tempMax < tempCurrent) {
    // openPhunSuong();
    digitalWrite(RelayPin6, 0);
    Blynk.virtualWrite(V6, 0);
    Serial.println("openPhunSuong");
  } else {
    // closePhunSuong();
    digitalWrite(RelayPin6, 1);
    Blynk.virtualWrite(V6, 1);
    Serial.println("closePhunSuong");
  }
}

int getDataTemp() {
  int temp_int = 0;
  sensors.requestTemperatures();

  //print the temperature in Celsius
  temp_1 = sensors.getTempCByIndex(0);
  temp_int = (int) temp_1;
  Blynk.virtualWrite(V4, temp_int);
  Serial.print("Temperature: ");
  Serial.print(temp_int);
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");

  return temp_int;
}
