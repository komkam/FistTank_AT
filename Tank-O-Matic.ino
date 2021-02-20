/*
  ######################################################################
  ##                      Pin OUT MEGA2560 R3                         ##
  ######################################################################
  D0/RX0  =
  D1/TX0  =
  D2      = MANL Manual indicator
  D3      = LPL lamp indicator
  D4      = HTL1 heater1 indicator
  D5      = HTL2 heater2 indicator
  D6      = FPL1 Filter Pump1 indicator
  D7      = FPL2 Filter Pump2 indicator
  D8      = APL Air Pump indicator
  D9      =
  D10     =
  D11     =
  D12     =
  D13     =
  D14/TX3 =
  D15/RX3 =
  D16/TX2 =
  D17/RX2 =
  D18/TX1 = BT/TX
  D19/RX1 = BT/RX
  D20     = SDA
  D21     = SCL
  D22     = CH1
  D23     = CH8
  D24     = CH2
  D25     = CH7
  D26     = CH3
  D27     = CH6
  D28     = CH4
  D29     = CH5
  D30     = colPins1
  D31     = rowPins1
  D32     = colPins2
  D33     = rowPins2
  D34     = colPins3
  D35     = rowPins3
  D36     = colPins4
  D37     = rowPins4
  D38     = MANT Manual Trig
  D39     = LPT Lamp Trig
  D40     = HTT1 Heater1 Trig
  D41     = HTT2 Heater2 Trig
  D42     = FPT1 FilterPump1 Trig
  D43     = FPT2 FilterPump2 Trig
  D44     = APT AirPump Trig
  D45     =
  D46     =
  D47     =
  D48     =
  D49     =
  D50     = DS18B20 Temperature Sensor PIN
  D51     =
  D52     =
  D53     =
  D54/A0  =
  D55/A1  =
  D56/A2  =
  D57/A3  =
  D58/A4  =
  D59/A5  =
  D60/A6  =
  D61/A7  =
  D62/A8  =
  D63/A9  =
  D64/A10 =
  D65/A11 =
  D66/A12 =
  D67/A13 =
  D68/A14 =
  D69/A15 =
*/

#define BAUDRATE 9600
#define DR digitalRead
#define DW digitalWrite

//####################################### LCD 20x4 I2C #########################################################
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //ADDRESS,COLS,ROWS
//##############################################################################################################

//##################################### DS18B20 Temperature Sensor #############################################
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 50
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//##############################################################################################################

//################################### DS3231 Time Module #######################################################
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 RTC;
//##############################################################################################################

//#################################### Bluetooth HC-06 #########################################################
#include <SoftwareSerial.h>
SoftwareSerial BT(19, 18); // RX, TX
//##############################################################################################################

//################################## Matrix Keypad 4x4 #########################################################
//#include <Keypad_I2C.h>
#include <Keypad.h>
//#define I2CADDR 0x21

const byte ROWS = 4; // กำหนดจำนวนของ Rows
const byte COLS = 4; // กำหนดจำนวนของ Columns

// กำหนด Key ที่ใช้งาน (4x4)
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {37, 35, 33, 31}; // เชื่อมต่อกับ Pin แถวของปุ่มกด
byte colPins[COLS] = {36, 34, 32, 30}; // เชื่อมต่อกับ Pin คอลัมน์ของปุ่มกด

/* makeKeymap(keys) : กำหนด Keymap
   rowPins : กำหนด Pin แถวของปุ่มกด
   colPins : กำหนด Pin คอลัมน์ของปุ่มกด
   ROWS : กำหนดจำนวนของ Rows
   COLS : กำหนดจำนวนของ Columns
   I2CADDR : กำหนด Address ขอ i2C
   PCF8574 : กำหนดเบอร์ IC
*/
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//##############################################################################################################

//--------------- Variable ---------------------
String now_date;
String now_time;
String new_date;
String new_time;
String timeon = "1:10:10";  //test string
String timeoff = "2:20:20"; //test string
String temppv;
String tempsv = "00.00"; //test string
int setmode;
float temp;
//----------------------------------------------

//-------------- State DATA --------------------
String LPS;
String HTS1;
String HTS2;
String FPS1;
String FPS2;
String APS;
String MANS;
//----------------------------------------------

//-------------- Output pin --------------------
// Relay Trig
const int CH1 = 22;  //Heater1
const int CH2 = 24;  //Heater2
const int CH3 = 26;  //Pump1
const int CH4 = 28;  //Pump2
const int CH5 = 29;  //AirPump
const int CH6 = 27;  //Light
const int CH7 = 25;  //SPARE
const int CH8 = 23;  //SPARE

//indicator
const int MANL = 2;
const int LPL = 3;
const int HTL1 = 4;
const int HTL2 = 5;
const int FPL1 = 6;
const int FPL2 = 7;
const int APL = 8;
//----------------------------------------------

//-------------- Input pin ---------------------
const int MANT = 38;
const int LPT = 39;
const int HTT1 = 40;
const int HTT2 = 41;
const int FPT1 = 42;
const int FPT2 = 43;
const int APT = 44;

