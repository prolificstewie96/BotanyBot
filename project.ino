/*Terrell Stewart
 * Botany Bot Project
 * 12-07-2019
 * The purpose of this project is to create a self regulating plant system that can be regulated
 * through soil moisture and humidity through sensor readings. Profiles can be created and saved
 * to an SD card which will store your desired plant's enviormental likings.
 */


#include<Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SPI.h>
#include <SD.h>

#define DHTPIN 2  
#define DHTTYPE DHT11 
#define HUMIDIFIER 7
#define WATER 8
#define BUTTON 3                                                 

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
DHT dht(DHTPIN, DHTTYPE);
void makeProfile(float *humidity, float *moisture);
void getProfile();
void delete_profile();
void runProfile(float *humidity, float *temperature);
int chipSelect = 4;
float humidifierwater;
File myFile;
void setup() {
  float humidity, moisture, temperature;
  int input;
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16,2);
  Serial.println("Botany Bot");
  lcd.clear();
  lcd.print(F("   BOTANY BOT"));
  pinMode(chipSelect, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(WATER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  
  
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Could not initialize SD card."));
  }
  else {
    Serial.println(F("SD Initialization complete.\n"));
  }

  /* Main Menu*/
  Serial.read();
  do{
    Serial.print('\n');
    delay(2000);
    lcd.clear();
    lcd.print(F("Main Menu"));
    Serial.println(F("Make profile....1")); Serial.println(F("Get profile...2")); 
    Serial.println(F("Run profile...3")); Serial.println(F("Delete profile...4")); Serial.println(F("Quit Program...99"));
    while(Serial.available() == 0);
    input = Serial.parseInt();
    switch(input){
      case 1:
       makeProfile(&humidity, &moisture);
        break;
     case 2:
       getProfile();
       break;
      case 3:
        runProfile(&humidity, &temperature);
        break;
      case 4:
        delete_profile();
        break;
      default:
       Serial.println(F("NOPE!"));
   }
  }while(input != 99);
}



void loop() {
  // put your main code here, to run repeatedly:


}




        /*Custom plant profile maker*/
void makeProfile(float *humidity, float *moisture) {
  Serial.read();
  lcd.clear();
  lcd.print("Make Profile");
  String newName, fileType = ".txt", humid, moist;
  int nameIndex,humidI,moistI;
  Serial.print(F("Enter desired profile name: "));
  while (Serial.available() == 0);
  newName = Serial.readString();
  Serial.println(newName);
  nameIndex = newName.indexOf('\0'); //Getting NULL character
  newName = newName.substring(0, nameIndex - 1); //Removing NULL character to combine strings
  newName = newName + fileType; //Combining strings
  

  if (SD.exists(newName)) {
    Serial.println(newName + "Already exists");
  }
  else {
    lcd.clear();
    Serial.println(F("Creating your new profile..."));
    lcd.clear();
    lcd.print(F("Creating..."));
    delay(1500);
    lcd.clear();
  }
  myFile = SD.open(newName,FILE_WRITE);
  Serial.println(F("Profile Created!"));
  lcd.print(F("Profile Created!"));
  Serial.println(F("Enter your plant's relative humidity: "));
  while(Serial.available() == 0);
  *humidity = Serial.parseFloat();
  Serial.print(*humidity); Serial.println(" %");
  Serial.parseFloat();
  humid = String(*humidity);
  humidI = humid.indexOf('\0');
  char humidarr[humidI];
  humid.toCharArray(humidarr,humidI);

  Serial.parseFloat();
  
  Serial.println(F("Enter your plant's desired soil moisture percentage: "));
  while(Serial.available() == 0);
  *moisture = Serial.parseFloat();
  Serial.print(*moisture); Serial.println(" %");
  moist = String(*moisture);
  moistI = humid.indexOf('\0');
  char moistarr[moistI];
  moist.toCharArray(moistarr,moistI);
 
  myFile.write(humidarr);
  myFile.write(" ");
  myFile.write(moistarr);
  myFile.close();
  Serial.println(F("Profile complete!"));
  lcd.clear();
  lcd.print("Profile Complete");
  Serial.parseFloat(); Serial.print(F("\n"));
}


void getProfile(){

  String profile, fileType = ".txt", output, humidity;
  int nameIndex;
  lcd.clear();
  lcd.print("Get Profile");
  Serial.println(F("Enter desired profile name"));
 Serial.readString();
 /* Get the name of the desired profile you want to delete */
  while (Serial.available() == 0);
  profile = Serial.readString();
  nameIndex = profile.indexOf('\0'); //Getting NULL character
  profile = profile.substring(0, nameIndex - 1); //Removing NULL character to combine strings
  profile = profile + fileType; //Combining strings
  
  /* If it exists then get profile */
  if(SD.exists(profile)){
    Serial.println(F("Fetching profile..."));
    lcd.clear();
    lcd.print("Profile found");
    delay(200);
  }
  /* If not then continue on */
  else{
    Serial.println(F("File does not exist."));
  }
  myFile = SD.open(profile, FILE_READ);
  while(myFile.available()){
   Serial.write(myFile.read()); 
  }
  Serial.print('\n');
  myFile.close();
}

