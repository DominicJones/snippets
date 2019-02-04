void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}


void loop()
{
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue, DEC);

  if(sensorValue > 50) // Threshold value
    {
      digitalWrite(13, HIGH);
    }
  else
    {
      digitalWrite(13, LOW);  
    }
}
