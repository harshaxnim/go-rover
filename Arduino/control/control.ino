#include <DHT.h>

// Global Variables /////////////////////////////////////////////////////////
#define SPKPIN 4
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int BUFFER_LENGTH = 20;
const int TIMER_COUNT = 2;
const int MOVE_LATENCY= 100;

const int motorLP =  6;
const int motorLN =  8;
const int motorRP = 10;
const int motorRN = 12;

int vbose = 0;
int endRead=0, bg=0, rd=0;
char* msg;
int updateInstruction=0, sendTemperature=0, sendHumidity=0, sendPressure=0;

struct timer{
  int status=0;
  unsigned long time=0;
}typedef timer;
timer* timers;
/////////////////////////////////////////////////////////////////////////////

// ESP Control API //////////////////////////////////////////////////////////
void s(String a){
  // Send
  Serial1.println(a);
  if(vbose){
    Serial.print("\n_-_-_-_\n>>S: ");
    Serial.println(a);
  }
  delay(100); // XXX
}

void r(){
  char c;
  while (Serial1.available() > 0) {
    
    c = Serial1.read();
    if( (c=='\n' || c=='\r') && rd ){ // got the message, now terminating
        if(vbose) Serial.println();
        endRead = 1;
        msg[rd-1] = '\0';
        return;
    }
    else if(rd){
      if(vbose) Serial.write(c);
      msg[rd-1]=c;
      rd++;
      if( rd>=BUFFER_LENGTH ) Serial.println("Buffer limited to 20. Update the code if necessary.");
    }
    else if(c=='+'){
      if(vbose) Serial.println("+");
      bg=1;
    }
    else if(c==':'){
      if(vbose) Serial.println(":");
      rd=1;
    }     
  }
}

void sendMessage(String c){
  c = c+'\n';
  int l = c.length();
  String header = "AT+CIPSEND=0,"; 
  s(header+l);
  s(c);
}

void prompt(){
  tone(SPKPIN, 1000, 50);
  String c = "shove-a-rover$ ";
  int l = c.length();
  String header = "AT+CIPSEND=0,"; 
  s(header+l);
  s(c);
}
/////////////////////////////////////////////////////////////////////////////

// Motor Control Code ///////////////////////////////////////////////////////
void forward(){
  // Left and Right motor forward
  digitalWrite(motorLP, HIGH); // 6
  digitalWrite(motorLN, LOW); //  8
  digitalWrite(motorRP, HIGH);
  digitalWrite(motorRN, LOW);
}
void backward(){
  // Left and Right motor backward
  digitalWrite(motorLP, LOW);
  digitalWrite(motorLN, HIGH);
  digitalWrite(motorRP, LOW);
  digitalWrite(motorRN, HIGH);
}
void left(){
  // Left motor backward
  digitalWrite(motorLP, LOW);
  digitalWrite(motorLN, HIGH);
  // Right motor forward
  digitalWrite(motorRP, HIGH);
  digitalWrite(motorRN, LOW);
}
void right(){
  // Left motor forward
  digitalWrite(motorLP, HIGH);
  digitalWrite(motorLN, LOW);
  // Right motor backward
  digitalWrite(motorRP, LOW);
  digitalWrite(motorRN, HIGH);
}
void stop(){
  // All low
  digitalWrite(motorLP, LOW);
  digitalWrite(motorLN, LOW);
  digitalWrite(motorRP, LOW);
  digitalWrite(motorRN, LOW);
}
/////////////////////////////////////////////////////////////////////////////

// Brains here //////////////////////////////////////////////////////////////
void handleTimers(){
  for (int i=0; i<TIMER_COUNT; i++){ // Check with all the timers
    unsigned long current = millis();
    
    if( timers[i].status && (current > timers[i].time) ){ // Alarm
      stop(); // Stop the motors
      timers[0].status = 0;
      sendMessage("<<< Done Moving. Awaiting further instructions..");
      prompt();
      break;
    }
  }
}

