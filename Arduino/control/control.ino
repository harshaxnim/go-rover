int vbose = 0;
int endRead=0, bg=0, rd=0;
char* msg;
int updateInstruction=0, sendTemperature=0, sendHumidity=0, sendPressure=0;

void s(String a){
  // Send
  Serial1.println(a);
  if(vbose){
    Serial.print("\n_-_-_-_\n>>S: ");
    Serial.println(a);
  }
  delay(100);
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
      if(rd>=21) Serial.println("Buffer limited to 20. Update the code if necessary.");
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

void respond(){
  if(updateInstruction){
    //do something
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

void setup() {
  delay(100);
  Serial.begin (115200);
  Serial1.begin(115200);
  delay(100);
  
  Serial.println("Initialising variables..");
  msg = (char*)malloc(21);
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
}