//----------------------------------------------

//##################################### Setup Loop ############################################################
void setup(void) {
  Serial.begin(BAUDRATE);
  BT.begin(BAUDRATE);                       //start bluetooth
  keypad.begin( makeKeymap(keys) );         //Keypad start
  Wire.begin();                             //I2C start
  sensors.begin();                          //tempurature sense
  RTC.begin();                              //start RTC clock
  RTC.adjust(DateTime(__DATE__, __TIME__));

  //set RTC time on first start
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  //lcd start

  lcd.init();
  lcd.backlight();

  // Setup pinMode
  //relay
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  pinMode(CH3, OUTPUT);
  pinMode(CH4, OUTPUT);
  pinMode(CH5, OUTPUT);
  pinMode(CH6, OUTPUT);
  pinMode(CH7, OUTPUT);
  pinMode(CH8, OUTPUT);

  //indicator
  pinMode(MANL, OUTPUT);
  pinMode(LPL, OUTPUT);
  pinMode(HTL1, OUTPUT);
  pinMode(HTL2, OUTPUT);
  pinMode(FPL1, OUTPUT);
  pinMode(FPL2, OUTPUT);
  pinMode(APL, OUTPUT);

  //trigger
  pinMode(MANT, INPUT);
  pinMode(LPT, INPUT);
  pinMode(HTT1, INPUT);
  pinMode(HTT2, INPUT);
  pinMode(FPT1, INPUT);
  pinMode(FPT2, INPUT);
  pinMode(APT, INPUT);
}
//##############################################################################################################

//#################################### Main Loop ###############################################################
void loop() {
  /*  char key = keypad.getKey();
    if (key != NO_KEY) {
      Serial.println(key);
    }*/
  gettime();
  gettemp();
  setup_sec();

  if (setmode == 0) {
    setupA();
    heat_cont();
    lamp_cont();
  }
  else if (setmode == 1) {
    setupB();
  }
  else if (setmode == 2) {
    setupC();
  }
  else if (setmode == 3) {
    setupD();
  }
}
//##############################################################################################################

//################################# Get Time Function ##########################################################
String gettime() {
  DateTime now = RTC.now();
  now_date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  now_time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  if (now_time == "165:165:85") {
    new_time = "ERROR !!!";
  }
  else {
    new_time = now_time;
  }
  if (now_date == "165/165/2165") {
    new_date = "ERROR !!!";
  }
  else {
    new_date = now_date;
  }
  Serial.println();
  Serial.print(new_time);
  Serial.print(" ");
  Serial.print(new_date);
  return new_date, new_time;
}
//##############################################################################################################

//#################################### Triger & Indiacator Check ###############################################
void state () {
  DR(MANT);
  DR(LPT);
  DR(HTT1);
  DR(HTT2);
  DR(FPT1);
  DR(FPT2);
  DR(APT);

  //Manual Mode
  if (MANT == HIGH) {
    MANS = "ON";
    DW(MANL, HIGH);
  }
  else {
    MANS = "OFF";
    DW(MANL, LOW);
  }

  //Lamp
  if (LPT == HIGH || DR(CH6) == HIGH) {
    LPS = "ON";
    DW(LPL, HIGH);
  }
  else {
    LPS = "OFF";
    DW(LPL, LOW);
  }

  //Heater1
  if (HTT1 == HIGH) {
    HTS1 = "ON";
    DW(HTL1, HIGH);
  }
  else {
    HTS1 = "OFF";
    DW(HTL1, LOW);
  }

  //Heater2
  if (HTT2 == HIGH) {
    HTS2 = "ON";
    DW(HTL2, HIGH);
  }
  else {
    HTS2 = "OFF";
    DW(HTL2, LOW);
  }

  //FilterPump1
  if (FPT1 == HIGH) {
    FPS1 = "ON";
    DW(FPL1, HIGH);
  }
  else {
    FPS1 = "OFF";
    DW(FPL1, LOW);
  }

  //FilterPump2
  if (FPT2 == HIGH) {
    FPS2 = "ON";
    DW(FPL2, HIGH);
  }
  else {
    FPS2 = "OFF";
    DW(FPL2, LOW);
  }

  //Airpump
  if (APT == HIGH) {
    APS = "ON";
    DW(APL, HIGH);
  }
  else {
    APS = "OFF";
    DW(APL, LOW);
  }
}
//##############################################################################################################

//################################# Get Tempurature Value ######################################################
void gettemp() {
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  if (temp == -127.00) {
    temppv = "ERROR !!!";
  }
  else {
    temppv = temp;
  }
  Serial.println();
  Serial.print(temppv);
  Serial.print(" ");
  Serial.print("ºC");
}
//##############################################################################################################

