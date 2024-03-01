#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <TimeLib.h>
//#define BUZZER_PIN 12
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#define INCLUDE_DATALOGGER_MODULE

float AccelX, AccelY, AccelZ;
float AccelCalibrationX, AccelCalibrationY, AccelCalibrationZ;
int CalibrationSamples;
String dataString="";

// const byte led_gpio = 32;
// const int buzzerPin = 34;


// bool buzzer_activate = false;
// unsigned long lastRedLedOnTime = 0;
float totalAcceleration;
int timeCount=0;
float rmsValue = 0;

// for RMS value
const int sampleRate = 100;  // Sampling rate in Hz
const int totalTime = 10;    // Total time in seconds
const int numSamples = 10;
int sampleCounter = 0;
float collectData[numSamples];

Adafruit_MPU6050 mpu;

// void initializeFile(){         //yeni
//   Serial.println("Initialize");
//   DataLogger.createFile("Data");
//   DataLogger.createColumn("X");
//   DataLogger.createColumn("Y");
//   DataLogger.createColumn("Z");
//   DataLogger.createColumn("RMS Value");
// }

// void Task1code( void * pvParameters ){
//   Serial.print("Task1 running on core ");
//   Serial.println(xPortGetCoreID());

//   /*for(;;){
//     appendFile(SD, "/data1.txt", dataString.c_str());
//     delay(200);
//   } */
// }

void setup() {
  Serial.begin(115200);
// if(!SD.begin()){
//         Serial.println("Card Mount Failed");
//         return;
//     }
//     uint8_t cardType = SD.cardType();

//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }

  // xTaskCreatePinnedToCore(
  //     Task1code, /* Function to implement the task */
  //     "Task1", /* Name of the task */
  //     10000,  /* Stack size in words */
  //     NULL,  /* Task input parameter */
  //     0,  /* Priority of the task */
  //     &Task1,  /* Task handle. */
  //     0); /* Core where the task should run */
  
  // pinMode(led_gpio, OUTPUT);
  // pinMode(buzzerPin, OUTPUT);
  
  //Serial.print("setup() running on core ");
  //Serial.println(xPortGetCoreID());
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 sensor, check wiring!");
    while (1);
  }

  Serial.println("MPU6050 found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);

  CalibrationSamples = 2000; // Number of calibration samples
  for (int i = 0; i < CalibrationSamples; i++) {
    readAccelerometer(); // Read accelerometer data
    AccelCalibrationX += AccelX;
    AccelCalibrationY += AccelY;
    AccelCalibrationZ += AccelZ;
    delay(3); // Delay between samples
  }
  
  // Calculate average calibration values
  AccelCalibrationX /= CalibrationSamples;
  AccelCalibrationY /= CalibrationSamples;
  AccelCalibrationZ /= CalibrationSamples;

//   Dabble.begin("Esp32");
// //  Dabble.waitForAppConnection();
//   DataLogger.sendSettings(&initializeFile);
}

void loop() {
  //Serial.print("loop() running on core ");
  //Serial.println(xPortGetCoreID());

  // Dabble.processInput();

  readAccelerometer();
  
  float CalibratedAccelX = AccelX - AccelCalibrationX;
  float CalibratedAccelY = AccelY - AccelCalibrationY;
  float CalibratedAccelZ = AccelZ - AccelCalibrationZ;

  if (abs(CalibratedAccelX) < 1) {
    CalibratedAccelX = CalibratedAccelX * 10;
  }
  
  if (abs(CalibratedAccelY) < 1) {
    CalibratedAccelY = CalibratedAccelY * 10;
  }
  
  if (abs(CalibratedAccelZ) < 1) {
    CalibratedAccelZ = CalibratedAccelZ * 10;
  }

  //Print calibrated accelerometer data
  // Serial.print("Calibrated Acceleration (X,Y,Z): ");
  // Serial.print(CalibratedAccelX);
  // Serial.print(", ");
  // Serial.print(CalibratedAccelY);
  // Serial.print(", ");
  //Serial.println(CalibratedAccelZ);
  
  collectData[sampleCounter] = (CalibratedAccelX * CalibratedAccelX) + 
                                (CalibratedAccelY * CalibratedAccelY) + 
                                (CalibratedAccelZ * CalibratedAccelZ);
  
  sampleCounter++;
  if (sampleCounter == numSamples) {
    float sumSamples = 0;
    for (int i = 0; i < sampleCounter; ++i) {
      sumSamples += collectData[i];
    }
    rmsValue = sqrt(sumSamples / sampleCounter);
    sampleCounter = 0; 
  }
  // if(rmsValue>3){
  //   // Notification.notifyPhone(String("Rms Value: ") + rmsValue);

  //   // tone(buzzerPin, 1000);
  //   // digitalWrite(led_gpio,HIGH);
  // }
  // else{
  //   noTone(buzzerPin);
  //   digitalWrite(led_gpio,LOW);
  // }
  String timeString = String(hour()) + ":" + String(minute()) + ":" + String(second()) + ":" + String(millis() % 1000);

  dataString = timeString + " - RMS Value: " + String(rmsValue) +
                      ", Acceleration (X,Y,Z): " + String(CalibratedAccelX) +
                      ", " + String(CalibratedAccelY) + ", " +
                      String(CalibratedAccelZ) + "\n";


  // DataLogger.send("X",CalibratedAccelX);
  // DataLogger.send("Y",CalibratedAccelY);
  // DataLogger.send("Z",CalibratedAccelZ);
  // DataLogger.send("RMS Value",rmsValue);

  Serial.println(dataString);
  delay(200);
}

void readAccelerometer() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  AccelX = a.acceleration.x;
  AccelY = a.acceleration.y;
  AccelZ = a.acceleration.z;
}

/*void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

String getData() {
  return dataString;
}*/
