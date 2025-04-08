//Libraries
#include <DHT.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>


// Define the servo pin
#define SERVO_PIN 53
// Define DHT 22 pin and setup (temp and humidity sensor)
#define DHTPin 11
#define DHTType 22

// Define commonly used colors for easy use
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define BUZZER_PIN 9

// Define pin for the fan
#define fanPin A9        // Analog pin
const int fanPinD = 12;  // Degital pin

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Setup the DHT Sensor
DHT dht(DHTPin, DHTType);

// Create a servo object
Servo servoMotor;

int doorPosition = 0;  //CLosed
const int statusLED = 51;
const int redLED = 50;
const int greenLED = 52;
const int yellowLED = 23;
const int yellowLED2 = 22;

int motionSensor = 10;  // the pin that the sensor is atteched to
int motionState = LOW;        // by default, no motion detected
int motionValue = 0;            // variable to store the sensor status (motionValueue)

bool stringComplete = false;
String inputString = "";
float temp;
float hum;
float oldTemp;
bool blink = false;
bool lightStatus = false;
bool buzzStatus = false;
bool doorStatus = false;
bool fanStatus = false;

void setup() {
  servoMotor.attach(SERVO_PIN);
  pinMode(fanPinD, OUTPUT);
  pinMode(redLED, OUTPUT);  // initalize LEDs as an output
  pinMode(yellowLED, OUTPUT);
  pinMode(yellowLED2, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(motionSensor, INPUT);  // initialize sensor as an input
  pinMode(BUZZER_PIN, OUTPUT);

  servoMotor.write(180);  //Close door
  doorPosition = 0;
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  dht.begin();
  lcd.print("Temp & Humidity");

}

void loop() {
  // int indicator = read_async();
  // lcd.clear();
  // lcd.print("Temp & Humidity");
  // // while (!Serial.available()){

  // // clearLCDLine(1);
  
  // clearLCDLine(1);
  // oldTemp = temp;
  // takeDHTReadings();
  // checkMotion();
  // blinky();
  // buzzBuzz();
  // // delay(500);
  // sendReadings();

  
  // sendReadings();
  
  
  // if(indicator != 1){
  //   read_async();
  // }
  
  while (stringComplete == false) {
  lcd.clear();
  lcd.print("Temp & Humidity");

  // while (!Serial.available()){
  
  clearLCDLine(1);
  oldTemp = temp;
  takeDHTReadings();
  checkMotion();
  blinky();
  buzzBuzz();

  if(Serial.available() != 1) {
    while (!Serial) yield();
    sendReadings();
    // Serial.println(Serial.available());
    // Serial.println("first if");

    delay(400);
    serialEvent();

  }
  if (Serial.available() > 0) {
    
    delay(500);
    // Serial.println("second if");
    // Serial.println(Serial.available());


    // delay(200);
    serialEvent();
  }

  }
  // } else {
    // sendReadings();
  // }
  // Reset string
  if (inputString.startsWith("status")) {
      sendStatus();
    } else if (inputString.startsWith("set")) {
      if (inputString.indexOf("on") > -1) {
        digitalWrite(statusLED, HIGH);
        Serial.println("Status LED turned on");
      } else if (inputString.indexOf("off") > -1) {
        digitalWrite(statusLED, LOW);
        Serial.println("Status LED turned off");
      } else if (inputString.indexOf("open") > -1) {
        openDoor();
      } else if (inputString.indexOf("close") > -1) {
        closeDoor();
      } else {
        Serial.println("invalid set command");
      }
    } else if (inputString.startsWith("fan")) {
      if (inputString.indexOf("on") > -1) {
        fanOn();
      } else if (inputString.indexOf("off") > -1) {
        fanOff();
      } else {
        Serial.println("invalid fan command");
      }
    } else if (inputString.startsWith("lights")) {
      if (inputString.indexOf("on") > -1) {
        lightStatus = true;
        lightsOn();
      } else if (inputString.indexOf("off") > -1) {
        lightStatus = false;
        lightsOff();
      }
    } else if (inputString.startsWith("alert")) {
      if (inputString.indexOf("on") > -1) {
        redAlertOn();        
        Serial.println("Alert On");
        blink = true;
        buzzStatus = true;
      } else if (inputString.indexOf("off") > -1) {
        redAlertOff();
        blink = false;
        buzzStatus = false;
        Serial.println("Alert Off");

      } else {
        Serial.println("invalid alert command");
      }
    } else if (inputString.startsWith("send")) {
      if (inputString.indexOf("temp") || inputString.indexOf("hum") || inputString.indexOf("read")) {
        sendReadings();
      } else {
        Serial.println("invalid send command");
      }
    }
    stringComplete = false;
    inputString = "";

  // if (Serial.available() > 0) serialEvent();
  // }

  // delay(1000);
}



float takeReading(float temp, float hum) {
  float t = dht.readTemperature();
  delay(100);
  float h = dht.readHumidity();

  return t, h;
}

float takeDHTReadings() {
  // Read temperature in Celsius
  temp = dht.readTemperature();
  // temp = temperature;
  // Read humidity (%)
  hum = dht.readHumidity();
  // hum = humidity;
  // Check if any reads failed and exit early (to try again)
  if (isnan(hum) || isnan(temp)) {  //Check if the readings are motionValueid
    lcd.print("Failed to read DHT");
    digitalWrite(redLED, HIGH);  //Blink Red LED
    delay(1000);
    digitalWrite(redLED, LOW);
    return;
  }

  // Print temperature and humidity to serial monitor
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.print(" °C");
  // Serial.print("    ");
  // Serial.print("Humidity: ");
  // Serial.print(humidity);
  // Serial.println(" %");
  // Print temperature and humidity to LCD
  // String tempPayload = "Temperature: " + (string)temperature + " °C" + " Humidity: " + (string)humidity + " %";
  // Serial.print(tempPayload);
  // Serial.print(str);

  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.print(" C  ");
  lcd.print(hum);
  lcd.print(" % ");

  controlDoor(hum);
  // delay(3000);
}


void checkMotion() {
  motionValue = digitalRead(motionSensor);  // read sensor motionValueue
  if (motionValue == HIGH) {                // check if the sensor is HIGH
    lightsOn();                     // turn LED ON
    // buzzBuzz();
    delay(100);  // delay 100 milliseconds

    if (motionState == LOW) {
      // Serial.println("Motion detected!");
      // openDoor();
      // lcd.clear();
      // lcd.print("MOTION DETECTED!");
      motionState = HIGH;  // update variable motionState to HIGH
    }
  } else {
    delay(200);  // delay 200 milliseconds

    if (motionState == HIGH) {
      // closeDoor();
      // noBuzz();
      // Serial.println("Motion stopped!");
      // lcd.clear();
      // lcd.print("MOTION STOPPED!");
      motionState = LOW;  // update variable motionState to LOW
      if (!lightStatus){
        lightsOff();  // turn LED OFF
      }
    }
  }
}

void controlDoor(float humidity) {
  if (humidity >= 45 & doorPosition == 0) {

    Serial.println("High humidity detected!");
    // lcd.setCursor(0, 1);
    // lcd.print("Opening Door!");
    delay(100);

    // Move the servo to 90 degrees (center position)
    openDoor();
    doorPosition = 1;
    fanOn();
    buzzStatus = true;
    blink = true;

    // Serial.println("Fan turned on!");
    // lcd.print("High humidity!");
    // lcd.setCursor(0, 1);
    // lcd.print("FAN turned on!");
    delay(100);  // Wait for 1 second
  }

  else if (humidity < 45 & doorPosition == 1) {
    Serial.println("Humidity level: Normal");
    // lcd.setCursor(0, 1);
    // lcd.print("Closing Door!");
    // delay(100);
    // Move the servo to 180 degrees (maximum position)
    closeDoor();
    doorPosition = 0;
    fanOff();
    buzzStatus = false;
    blink = false;
    delay(100);  // Wait for 1 second
  }
}

void openDoor() {
  doorStatus = true;
  Serial.println("Door Open");
  servoMotor.write(135);         // turn servo motor 45 degrees clockwise to open the door
  digitalWrite(greenLED, HIGH);  // turn LED ON
  digitalWrite(redLED, LOW);
  // doorPosition = 1;
}

void closeDoor() {
  doorStatus = false;
  Serial.println("Door Closed");
  servoMotor.write(180);        // turn servo motor 45 degrees counter clockwise to close the door
  digitalWrite(greenLED, LOW);  // turn LED OFF
  // doorPosition = 0;
}

void redAlertOn() {
  digitalWrite(redLED, HIGH);
}

void redAlertOff() {
  digitalWrite(redLED, LOW);
}

void fanOn() {
  fanStatus = true;
  digitalWrite(fanPinD, HIGH);
  Serial.println("Fan On");
}

void fanOff() {
  fanStatus = false;
  digitalWrite(fanPinD, LOW);
  Serial.println("Fan Off");
}

void lightsOn() {
  digitalWrite(yellowLED2, HIGH);
  digitalWrite(yellowLED, HIGH);
  Serial.println("Lights On");

}

void lightsOff() {
  lightStatus=false;
  digitalWrite(yellowLED2, LOW);
  digitalWrite(yellowLED, LOW);
  Serial.println("Lights Off");
}

void sendStatus() {
  if (!Serial.available()){
    if (isnan(hum) || isnan(temp)) {  //Check if the readings are motionValueid
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    else{
      Serial.println("All systems are operating!");
    }  
    Serial.println(statusString());
  }
  else return;
}

void sendReadings() {
  if (isnan(hum) || isnan(temp)) {  //Check if the readings are motionValueid
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Print temperature and humidity to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C");
  Serial.print("    ");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
}

void blinky() {
  if (blink) {
    redAlertOn();
    delay(500);
    redAlertOff();
  } else return;
}

void buzzBuzz(){
  if (buzzStatus){
    tone(BUZZER_PIN, 45);          // Produce a tone of 100 Hz on the buzzer pin
    delay(500);
    noTone(BUZZER_PIN);
  }
  else noTone(BUZZER_PIN);
}

String statusString() {
  String statusString;
  statusString += "Door: ";
  if (doorStatus){
    statusString += "Open  ";
  } else statusString += "Closed";
  statusString += " Lights: ";
  if (lightStatus) {
    statusString += "On ";
  } else statusString += "Off";
  statusString += " Fan: ";
  if (fanStatus) {
    statusString += "On ";
  } else statusString += "Off";
  statusString += " Alert: ";
  if (buzzStatus) {
  statusString += "On ";
  } else statusString += "Off";

  // // makeStatusString(statusString);
  
  statusString += " Temperature: ";
  statusString += temp; 
  statusString += " °C ";
  statusString += " Humidity: ";
  statusString += hum;
  statusString += " %";
  return statusString;
}
void clearLCDLine(int line){
  for(int n = 0; n < 16; n++) { // 20 indicates symbols in line. For 2x16 LCD write - 16
    lcd.setCursor(n,line);
    lcd.print(" ");
  }
  lcd.setCursor(0,line); // set cursor in the beginning of deleted line
}

int read_async() {
  static int state = 0;
  int ret;
  float oldReading = oldTemp;

  switch (state) {
    case 0:
      if (stringComplete) {
        return -1;
      }
      state++;
      return -1;
    case 1:
      if (!stringComplete || (temp == oldReading)) {
        // sendReadings();
        return -1;
      }
      state++;
      return -1;
    default:
      ret = 1;
      state = 0;
      return ret;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar = '\n') {
      stringComplete = true;
    }
  }
}