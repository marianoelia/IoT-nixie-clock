/*
 * Project Particle Photon Nixie Clock
 * Description:
 * Author:
 * Date: July 20 2017
 */


 int zoneAddress = 0;
 int blueLEDAddress = 1;
 int dstAddress = 2;
 int zeroOrNotAddress = 3;


 int multiplexDelay = 1000;

 int limit = 100;
 //Grounds
 int digit1 = 5;
 int digit2 = 4;
 int digit3 = 3;
 int digit4 = 2;
 int decimalPin = 6;
 int DHTPin = 7;
 int blueLEDPin = 1;
 //int blueLED = 1; //blue LED pin
 int dateInterval; // = 30; // interval which date is shown

 int zone;
 int ledOnOff;
 int dst;
 int zeroOrNot = 1;
 long previousMillis = 0;

 int count = 0;
 int oldTime;
 int segmentPins[4]={
     A1, A3, A2, A0 //
 };

 byte numbers[10][4] = {
  {0,0,0,1}, // 0
  {0,0,0,0}, // 1
  {1,0,0,1}, // 2
  {1,0,0,0}, // 3
  {0,1,1,1}, // 4
  {0,1,1,0},// 5
  {0,1,0,1},// 6
  {0,1,0,0}, // 7
  {0,0,1,1},// 8
  {0,0,1,0}// 9
     };

 void setup() {
//Cloud particle funtions
     Particle.function("SetOffset", setZone);
     Particle.function("Scramble",scramble);
     Particle.function("Date", showDate);
     Particle.function("BlueLEDs", blueLEDSwitch);
     Particle.function("isDST", dstSettings);
     Particle.function("Leading Zero", setZeroOrNot);
     Particle.function("Start Effect", startEffect);
     Particle.function("Change delay", changeDelay);

 pinMode(A0, OUTPUT);
 pinMode(A1, OUTPUT);
 pinMode(A2, OUTPUT);
 pinMode(A3, OUTPUT);

 pinMode(blueLEDPin, OUTPUT); //Pin to turn blue leds on/off
 pinMode(DHTPin, INPUT); //Pin input from DHT11

 pinMode(digit1, OUTPUT);
 pinMode(digit2, OUTPUT);
 pinMode(digit3, OUTPUT);
 //EEPROM.get(zoneAddress, zone);
 pinMode(digit4, OUTPUT);
 pinMode(decimalPin, OUTPUT);


 //digitalWrite(7, LOW);
 //Serial.begin(9600);

 //timer.start();
 //get last settings
 getTimeZone();
 getLEDSettings();
 getDSTSettings();
 getZeroOrNot();
 startEffect("on");
 oldTime = getTime();
 digitalWrite(decimalPin, LOW);
 //showTime();
 //Time.month();
 } ////////////// End of void setup()




 void loop(){

   unsigned long currentMillis = millis();

 showTime();
 //Timer timer(10000, print_every_second);


 if (currentMillis - previousMillis  > 60000){ //every 2 minutes
     previousMillis = currentMillis;
     scramble("on");
     showDate("on");
     scramble("on");
    // count = 0;
   //  oldTime = getTime();
 }

 } ////////////// End of void loop()





 void randomEffect(){


 }

 int randomNumber(int min, int high){


 }


 //used in showing digits but not called directly.
 void displayBCD(int n){
   for (int i = 0; i<4; i++){
     digitalWrite(segmentPins[i], numbers[n][i]);
   }
 }

 //showdigits takes a 4 digit number and displays it on the nixie tubes for a specific length of time.
 void showdigits(int number, int delayLength){ //number will be up to 4 long
 /* To show the multiplexing effect, anodes of the Nixie Tubes will be cycled
  * with a 5 millisecond delay in between each digit
 */
 //1.6
 //Lets use 1234 as an example number input
 //entire function takes 4times*delaylength to complete, so the input delay is divided by 8
 //
 delayLength = delayLength/4;///8;
 for (int current = 0; current<delayLength; current++){
 // Serial.print("Time: ");
  //unsigned long currentMillis = millis(); //used to measure the duration of the function


  int thousands = number/1000;

  digitalWrite(digit4,LOW);
   displayBCD(thousands); //divide by 1000 will leave 1 as the number
  if ( thousands == 20){ //0
     digitalWrite(digit1, LOW);
 }
 else{
   digitalWrite(digit1, HIGH);
   }//only
   digitalWrite(digit2, LOW);
   digitalWrite(digit3, LOW);

 delayMicroseconds(multiplexDelay);

 int hundreds = number%1000/100; //remove the front number, leaving 234

   digitalWrite(digit1, LOW);
   displayBCD(hundreds); //234/100 is 2.
   digitalWrite(digit2, HIGH);
   digitalWrite(digit3, LOW);
   digitalWrite(digit4, LOW);

   delayMicroseconds(multiplexDelay);
 int tens = ((number%1000)%100)/10; //display 3

   digitalWrite(digit1, LOW);
   digitalWrite(digit2, LOW);
   displayBCD(tens);
   digitalWrite(digit3, HIGH);
   digitalWrite(digit4, LOW);


 delayMicroseconds(multiplexDelay);

 int ones = ((number%1000)%100)%10;
   digitalWrite(digit1, LOW);
   digitalWrite(digit2, LOW);
   digitalWrite(digit3, LOW);
   displayBCD(ones); //display the last digit, 4
   digitalWrite(digit4, HIGH);

   delayMicroseconds(multiplexDelay);

   digitalWrite(digit4, LOW);
   //Serial.println(currentMillis);

 }
 }



 int scramble(String command){
    // int zone;
     if (command=="on"){
      //   zone = -5;
 for (int i = 0; i<2; i++){
 for (int j = 0; j<=9999; j+=1111){
 showdigits(j, 30); //8
 }
 }
 return 5;
 }
 else if ( command =="off")
 return 5;
 }



 int showTime(){
 int hourPlusMinute = getTime();
 showdigits(hourPlusMinute,8);
 return hourPlusMinute;
 }



 int showDate(String command){
 if (command == "on"){
 int date = Time.month()*100+Time.day();
 showdigits(date, 3000);
     return 1;
 }
 return -1;
 }



 //////////////////////////////////////////////// Time Functions ///////////////////////////
 void testDigits(){

     digitalWrite(digit1, HIGH); //digit 1 correct
     digitalWrite(digit2, HIGH); //digit 2 correct
     digitalWrite(digit3, HIGH); //digit 3 correct
     digitalWrite(digit4, HIGH); //digit 4 correct
     for (int i = 0; i<=9; i++){
     displayBCD(i);
     delay(1000);
     }
     delay(3000);//should be left on 9
 }

 //give function 4 digits to display in a cool scrolling fashion
 void lowestToHighestEffect(int digits){
    //  Time.zone(zone);//get time
    // int hourPlusMinute = (Time.hourFormat12())*100+Time.minute();





 }


 // set dst
 int dstSettings(String command){
     if (command == "1"){
         EEPROM.put(dstAddress, 1);
     }
     else if (command == "0"){
         EEPROM.put(dstAddress, 0);
     }
     return 1;
 }
 void getTimeZone(){
     EEPROM.get(zoneAddress, zone);
     Time.zone(zone);

 }

 int blueLEDSwitch(String command){

     if (command == "on"){
         ledOnOff = 1;
     digitalWrite(blueLEDPin, ledOnOff);
     digitalWrite(decimalPin, HIGH); //neon bulb
     EEPROM.put(blueLEDAddress, ledOnOff);
     return 5;
     }
     else {
         ledOnOff = 0;
     digitalWrite(blueLEDPin, ledOnOff);
     digitalWrite(decimalPin, LOW); //neon bulb
     EEPROM.put(blueLEDAddress, ledOnOff);
     return 5;
     }
     return 2;
 }

 void getLEDSettings(){
     EEPROM.get(blueLEDAddress, ledOnOff);
     if (ledOnOff == 1) {
     digitalWrite(blueLEDPin, HIGH);
     digitalWrite(decimalPin, HIGH);
   }
     else {
     digitalWrite(blueLEDPin, LOW);
     digitalWrite(decimalPin ,LOW);
 }
 }

 void getDSTSettings(){
     EEPROM.get(dstAddress, dst);
     if (dst==1){
      //   Time.setDSTOffset(1);
        // beginDST();
     }
     else if (dst == 0){
       //  Time.setDSTOffset(0);
      //    beginDST();
     }
 }

 /*
  * Functions for displaying time
  *
  *
  */
 //void showDigitForNum
 int getTime(){
    Time.zone(zone);
     int hourPlusMinute = (Time.hourFormat12())*100+Time.minute();
 return hourPlusMinute;
 }


 void getZeroOrNot(){
     EEPROM.get(zeroOrNotAddress, zeroOrNot);

 }

 int setZeroOrNot(String command){
     if (command == "0"){
         zeroOrNot = 0;
     EEPROM.put(zeroOrNotAddress, 0);
     return 0;
     }
     else if (command == "1"){
         zeroOrNot = 1;
     EEPROM.put(zeroOrNotAddress, 1);
     return 1;
     }
    else return -1;
 }


 int setZone(String command){
     /*
        if (command == "Toronto"){
         zone = -5;

         EEPROM.put(zoneAddress, zone);
           return zone;
     }
    else if (command == "Vancouver"){
     zone = -8;
      EEPROM.put(zoneAddress, zone);
     return zone;
     }
     else if (command == "reset"){
     zone = 0;
      EEPROM.put(zoneAddress, zone);
     return zone;
     }
     */
     //if (command.toInt() ==
     zone = command.toInt();
     EEPROM.put(zoneAddress, zone);
 return 1;
 }


 /*
 * Start effect starts at 00:00 and cycles the minutes digit a couple times before
 * landing on the correct digit. It then proceeds to the tens digit and cycles it.
 */
 int startEffect(String command){
    // delay(300);
  //  int currentTime = getTime();
  if (command == "on"){

  int fullTime = getTime();
    int ones = ((fullTime%1000)%100)%10;
    int tens = ((fullTime%1000)%100)/10;
    int hundreds = fullTime%1000/100;
   // int thousands = fullTime/1000;
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
  }
   return 2;
 }

 int changeDelay(String Command){
     /*
     int length = Command.length();
     Command = Command.replace(1, "");
     Command = Command.replace(length, "");
     */
     multiplexDelay = Command.toInt();

     return multiplexDelay;
 }