void respond(){
  if(updateInstruction){
    
    if(msg[1] == 'r'){
      right();
      if(vbose) sendMessage("Moving right..");
      timers[0].status = 1;
      timers[0].time = millis()+MOVE_LATENCY;
    }
    else if(msg[1] == 'l'){
      left();
      if(vbose) sendMessage("Moving left..");
      timers[0].status = 1;
      timers[0].time = millis()+MOVE_LATENCY;
    }
    else if(msg[1] == 'f'){
      forward();
      if(vbose) sendMessage("Moving forward..");
      timers[0].status = 1;
      timers[0].time = millis()+MOVE_LATENCY;
    }
    else if(msg[1] == 'b'){
      backward();
      if(vbose) sendMessage("Backing up..");
      timers[0].status = 1;
      timers[0].time = millis()+MOVE_LATENCY;
    }
    else{
      sendMessage("Estúpido! USAGE: i<d>\nd: l, r, f or b");
    }
    
    updateInstruction = 0;
  }
  else if(sendTemperature){
    int c = 0;
    READTEMP: float t = dht.readTemperature();
    if ( isnan(t) ) {
      Serial.println("Failed reading from temp sensor. Trying again.");
      c++;
      if(c>10) {
        sendMessage("Failed to read");
        sendTemperature = 0;
        return;
      }
      goto READTEMP;
    }
    String header = "Temperature: ";
    String footer = " ºC";
    sendMessage(header+t+footer);
    sendTemperature = 0;
  }
  else if(sendHumidity){
    int c = 0;
    READHUMID: float h = dht.readHumidity();
    if ( isnan(h) ) {
      Serial.println("Failed reading from humidity sensor. Trying again.");
      c++;
      if(c>10) {
        sendMessage("Failed to read");
        sendHumidity= 0;
        return;
      }
      goto READHUMID;
    }
    String header = "RelHumidity: ";
    String footer = " %";
    sendMessage(header+h+footer);
    sendHumidity = 0;
  }
  else if(sendPressure){
    //do something
    sendMessage("Pressure sensor not available");
    sendPressure = 0;
  }
}

void analyse(){
  Serial.println(msg);
  
  if(!strncmp("hi",msg,2)){
    sendMessage("Hola! No hablo ingles!");
  }
  else if(!strncmp("i",msg,1)){
    updateInstruction = 1;
  }
  else if(!strcmp("t",msg)){
    sendTemperature = 1;
  }
  else if(!strcmp("h",msg)){
    sendHumidity = 1;
  }
  else if(!strcmp("p",msg)){
    sendPressure = 1;
  }
  else if(!strcmp("quit",msg)){
    sendMessage("See you later Aligator!");
    s("AT+CIPCLOSE=0"); // Close the connection
  }
  else{
    sendMessage("¿Que? ¡No te entiendo!");
  }
  
  bg = 0;
  rd = 0;
  endRead = 0;
}
/////////////////////////////////////////////////////////////////////////////

// Arduino Setup and Loop ///////////////////////////////////////////////////
void setup() {
  delay(100);
  Serial.begin (115200);
  Serial1.begin(115200);
  delay(100);
  
  Serial.println("Initialising variables..");
  msg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
  timers = (timer*)malloc(TIMER_COUNT*sizeof(timer));
  dht.begin();
  timers[0].status = 0;
  timers[1].status = 0;
  pinMode(motorLP, OUTPUT);
  pinMode(motorLN, OUTPUT);
  pinMode(motorRP, OUTPUT);
  pinMode(motorRN, OUTPUT);

  tone(SPKPIN, 1000, 100);
  delay(100);
  
  s("AT+CIPMUX=1"); // Enabling up MUX
  s("AT+CIPSERVER=1,7037"); // Start listening on this port
  Serial.println("Server listening on port 7037.");
  tone(SPKPIN, 1200, 100);
}

void loop() {
  r();
  
  if(endRead){
    analyse();
    respond();
    prompt();
  }
  
  handleTimers();
}
/////////////////////////////////////////////////////////////////////////////

