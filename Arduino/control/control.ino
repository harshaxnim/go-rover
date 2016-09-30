
// Global Variables /////////////////////////////////////////////////////////
const int BUFFER_LENGTH = 20;
const int TIMER_COUNT = 2;

const int motorLP = 6;
const int motorLN = 8;
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
/////////////////////////////////////////////////////////////////////////////

// Motor Control Code ///////////////////////////////////////////////////////
void forward(){
  // Left and Right motor forward
  digitalWrite(motorLP, HIGH);
  digitalWrite(motorLN, LOW);
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
  for (int i=0; i<TIMER_COUNT; i++){
    unsigned long current = millis();
    if( current > timers[i].time ){ // alarm
      stop();
    }
  }
}

void respond(){
  if(updateInstruction){
    if(msg[1] == 'r'){
      right();
      timers[0].status = 1;
      timers[0].time = millis();
    }
    else if(msg[1] == 'l'){
      left();
    }
    else if(msg[1] == 'f'){
      forward();
    }
    else if(msg[1] == 'b'){
      backward();
    }
    
    updateInstruction = 0;
  }
  else if(sendTemperature){
    //do something
    sendTemperature = 0;
  }
  else if(sendHumidity){
    //do something
    sendHumidity = 0;
  }
  else if(sendPressure){
    //do something
    sendPressure = 0;
  }
}

void analyse(){
  Serial.println(msg);
  
  if(!strncmp("hi",msg,2)){
    Serial.println("he");
    sendMessage("Hola! No hablo ingles!");
  }
  else if(!strncmp("i",msg,1)){
    updateInstruction = 1;
    sendMessage("¿Que?");
  }
  else if(!strncmp("t",msg,1)){
    sendTemperature = 1;
    sendMessage("temp");
  }
  else if(!strncmp("h",msg,1)){
    sendHumidity = 1;
    sendMessage("hum");
  }
  else if(!strncmp("p",msg,1)){
    sendPressure = 1;
    sendMessage("press");
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
  delay(100);
  
  s("AT+CIPMUX=1"); // Enabling up MUX
  s("AT+CIPSERVER=1,7037"); // Start listening on this port
  Serial.println("Server listening on port 7037.");
}

void loop() {
  r();
  if(endRead){
    analyse();
    respond();
  }
  handleTimers();
}
/////////////////////////////////////////////////////////////////////////////
