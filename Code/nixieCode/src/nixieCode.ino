/*
 * Project Particle Photon Nixie Clock
 * Description: Software to control 4 IN-14 Nixie tubes using my designed PCB's.
 * Link to repository: https://github.com/mrelia100/nixie-clock
 * Author: Mariano Elia
 * Date Published: June 5, 2018
 */

#include <Adafruit_DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT11

int temperature, humidity;

DHT dht(DHTPIN, DHTTYPE);
//EEPROM Addresses
 int zoneAddress = 0;
 int lastLEDSetting = 1;

//Variable with
 int multiplexDelay = 1000;

//External Pins
 int digit1 = 5;
 int digit2 = 4;
 int digit3 = 3;
 int digit4 = 2;
 int colonPin = 6;
 int blueLEDPin = 1;

 int zone;
 int ledOnOff;
 long previousMillis = 0;

 int count = 0;
 int oldTime;
 const int segmentPins[4]={
     A1, A3, A2, A0
 };

 byte numbers[10][4] = {
  {0,0,0,1}, // 0
  {0,0,0,0}, // 1
  {1,0,0,1}, // 2
  {1,0,0,0}, // 3
  {0,1,1,1}, // 4
  {0,1,1,0}, // 5
  {0,1,0,1}, // 6
  {0,1,0,0}, // 7
  {0,0,1,1}, // 8
  {0,0,1,0}  // 9
  };

 void setup() {
    //Cloud particle funtions
     Particle.function("SetOffset", setZone);
     Particle.function("Scramble",scramble);
     Particle.function("Date", showDate);
     Particle.function("TurnLEDOn", blueLEDSwitch);
     Particle.function("Start Effect", startEffect);
     Particle.function("Get Temp", readTemperature);
     Particle.function("Get Humidity", readHumidity);
 pinMode(A0, OUTPUT);
 pinMode(A1, OUTPUT);
 pinMode(A2, OUTPUT);
 pinMode(A3, OUTPUT);

 pinMode(blueLEDPin, OUTPUT); //Pin to turn blue leds on/off
 pinMode(DHTPIN, INPUT); //Pin input from DHT11

 pinMode(digit1, OUTPUT);
 pinMode(digit2, OUTPUT);
 pinMode(digit3, OUTPUT);
 pinMode(digit4, OUTPUT);
 pinMode(colonPin, OUTPUT);

 getTimeZone();
 getLEDSettings();
 startEffect("1");
 oldTime = getTime();
 digitalWrite(colonPin, LOW);
 } ////////////// End of void setup()


 void loop(){
   unsigned long currentMillis = millis();
   showTime();
 if (currentMillis - previousMillis  > 60000){ //every 2 minutes
     previousMillis = currentMillis;
     scramble("1");
     showDate("1");
     scramble("1");
 }
 } ////////////// End of void loop()

 //used in showing digits but not called directly.
 void writeBCD(int n){
   for (int i = 0; i<4; i++){
     digitalWrite(segmentPins[i], numbers[n][i]);
   }
 }

 //Function showdigits takes a 4 digit number and displays it on the nixie tubes for a specific length of time.
 void showdigits(int number, int delayLength){ //number will be up to 4 long
 /* To show the multiplexing effect, anodes of the Nixie Tubes will be cycled
 */
 delayLength = delayLength/4;
 for (int current = 0; current<delayLength; current++){
  int thousands = number/1000;
  digitalWrite(digit4,LOW);
  writeBCD(thousands);
  if ( thousands == 20){ //0
     digitalWrite(digit1, LOW);
 }
 else{
   digitalWrite(digit1, HIGH);
   }
   digitalWrite(digit2, LOW);
   digitalWrite(digit3, LOW);
   delayMicroseconds(multiplexDelay);

 int hundreds = number%1000/100;
   digitalWrite(digit1, LOW);
   writeBCD(hundreds); //234/100 is 2.
   digitalWrite(digit2, HIGH);
   digitalWrite(digit3, LOW);
   digitalWrite(digit4, LOW);
   delayMicroseconds(multiplexDelay);

 int tens = ((number%1000)%100)/10;
   digitalWrite(digit1, LOW);
   digitalWrite(digit2, LOW);
   writeBCD(tens);
   digitalWrite(digit3, HIGH);
   digitalWrite(digit4, LOW);
   delayMicroseconds(multiplexDelay);

 int ones = ((number%1000)%100)%10;
   digitalWrite(digit1, LOW);
   digitalWrite(digit2, LOW);
   digitalWrite(digit3, LOW);
   writeBCD(ones);
   digitalWrite(digit4, HIGH);
   delayMicroseconds(multiplexDelay);
   digitalWrite(digit4, LOW);
 }
 }

 int scramble(String command){
 for (int i = 0; i<2; i++){
 for (int j = 0; j<=9999; j+=1111){
 showdigits(j, 30); //8
 }
 }
 return 1;
 }


 int showTime(){
 int hourPlusMinute = getTime();
 showdigits(hourPlusMinute,8);
 return hourPlusMinute;
 }


 int showDate(String command){
 int date = Time.month()*100+Time.day();
 showdigits(date, 3000);
     return 1;
 }


 //////////////////////////////////////////////// Time Functions ///////////////////////////

 void getTimeZone(){
     EEPROM.get(zoneAddress, zone);
     Time.zone(zone);
 }

 int blueLEDSwitch(String command){
     if (command == "1"){
      ledOnOff = 1;
     digitalWrite(blueLEDPin, ledOnOff);
     digitalWrite(colonPin, HIGH); //neon bulb
     EEPROM.put(lastLEDSetting, ledOnOff);
     return 1;
     }
     else {
         ledOnOff = 0;
     digitalWrite(blueLEDPin, ledOnOff);
     digitalWrite(colonPin, LOW); //neon bulb
     EEPROM.put(lastLEDSetting, ledOnOff);
     return 1;
     }
     return 0;
 }

 void getLEDSettings(){
     EEPROM.get(lastLEDSetting, ledOnOff);
     if (ledOnOff == 1) {
     digitalWrite(blueLEDPin, HIGH);
   }
     else {
     digitalWrite(blueLEDPin, LOW);
 }
 }


 int getTime(){
    Time.zone(zone);
     int hourPlusMinute = (Time.hourFormat12())*100+Time.minute();
 return hourPlusMinute;
 }

 int setZone(String command){
     zone = command.toInt();
     EEPROM.put(zoneAddress, zone);
 return 1;
 }

 /*
 * Start effect starts at 00:00 and cycles the minutes digit a couple times before
 * landing on the correct digit. It then proceeds to the tens digit and cycles it.
 */
 int startEffect(String command){
    int fullTime = getTime(); //Receive function in full form, eg 1230
    int ones = ((fullTime%1000)%100)%10;
    int tens = ((fullTime%1000)%100)/10;
    int hundreds = fullTime%1000/100;
     showdigits(0000,1000);

     for (int i = 0; i<4; i++){
         if (i ==0){
     for (int j = 0; j<9; j++){
         String string = "000";
         String numToString = String(j);
         string.concat(numToString);
         int number = string.toInt();
         showdigits(number, 30);
     }
         }

     if (i == 1){
         for (int j = 0; j<9; j++){
         String string = "00";
         String numToString = String(j);
         string.concat(numToString);
         string.concat(ones);
         int number = string.toInt();
         showdigits(number, 30);
     }
     }

         if (i == 2){
         for (int j = 0; j<9; j++){
         String string = "0";
         String numToString = String(j);
         string.concat(numToString);
         string.concat(tens);
         string.concat(ones);
         int number = string.toInt();
         showdigits(number, 30);
     }
     }
         if (i == 3){
         for (int j = 0; j<9; j++){
         String string = "";
         String numToString = String(j);
         string.concat(numToString);
         string.concat(hundreds);
         string.concat(tens);
         string.concat(ones);
         int final = string.toInt();
         showdigits(final, 30);
     }
     }

  }
    showdigits(fullTime, 4000);
   return 1;
 }

int readTemperature(String command){
temperature = dht.getTempCelcius();
delay(10);
return temperature;
 }

 int readHumidity(String command){
humidity = dht.getHumidity();
delay(10);
return humidity;
 }
