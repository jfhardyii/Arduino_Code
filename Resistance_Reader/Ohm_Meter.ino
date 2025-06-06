#include <LiquidCrystal.h>
#include <IRremote.h>
#include <SPI.h>
#include <SD.h>
#define NUM_SHUNT_RES 8
#define SELECT_PINS 3
#define IR_RECEIVER_PIN 7 
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
const int LED1 = A13, LED2 = A12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//The code below initializes the correct pin for the SD card code
const int chipSelect = 53;
File dataFile;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The code below defines the inputs used in the system.
int analogPin = 15;
const byte SelPins[SELECT_PINS] = {5,4,3}; //Sets up the selector pins
const byte enable = 6; // 1 is no connection, 0 is resistor value.
const byte LED_Power = 8; //This turns the LED ON
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The code below sets the values of the resistors and other variables used
float rRef[NUM_SHUNT_RES] = {998, 5077, 10040, 50010, 100692, 498630, 1004770, 5020000}; //This assigns rRef to one of the reference resistors.
int raw = 0;
float base=0;
int Vin = 5.0427;
int i = 0;
float Vout = 0;
float New_Vin=0;
float RL = 0;
float disp = 0;
float buffer = 0;
int Res = 0;
int time=0;
int first_time=1;
int power = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This sets up the IR remote
const int POWER = 69;
const int PLAY = 64;
const int UP = 9;
const int DOWN = 7;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
 lcd.begin(16, 2);  
 Serial.begin(9600);
 IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);  


//This is for the SD Code
while (!Serial){
  ;
}
pinMode(LED1, OUTPUT);
digitalWrite(LED1, LOW);//GREEN

pinMode(LED2, OUTPUT);
digitalWrite(LED2, LOW);//RED
 //This enable the pinModes.
 pinMode(enable, OUTPUT);
 digitalWrite(enable, HIGH);
//This sets up the pinMode for the selector pins.
for (int i = 0; i<NUM_SHUNT_RES; i++)
{
  pinMode(SelPins[i], OUTPUT);
  digitalWrite(SelPins[i], HIGH);
}

pinMode(LED_Power, OUTPUT); // Set the LED pin as an output
digitalWrite(LED_Power, HIGH); // Set the initial state of the pin to LOW

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is SD stuff
Serial.print("\nInitializing SD card...\n");
lcd.setCursor(0,0);
lcd.print("Initializing");
lcd.setCursor(0,1);
lcd.print("SD card...");
delay(3000);                                 
lcd.clear();

pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present\n");
    lcd.setCursor(0,0);
    lcd.println("Card failed,");
    lcd.setCursor(0,1);
    lcd.println("or not present");
    delay(3000);
    lcd.clear();
    digitalWrite(LED2, HIGH);//RED
    delay(1000);
    digitalWrite(LED2, LOW);//RED
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.\n");
  lcd.setCursor(0,0);
  lcd.println("SD card");
  lcd.setCursor(0,1);
  lcd.println("initialized.");
  delay(3000);
  lcd.clear();


  // Create a new file
  char filename[] = "data.txt";
  if (!SD.exists(filename)) {
    // Create the file if it doesn't exist
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      Serial.println("File created\n");
      lcd.setCursor(0,0);
      lcd.println("File created");
      delay(3000);
      lcd.clear();

      dataFile.close();
    } else {
      Serial.println("Error creating file\n");
      lcd.setCursor(0,0);
      lcd.println("Error creating");
      lcd.setCursor(0,1);
      lcd.println("file");
      delay(3000);
      lcd.clear();

    }
  } else {
    // If the file exists, open it for appending
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      Serial.println("File opened\n");
      lcd.println("File opened");
      delay(3000);
      lcd.clear();


    } else {
      Serial.println("Error opening file\n");
      lcd.setCursor(0,0);
      lcd.println("Error opening file");
      lcd.setCursor(0,1);
      lcd.println("file");
      delay(3000);
      lcd.clear();

    }
  }
   if (dataFile) {
    dataFile.println("Time:   Resistance in ohms:\n");
    dataFile.flush(); // Flush the data to ensure it's written to the card
    Serial.println("Data written to file");
    digitalWrite(LED1, HIGH);//GREEN
    delay(1000);
    digitalWrite(LED1, LOW);//GREEN

  } else {
    Serial.println("Error writing to file");
    digitalWrite(LED2, HIGH);//RED
    delay(1000);
    digitalWrite(LED2, LOW);//RED
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  int x = 1;
// while (x = 1) {
//   if (IrReceiver.decode()) {
//     uint16_t command = IrReceiver.decodedIRData.command;
//     switch (command) { 
//       case 69: 
//         Serial.println("Power"); 
//         break;
//       case 64:
//         Serial.println("Pause");
//         break;
//       case 9: 
//         Serial.println("Up"); 
//         break;
//       case 7:
//         Serial.println("Down");
//         break;	
//       default:	
//         Serial.println(command);	  
//     } 	   
		
//     delay(100);
//     IrReceiver.resume();
//   }
// }
  //This loop lasts for 10 minutes so that live data is displayed on the LCD but only 10 minutes are recorded on the SD.
  for (int i = 0; i<600; i++)
  {
  lcd.clear();
  //This reads the iniatial Resistance for the code to choose which shunt resistor to use.
  for (int i = 0; i<20; i++)//This for loop is used to verify that the proper value is used.
{
  raw = analogRead(analogPin);
    digitalWrite(SelPins[0], HIGH);
    digitalWrite(SelPins[1], LOW);
    digitalWrite(SelPins[2], LOW);
    digitalWrite(enable, LOW);
    buffer = raw * Vin;
    Vout = (buffer)/1024.0;
    buffer = (Vin/Vout) - 1;
    base= rRef[4] * buffer;
    New_Vin= 5-Vout;
}
//This is the 1st of 8 ranges: 1 kohm
if (base < 2500)
{
    digitalWrite(SelPins[0], LOW);
    digitalWrite(SelPins[1], LOW);
    digitalWrite(SelPins[2], LOW);
    Res = 1;
//This is the 2nd of 8 ranges: 5 kohm
}else if (base>=2500 && base<7500)
{
    digitalWrite(SelPins[0], LOW);
    digitalWrite(SelPins[1], LOW);
    digitalWrite(SelPins[2], HIGH);
    Res = 2;
}
//This is the 3rd of 8 ranges: 10 kohm
else if (base>=7500 && base<25000)
{
    digitalWrite(SelPins[0], LOW);
    digitalWrite(SelPins[1], HIGH);
    digitalWrite(SelPins[2], LOW);
    Res = 3;
}
//This is the 4th of 8 ranges: 50 kohm
else if (base>=25000 && base<75000)
{
    digitalWrite(SelPins[0], LOW);
    digitalWrite(SelPins[1], HIGH);
    digitalWrite(SelPins[2], HIGH);
    Res = 4;
}
//This is the 5th of 8 ranges: 100 kohm
else if (base>=75000 && base<250000)
{
    digitalWrite(SelPins[0], HIGH);
    digitalWrite(SelPins[1], LOW);
    digitalWrite(SelPins[2], LOW);
    Res = 5;
}
//This is the 6th of 8 ranges: 1kohm
else if (base>=250000 && base<750000)
{
    digitalWrite(SelPins[0], HIGH);
    digitalWrite(SelPins[1], LOW);
    digitalWrite(SelPins[2], HIGH);
    Res = 6;
}
//This is the 7th of 8 ranges
else if (base>=750000 && base<2500000)
{
    digitalWrite(SelPins[0], HIGH);
    digitalWrite(SelPins[1], HIGH);
    digitalWrite(SelPins[2], LOW);
    Res = 7;
}
//This is the 8th of 8 ranges
else if (base>=2500000)
{
    digitalWrite(SelPins[0], HIGH);
    digitalWrite(SelPins[1], HIGH);
    digitalWrite(SelPins[2], HIGH);
    Res = 8;
}
for (int i = 0; i<20; i++)
{
    raw = analogRead(analogPin);
    buffer = raw * Vin;
    Vout = (buffer)/1024.0;
    buffer = (Vin/Vout) - 1;
    RL= (rRef[Res-1] + 201) * buffer;
    New_Vin= 5-Vout;
}

    // This code below prints out the values of the Resistance
    Serial.print("Vout: ");
    Serial.println(Vout);


    Serial.print("Rl: ");
    Serial.println(RL);


    //This line will print out the values in the serial monitor and lcd. These values will be saved into the sd file.
    //The code blow chooses which code prints to the lcd
    if (Vout==0){//If Vout is 0 then there is no connection.
    lcd.setCursor(0, 1);
    lcd.print("No Connection");
    delay(1000);}
   if ((RL < 500)&&(Vout!=0)) {
    lcd.setCursor(0, 1);
    lcd.print("RL: ");
    lcd.print(RL);
    lcd.print(" ohms");
    delay(1000);
    digitalWrite(LED1, LOW);//GREEN
    digitalWrite(LED2, LOW);//RED
    }
    else if ((RL>=500000)&&(Vout!=0)){
    lcd.setCursor(0, 1);
    lcd.print("RL: ");
    lcd.print(RL/1000000);
    lcd.print(" Mohms");
    delay(1000);
    digitalWrite(LED1, LOW);//GREEN
    digitalWrite(LED2, LOW);//RED
    }
  else if (Vout!=0){
    lcd.setCursor(0, 1);
    lcd.print("RL: ");
    lcd.print(RL/1000);
    lcd.print(" kohms");
    delay(1000);
    digitalWrite(LED1, LOW);//GREEN
    digitalWrite(LED2, LOW);//RED
    }
    if (first_time==1)
    {
    if (dataFile) {
    dataFile.print(time);
    dataFile.print(" ");
    dataFile.println(RL);
    lcd.setCursor(15, 0);
    digitalWrite(LED1, HIGH);//GREEN
    dataFile.flush(); // Flush the data to ensure it's written to the card
    Serial.println("Data written to file");
  } else {
    Serial.println("Error writing to file");
    digitalWrite(LED2, HIGH);//RED
  }
  time=time+10;
  first_time=first_time+1;
    }
  }
    if (dataFile) {
    dataFile.print(time);
    dataFile.print(" ");
    dataFile.println(RL);
    lcd.setCursor(15, 0);
    digitalWrite(LED1, HIGH);//GREEN
    dataFile.flush(); // Flush the data to ensure it's written to the card
    Serial.println("Data written to file");
  } else {
    Serial.println("Error writing to file");
    digitalWrite(LED2, HIGH);//RED
  }
  time=time+10;
}

