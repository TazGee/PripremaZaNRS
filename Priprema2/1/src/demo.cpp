#include<arduinoPlatform.h>
#include<tasks.h>
#include<interrupts.h>
#include<stdio.h>
#include<serial.h>
#include<pwm.h>

extern serial Serial;

int myPIN = 26;
int oldState, periodTime, startTime, elapsedTime;

#define numOfLastValues 20
int numOfValues;
int values[numOfLastValues];
int brPojavljivanja[41];
int numOfErrors;
int suma = 0;
int suma20 = 0;

void brojevi(int id, void * tptr)
{
    // promenljiva za preskakanje prvog intervala, koji nije potpun :)
    static int first = true;
    int newState = digitalRead(myPIN);
	if (oldState == 1  && newState == 0) {
        elapsedTime = millis()-startTime;
	} else if (oldState == 0 && newState ==1) {
        int time = millis();
        periodTime = time-startTime;
        startTime = time;
        if (!first) {
           Serial.print(elapsedTime);
           Serial.print(' ');
           Serial.println(periodTime);
           int temperatura = map(elapsedTime, 0, 2000, 0, 40);
           suma20 = suma20- values[numOfValues%numOfLastValues] + temperatura;
           values[numOfValues%numOfLastValues] = temperatura;
           suma = suma+temperatura;
           brPojavljivanja[temperatura]++;
           numOfValues++;
           Serial.print("Prosecna temperatura: ");
           Serial.println(suma/numOfValues);
           if (numOfValues>=numOfLastValues) {
                Serial.print("Prosecna temperatura poslednjih 20 merenja: ");
                Serial.println(suma20/numOfLastValues);
           }
           if (numOfValues%numOfLastValues==0) {
              int maxJ = 0;
              for(int j=1; j<=40; j++)
                if (brPojavljivanja[j]>brPojavljivanja[maxJ])
                    maxJ = j;
              Serial.print("Najcesca temperatura: ");
              Serial.println(maxJ);
           }
        }
        first = false;
	}
	oldState = newState;
}

void setup()
{
  Serial.begin(9600);
  pwmSin(myPIN, 2000, 0.0);
  oldState = digitalRead(myPIN);
  startTime = millis();
  elapsedTime = 0;
  periodTime = 0;
  numOfValues = 0;
  for(int i=0; i<numOfLastValues; i++)
    values[i] = 0;
  for(int i=0; i<=40; i++)
    brPojavljivanja[i] = 0;
  createTask(brojevi, 1, TASK_ENABLE, NULL);
}

void loop()
{

}
