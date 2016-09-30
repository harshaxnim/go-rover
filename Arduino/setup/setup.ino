int vbose = 0;

void s(char* a){
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
    Serial.write(c);
  }
}

void setup() {

  delay(5000); // Initial delay to accommodate serial opening- debug
  
  Serial.begin (115200);
  Serial1.begin(115200);
  
  // Initial Setup- Run once
  Serial.println("Please Wait (upto 5 secs).. Resetting the device..");
  s("ATE1"); // Enable echo
  s("AT+RST"); // Reset the previous setup
  delay(5000);
  Serial.println("Done resetting. Standby for initiation of the module..");

  s("AT+CWSAP=\"Shove-a-Rover\",\"1123581321\",6,4"); // Setup AP
  s("AT+CWDHCP=0,1"); // Enable DHCP
  Serial.println("AP should now be up and running.");

  
  s("AT+CIPMUX=1"); // Enabling up MUX
  s("AT+CIPSERVER=1,7037"); // Start listening on this port
  Serial.println("Server listening on port 7037.");
  Serial.println("You can now telnet the connection to test.");
}

void loop() {
  r();
}