void runProfile(float *humidity, float *temperature){
  float humidSNTL, moistSNTL;
  int moisture = analogRead(0); 
  int button;
  moisture = map(moisture, 0, 1024, 0, 100);
  Serial.parseFloat();
  lcd.clear();
  lcd.print("Run Profile");

  /* Get humidity Sentinal */
  Serial.print(F("\nEnter your desired humidity: "));
  while(Serial.available() == 0);
  humidSNTL = Serial.parseFloat(); Serial.parseFloat();
  Serial.println(humidSNTL); 
  
  /* Get moisture Sentinal */
  Serial.print(F("\nEnter your desired soil moisture: "));
  while(Serial.available() == 0);
  moistSNTL = Serial.parseFloat(); 
  Serial.println(moistSNTL);
  *humidity = dht.readHumidity();
  *temperature = dht.readTemperature(true);
  Serial.println(F("input 99 for humidity and to quit the program"));
  Serial.print(F("Humidity: ")); Serial.print(*humidity); Serial.print(F("% Temperature: ")); Serial.println(*temperature);

  /* Run the program to constantly water and humidifiy the plant */
  do{
    
    /*Water the plant*/ 
   *humidity = dht.readHumidity();
   *temperature = dht.readTemperature(true);
   moisture = analogRead(0);
   moisture = map(moisture, 0, 1024, 0, 100);
    delay(2000);
    button = digitalRead(BUTTON);
    if(button != 0){ // If button is not being pressed
      do{
        if(moistSNTL > moisture){
          lcd.clear();
          lcd.print(F("Watering"));
          lcd.setCursor(0,1); lcd.print(F("Moisture ")); lcd.print(moisture); lcd.print(F("%")); 
          Serial.println("watering");
          digitalWrite(WATER, HIGH);
          delay(2000);
          digitalWrite(WATER,LOW);
          lcd.clear();
          lcd.print(F("Hold button to ")); lcd.setCursor(0,1); lcd.print(F("Quit"));
          delay(3000);
          moisture = analogRead(0);
          moisture = map(moisture, 0, 1024, 0, 100);
          Serial.println(moisture);
        }
        button = digitalRead(BUTTON);
        if(moisture >= moistSNTL){
          button = 0;
        } 
      }while((button != 0));
    }
    button = digitalRead(BUTTON);
    
   /* Humidify Air */
   if(button != 0){
    do{
      *humidity = dht.readHumidity();
      if((humidSNTL > *humidity)){
        delay(100);
        lcd.clear();
        lcd.print(F("Humidity ")); lcd.print(*humidity); lcd.print(F("%"));      
        delay(100); 
        digitalWrite(HUMIDIFIER, HIGH);
        *humidity = dht.readHumidity();
        Serial.println(*humidity);
        button = digitalRead(BUTTON);
      }
      if(*humidity >= humidSNTL){
        button = 0;      
        digitalWrite(HUMIDIFIER, LOW);
      }
    }while(button != 0);
    lcd.clear();
    lcd.print("Standing Bye");delay(2000);lcd.clear();
    digitalWrite(HUMIDIFIER, LOW);
  }
    
    
    button = digitalRead(BUTTON); lcd.print("Loop complete");
    }while(button != 0);
  }


void delete_profile(){
  lcd.clear();
  lcd.print(F("Delete Profile"));
  String profile, fileType = ".txt", output, humidity;
  int nameIndex;
  Serial.println(F("Enter desired profile name to delete"));
  Serial.readString();
  while (Serial.available() == 0);
  profile = Serial.readString();
  nameIndex = profile.indexOf('\0'); //Getting NULL character
  profile = profile.substring(0, nameIndex - 1); //Removing NULL character to combine strings
  profile = profile + fileType; //Combining strings
  
  /* If profile exists then delete it */
  if(SD.exists(profile)){
    lcd.clear();
    Serial.println(F("Deleting profile..."));
    lcd.print(F("Deleting..."));
    delay(1000);
    lcd.clear();
    SD.remove(profile);
    Serial.println(F("Profile deleted"));
    lcd.print(F("Profile Deleted"));
  }
  /* If not then continue on */
  else{
    Serial.println(F("File does not exist."));
  }
}
