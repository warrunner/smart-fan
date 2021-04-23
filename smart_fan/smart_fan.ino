
/**
 * Created by Team 26 as part of Minor Project
*/

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <amt1001_ino.h>

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include "DHT.h"

//Declaring DHT variable
DHT dht;

//Declaring servo variable
Servo servo;

//LM35 variables
float vref = 5.0;
float resolution = vref/1023;

//Google sheets ID
String readString;
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "";  // Replace by your GAS service id


//WIFI authenticatoin and firebase credentials update
#define WIFI_SSID "SSID"  //add wifi ssid
#define WIFI_PASSWORD "password" //add password
#define FIREBASE_HOST "" //the firebase host address is removed for security reasons
#define FIREBASE_AUTH "" //firebase authentication ID is removed for secuirty reasons

//Define FirebaseESP8266 data object for data sending and receiving
FirebaseData fbdo;

//optimal speed calculation
int correspondingSpeed(int temp, int hum){
  int ideal = 5;
  //temperature greater than 37
  if(temp >=37 && hum<15){
    ideal = 3;
    return ideal;
  }
  else if(temp>=37 && hum<55){
    ideal = 4;
    return ideal;
  }
  else if(temp>=37 && hum>=55){
    ideal = 5;
    return ideal;
  }
  //temperatures between 37 and 30
  if(temp>=30 && temp<37 && hum<15){
    ideal = 3;
    return ideal;
  }
  else if(temp>=30 && temp<37 && hum<55){
    ideal = 4;
    return ideal;
  }
  else if(temp>=30 && temp<37 && hum>=55){
    ideal = 5;
    return ideal;
  }
  //temperatures less than 30
  if(temp < 30 && hum<15){
    ideal = 1;
    return ideal;
  }
  else if(temp<30 && hum<45){
    ideal = 2;
    return ideal;
  }
  else if(temp<30 && hum<75){
    ideal = 3;
    return ideal;
  }
  else if(temp<30 && hum>= 75){
    ideal = 4;
    return ideal;
  }
}

//setting fan speed
int setFanspeed(int fanspeed){
  if(fanspeed == 0){
    servo.write(0);
  }
  else if(fanspeed == 1) {
    servo.write(90);
  }
  else if(fanspeed == 2) {
    servo.write(110);
  }
  else if(fanspeed == 3) {
    servo.write(135);
  }
  else if(fanspeed == 4) {
    servo.write(155);
  }
  else if(fanspeed == 5) {
    servo.write(180);
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  dht.setup(D1); /*D1 is used for data communication*/
  servo.attach(4,0,2000); //(pin, min pulse width, max pulse width in microseconds)
  servo.write(0); //motor comes to HALT

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  //Setting the Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);


  //The following code is to check for proper integration with Firebase
  //Trying to set int data to Firebase
  //The set function returns bool for the status of operation
  //fbdo requires for sending the data
  if(Firebase.setInt(fbdo, "/LED_Status", 1))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }


  //Try to get int data from Firebase
  //The get function returns bool for the status of operation
  //fbdo requires for receiving the data
  if(Firebase.getInt(fbdo, "/LED_Status"))
  {
    //Success
    Serial.print("Get int data success, int = ");
    Serial.println(fbdo.intData());

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }

  
 if(Firebase.get(fbdo, "/LED_Status"))
  {
    //Success
    Serial.print("Get variant data success, type = ");
    Serial.println(fbdo.dataType());

    if(fbdo.dataType() == "int"){
      Serial.print("data = ");
      Serial.println(fbdo.intData());
    }else if(fbdo.dataType() == "bool"){
      if(fbdo.boolData())
        Serial.println("data = true");
      else
        Serial.println("data = false");
    }

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in get, ");
    Serial.println(fbdo.errorReason());
  }
}
String switcch = "OFF"; //switch position
String moDe = "AUTO";   //mode
String sPeed = "5";     // speed level

