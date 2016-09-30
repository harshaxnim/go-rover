/* Serial pass through for teensy */

void setup()
{
  delay(5000);
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop()
{
  /* send everything received from the hardware uart to usb serial & vv */
  if (Serial.available() > 0) {
    char ch = Serial.read();
    Serial1.print(ch);
  }
  if (Serial1.available() > 0) {
    char ch = Serial1.read();
    Serial.print(ch);
  }
}