//################################ Heater Control ##############################################################
void heat_cont() {
  if (temppv == tempsv) {
    DW(CH2, HIGH);
    DW(CH3, HIGH);
    Serial.println();
    Serial.println("Heater No.1 Auto ON");
    Serial.println("Heater No.2 Auto ON");
  }
  else if (temppv <= tempsv) {
    DW(CH2, LOW);
    DW(CH3, LOW);
    Serial.println();
    Serial.println("Heater No.1 Auto OFF");
    Serial.println("Heater No.2 Auto OFF");
  }
  else if (temppv == "ERROR !!!") {
    DW(CH2, LOW);
    DW(CH3, LOW);
    Serial.println();
    Serial.println("TEMPPURATURE SENSOR ERROR!!!");
    Serial.println("Heater No.1 Force OFF");
    Serial.println("Heater No.2 Force OFF");
  }
}
//##############################################################################################################

//################################# Lamp Control ###############################################################
void lamp_cont() {
  if (new_time == timeon) {
    DW(CH6, HIGH);
    Serial.println();
    Serial.println("Lamp Auto ON");
  }
  else if (new_time == timeoff) {
    DW(CH6, LOW);
    Serial.println();
    Serial.println("Lamp Auto OFF");
  }
  else if (new_time == "ERROR !!!") {
    DW(CH6, LOW);
    Serial.println();
    Serial.println("TIME MODULE ERROR!!!");
    Serial.println("LAMP Force OFF");
  }
}
//##############################################################################################################

//############################### Setup Sequence ###############################################################
void setup_sec() {
  char key = keypad.getKey();
  if (key == 'A') {
    Serial.println();
    Serial.println("Display Main Screen");
    lcd.clear();
    setmode = 0;
    //setupA();
  }
  else if (key == 'B') {
    Serial.println();
    Serial.println("Setup Tempurature");
    lcd.clear();
    //setupB();
    setmode = 1;
  }
  else if (key == 'C') {
    Serial.println();
    Serial.println("Setup Lamp Time");
    lcd.clear();
    //setupC();
    setmode = 2;
  }
  else if (key == 'D') {
    Serial.println();
    Serial.println("MANUAL MODE");
    lcd.clear();
    //setupD();
    setmode = 3;
  }
}
//##############################################################################################################

//################################# Main Screen ################################################################
void setupA() {
  DateTime now = RTC.now();
  lcd.setCursor(1, 0);
  lcd.print("Automatic FishTank");
  lcd.setCursor(1, 1);
  lcd.print("Temp = ");
  lcd.setCursor(8, 1);
  lcd.print("           ");
  lcd.setCursor(8, 1);
  lcd.print(temppv);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(1, 2);
  lcd.print("Time = ");
  lcd.setCursor(8, 2);
  lcd.print("         ");
  lcd.setCursor(8, 2);
  lcd.print(new_time);
  lcd.setCursor(1, 3);
  lcd.print("Date = ");
  lcd.setCursor(8, 3);
  lcd.print("          ");
  lcd.setCursor(8, 3);
  lcd.print(new_date);
}
//##############################################################################################################

//############################### Setup Tempurature Screen #####################################################
void setupB() {
  lcd.setCursor(1, 0);
  lcd.print("Tempurature Config");
  lcd.setCursor(4, 2);
  lcd.print("PV = ");
  lcd.setCursor(9, 2);
  lcd.print("         ");
  lcd.setCursor(9, 2);
  lcd.print(temppv);
  lcd.setCursor(16, 2);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(4, 3);
  lcd.print("SV = ");
  lcd.print(tempsv);
  lcd.setCursor(16, 3);
  lcd.print((char)223);
  lcd.print("C");
  // lcd.setCursor();

}
//##############################################################################################################

//################################# Auto Lamp Time Setup #######################################################
void setupC() {
  lcd.setCursor(1, 0);
  lcd.print("Lamp ON/OFF Config");
  lcd.setCursor(1, 1);
  lcd.print("NOW =");
  lcd.setCursor(8, 1);
  lcd.print("          ");
  lcd.setCursor(8, 1);
  lcd.print(new_time);
  lcd.setCursor(1, 2);
  lcd.print("ON  =");
  lcd.setCursor(8, 2);
  lcd.print(timeon);
  lcd.setCursor(1, 3);
  lcd.print("OFF =");
  lcd.setCursor(8, 3);
  lcd.print(timeoff);
}
//##############################################################################################################

//################################ Manual Mode Screen ##########################################################
void setupD() {
  lcd.setCursor(4, 0);
  lcd.print("MANUAL MODE!");
  lcd.setCursor(1, 1);
  lcd.print("HT 1 = ");
  lcd.print("   ");
  lcd.print(HTS1);
  lcd.setCursor(1, 2);
  lcd.print("HT 2 = ");
  lcd.print("   ");
  lcd.print(HTS2);
  lcd.setCursor(1, 3);
  lcd.print("LP 1 = ");
  lcd.print("   ");
  lcd.print(LPS);
  lcd.setCursor(12, 1);
  lcd.print("FP 1 = ");
  lcd.print("   ");
  lcd.print(FPS1);
  lcd.setCursor(12, 2);
  lcd.print("FP 2 = ");
  lcd.print("   ");
  lcd.print(FPS2);
  lcd.setCursor(12, 3);
  lcd.print("AP = ");
  lcd.print("   ");
  lcd.print(APS);
}
//##############################################################################################################