//Messages for Application interface
char *myStrings[] = {"Sit back and relax!", "You're pro bro!", "Relaxxxxxxxxxx!!!!!", "Comfort is priority", "Chilling is thrilling", "My fan is smart", "Enjoy the day!"}; //j
char *offStrings[] = {"Why not turn on the Fan?", "Toggle the ON button for cool breeze"}; //k
int i =-1;
int j = 3;
int k = 1;
void loop()
{
  Serial.println("-------------------Starts here-------------");
  //changing j and k values for messages
  j = j+1;
  if(j>6){
    j = 0;
  }
  k = k + 1;
  if(k>1) {
    k = 0;
  }
  digitalWrite(LED_BUILTIN, LOW);
  //reading temperature and humidity values
  int temperature = analogRead(A0);
  temperature = (temperature*resolution);
  temperature = temperature*100;
  int humidity = dht.getHumidity();
  //converting them into strings for updating into rtdb
  String temp = String(temperature);
  String hum = String(humidity);
  //updating temperature in database
  if(Firebase.setString(fbdo, "/controller/temp", temp)){
    Serial.print(temp + " ");
    Serial.println("Temperature Updated");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    //printing error in case of failure
    Serial.print("Error in set, ");
    Serial.println(fbdo.errorReason());
  }
  digitalWrite(LED_BUILTIN, LOW);
  //updating humidity in database
  if(Firebase.setString(fbdo, "/controller/hum", hum)){
    Serial.print(hum + " ");
    Serial.println("Humidity Updated");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    //printing error in case of failure
    Serial.print("Error in setting humidity, ");
    Serial.println(fbdo.errorReason());
  }
  digitalWrite(LED_BUILTIN, LOW);

  //calculate corresponding speed
  int idealspeed = correspondingSpeed(temperature, humidity);
  String ideal = String(idealspeed);

  //updating suggested speed
  if(Firebase.setString(fbdo, "/controller/test", ideal)){
    Serial.print(ideal + " ");
    Serial.println("Suggested speed updated");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    //printing error in case of failure
    Serial.print("Error in setting suggested speed, ");
    Serial.println(fbdo.errorReason());
  }
  digitalWrite(LED_BUILTIN, LOW);

  //updating commands
  String command = "Suggested speed is " + ideal;
  /*
   * 
   * this is used for updating the fan speeed. This is commented because other updates 
   * being done in the later section of the programme
  if(Firebase.setString(fbdo, "/controller/command", command))
  {
    Serial.print(ideal + " ");
    Serial.println("Command updated");
  }
  else{
    Serial.print("Error in setting command, ");
    Serial.println(fbdo.errorReason());
  }
  */

  //reading whether fan is ON/OFF
  if(Firebase.getString(fbdo, "/android/switch")){
    switcch = fbdo.stringData();
    Serial.print(switcch + " ");
    Serial.println("Switch position");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    //printing error in case of failure
    Serial.print("Error in reading switch, ");
    Serial.println(fbdo.errorReason());
    return;
  }
  digitalWrite(LED_BUILTIN, LOW);

  //if switch is off

  if(switcch == "OFF"){
    //updating the command as Fan is OFF
    if(Firebase.setString(fbdo, "/controller/command", offStrings[k])){
      Serial.print(switcch + " ");
      Serial.println("Command updated");
    }
    else{
      Serial.print("Error in reading switch, ");
      Serial.println(fbdo.errorReason());
    }
    //switching off the Fan
    setFanspeed(0);

    //sending data to google sheets
    sendData(idealspeed, 0);
    delay(1000);
    return;
  }
  
  //if switch is on

  //reading whether mode is auto/manual
  if(Firebase.getString(fbdo, "/android/mode")){
    moDe = fbdo.stringData();
    Serial.print(moDe + " ");
    Serial.println("Mode");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    Serial.print("Error in reading mode, ");
    Serial.println(fbdo.errorReason());
    return;
  }
  digitalWrite(LED_BUILTIN, LOW);

  //if mode is auto, switch the fan to ideal speed and update the speed in android section. The loop will continue from this.
  if(moDe == "AUTO"){
    Serial.print(ideal + "speed");
    Serial.println("auto switched");
    //updating ideal speed
    //set fan speed
    setFanspeed(idealspeed);
    if(Firebase.setString(fbdo, "/android/speed", ideal)){
      Serial.print(ideal + " ");
      Serial.println("auto speed updated");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else{
      Serial.print("Error in updating auto ideal speed, ");
      Serial.println(fbdo.errorReason());
    }

    //updating the sentences
    if(Firebase.setString(fbdo, "controller/command", myStrings[j])){
      Serial.print(ideal + " ");
      Serial.println("command updated");
    }
    else{
      Serial.print("Error in updating command, ");
      Serial.println(fbdo.errorReason());
    }

    //sending data to google sheets
    sendData(idealspeed, idealspeed);

    //delaying for 10 seconds
    delay(1000);
    return;
  }
  digitalWrite(LED_BUILTIN, LOW);

  //reading user's fan speed from android section
  if(Firebase.getString(fbdo, "/android/test")){
    sPeed = fbdo.stringData();
    
    int fanspeed = sPeed.toInt();
    //set speed
    setFanspeed(fanspeed);

    //sending data to google sheet
    sendData(idealspeed, fanspeed);

    
    Serial.print(sPeed + " ");
    Serial.println("Fan speed set");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    Serial.print("Error in reading speed, ");
    Serial.println(fbdo.errorReason());
  }

  //updating command
  if(Firebase.setString(fbdo, "/controller/command", "Try our new Auto mode! Suggested speed is " + ideal)){
    Serial.print(ideal + " ");
    Serial.println("Command updated");
  }
  else{
    Serial.print("Error in setting command, ");
    Serial.println(fbdo.errorReason());
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  
  
}


//google sheets sending data
void sendData(int x, int y)
{
 client.setInsecure();
 Serial.print("connecting to ");
 Serial.println(host);
 if (!client.connect(host, httpsPort)) {
   Serial.println("connection failed");
   return;
 }
 if (client.verify(fingerprint, host)) {
 Serial.println("certificate matches");
 } else {
 Serial.println("certificate doesn't match");
 }
 String string_x     =  String(x, DEC);
 String string_y     =  String(y, DEC);
 String url = "/macros/s/" + GAS_ID + "/exec?Value1=" + string_x + "&Value2=" + string_y;
 Serial.print("requesting URL: ");
 Serial.println(url);
 client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "User-Agent: BuildFailureDetectorESP8266\r\n" +
        "Connection: close\r\n\r\n");
 Serial.println("request sent");
 while (client.connected()) {
 String line = client.readStringUntil('\n');
 if (line == "\r") {
   Serial.println("headers received");
   break;
 }
 }
 String line = client.readStringUntil('\n');
 if (line.startsWith("{\"state\":\"success\"")) {
 Serial.println("esp8266/Arduino CI successfull!");
 } else {
 Serial.println("esp8266/Arduino CI has failed");
 }
 Serial.println("reply was:");
 Serial.println("==========");
 Serial.println(line);
 Serial.println("==========");
 Serial.println("closing connection");
}
