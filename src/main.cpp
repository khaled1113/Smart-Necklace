//incuding the required Libraries
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <WebServer.h>
//Access point ssid and password
const char *ap_ssid = "Ahmed Waleed Smart Necklace";
const char *ap_password = "12345678";
//Creating a web server at port 80
WebServer server(80);
//declaring the variables of the local network and the email
String local_ssid;
String local_password;
String recipient_email;
int fall_threshold = 20; // Initial threshold value
//Creating the objects of the required libraries
Adafruit_MPU6050 mpu;
//creating the object of the SMTP session
SMTPSession smtp;
//Function to handle the root page and have the html form to enter the required data
void handleRoot() {
  server.send(200, "text/html",
              "<style>"
              "body { display: flex; justify-content: center; align-items: center; height: 100vh; background-color: #A7BEAE; font-family: Arial, sans-serif; color: #B85042; }"
              "form { background-color: #E7E8D1; padding: 50px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); width: 400px; color: #B85042; }"
              "input[type='text'] { display: block; margin-bottom: 20px; padding: 20px; width: 100%; border: 1px solid #ccc; border-radius: 5px; font-size: 18px; color: #B85042; background-color: #E7E8D1; }"
              "input[type='submit'] { background-color: #B85042; color: white; padding: 20px 25px; border: none; border-radius: 5px; cursor: pointer; font-size: 18px; }"
              "input[type='submit']:hover { background-color: #8B3A32; }"
              "</style>"
              "<form action=\"/submit\" method=\"POST\">"
              "SSID: <input type=\"text\" name=\"ssid\"><br>"
              "Password: <input type=\"text\" name=\"password\"><br>"
              "Email: <input type=\"text\" name=\"email\"><br>"
              "Threshold: <input type=\"text\" name=\"threshold\" value=\"20\"><br>"
              "<input type=\"submit\" value=\"Submit\">"
              "</form>");
}
//Function to handle the submit button and get the data from the form
void handleSubmit() {
  //hassarg function to check if the form has the required data
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("email") && server.hasArg("threshold")) {
    //assigning the data to the variables
    local_ssid = server.arg("ssid");
    local_password = server.arg("password");
    recipient_email = server.arg("email");
    fall_threshold = server.arg("threshold").toInt();
//Serial printing the data
    Serial.println("SSID: " + local_ssid);
    Serial.println("Password: " + local_password);
    Serial.println("Email: " + recipient_email);
    Serial.println("Threshold: " + String(fall_threshold));

    server.send(200, "text/plain", "Data received. Check Serial Monitor.");

    WiFi.begin(local_ssid.c_str(), local_password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void sendEmail() {
  ESP_Mail_Session session;
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 465;
  session.login.email = "kmr444166@gmail.com";
  session.login.password = "fnoc bwiw ruaa mezb";
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "ESP 32";
  message.sender.email = "kmr444166@gmail.com";
  message.subject = "Fall Detected!";
  message.addRecipient("", recipient_email.c_str());

  String htmlMsg = "<div style=\"color:#000000;\"><h1>Fall Detected!</h1><p>A fall has been detected by the ESP32.</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 15);
  Serial.println("Initializing MPU6050");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  Serial.println("MPU6050 initialized");

  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point Created");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.begin();

  smtp.debug(1);
}

void loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    int res = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
    Serial.print("Resultant acceleration: ");
    Serial.println(res);

    if (res >= fall_threshold) {
      Serial.println("Fall detected");
      sendEmail();
    }

    delay(50);
  }
}