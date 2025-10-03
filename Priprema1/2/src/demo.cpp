#include<arduinoPlatform.h>
#include<tasks.h>
#include<interrupts.h>
#include<serial.h>

extern serial Serial;

int oldState, oldTime;

int analogInPin = A0;
int PIN_LD1 = 26;
int PIN_BTN1 = 4;
int PIN_SW1 = 2;
int INT_SW1 = 1;
int br, dugme_id;

void prikazi(){
   int sensorValue = analogRead(analogInPin);
   int outputValue = map(sensorValue, 0, 1023, 0, 255);
   for(int i=26; i<34; i++){
      digitalWrite(i, outputValue%2);
      outputValue = outputValue/2;
   }
}

void dugme(int id, void * tptr)
{
    int newState = digitalRead(PIN_BTN1);
	if (oldState == 1  && newState == 0) {
        if (millis()-oldTime>=2000) {
            int sensorValue = analogRead(analogInPin);
            br = br+sensorValue;
            Serial.println(br);
		}
	} else if (oldState == 0 && newState ==1) {
        oldTime = millis();
	}
	oldState = newState;
}

void obrisiInterrupt(int id, void * tptr)
{
   if (Serial.available()){
      char *s = Serial.readString();
      // na plocici pisati
      //const char* s = Serial.readString().c_str();
      int x = atoi(s);
      if (x<br) {
        setTaskState(dugme_id, TASK_DISABLE);
        Serial.println("Task onemogucen");
      } else
        Serial.println("Task nije onemogucen");
   }
}

void setup()
{
  Serial.begin(9600);
  for(int i = 0; i<8; i++)
    pinMode(PIN_LD1+i, OUTPUT);
  br = 0;
  oldTime = millis();
  dugme_id = createTask(dugme, 50, TASK_ENABLE, NULL);
  attachInterrupt(INT_SW1, prikazi,RISING);
  createTask(obrisiInterrupt, 50, TASK_ENABLE, NULL);
}

void loop()
{

}
