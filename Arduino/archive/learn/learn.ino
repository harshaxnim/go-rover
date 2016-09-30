int vbose = 1;

void serialFlush(){
  while(Serial1.available() > 0) {
    char t = Serial1.read();
  }
}

void sr(char* a){
  // Send
  Serial1.println(a);
  if(vbose){
    Serial.print("\n_-_-_-_\n><SR: ");
    Serial.println(a);    
  }
  //Recieve
  delay(50);
  while (Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }
  
  delay(100);
}

void s(char* a){
  // Send
  Serial1.println(a);
  if(vbose){
    Serial.print("\n_-_-_-_\n>>S: ");
    Serial.println(a);
  }
  delay(100);
}

void advertise(){
  s("AT+CIPSEND=15");
  s("Shove-a-Rover");
}

void r(){
  char c;
  int b = 0;
  while (Serial1.available() > 0) {
    c = Serial1.read();
    if(b) Serial.write(c);
    if(c=='+') b=1;
    else if(c=='\n' || c=='\r'){
      b=0;
      Serial.println();
      break;
    }
  }
  
}

void br(){
  char c;
  Serial.println("tryna read");
  while (Serial1.available() > 0) {
    c = Serial1.read();
    Serial.write(c);
  }
}

void setup() {
  Serial.begin (115200);
  Serial1.begin(115200);
  
  // Initial Setup- Run once, only if changed
  s("AT+RST"); // Enable this line when this setup seems not to work
  delay(2000);  
  s("ATE0");
  delay(1000);
  s("AT+CWSAP=\"Shove-a-Rover\",\"1123581321\",6,4"); // Setup AP
  delay(1000);
  s("AT+CWDHCP=0,1"); // Enable DHCP
  delay(1000);
  s("AT+CIPSTART=\"UDP\",\"192.168.4.2\",7037"); // Open a UDP Socket
}

void loop() {
  advertise();
  br();
  delay(1000);
}
