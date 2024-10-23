#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include<WebServer.h>
#include<WiFi.h>
const char* ssid = "Accelometer Data";
const char* password = "12345678";

//what is the meaning of the web server (80) ??
WebServer server(80);

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(4,15);
  Serial.println("Initializing mpu 6050");
  if(!mpu.begin()){
    Serial.println("Failed to find MPU6050 chip");
    while(1);
    //if the device is not found then the program will be stuck in infinite loop till
  }
  Serial.println("MPU6050 is intialized");
}

void loop () {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a,&g,&temp);
  Serial.print("Acceleration of x :");
  Serial.print(a.acceleration.x);
  Serial.print("Y: ");
  Serial.print(a.acceleration.y);
  Serial.print("Z:");
  Serial.print(a.acceleration.z);

  Serial.print("Gyro of x :");
  Serial.print(g.gyro.x);
  Serial.print(" Y:");
  Serial.print(g.gyro.y);
  Serial.print(" Z:");
  Serial.print(g.gyro.z);

  Serial.print(" Temperature :");

  Serial.println(temp.temperature);
  delay (500);
  // how to make serial print in a single line
  Serial.print("Acceleration of y :");
}