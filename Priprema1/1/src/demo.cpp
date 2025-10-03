#include<arduinoPlatform.h>
#include<tasks.h>
#include<interrupts.h>
#include<stdio.h>
#include<serial.h>
#include <pwm.h>
#include<data.h>

extern serial Serial;

const int lampica = 26;
const int dugme = 4;
const int prekidac = 2;
const int idPrekidaca = 1;

int br, lampica_id;
int buttonState;

void interruptOff();

void interruptOn()
{
    br++;
    if(br > 10) br = 1;
    detachInterrupt(idPrekidaca);
    attachInterrupt(idPrekidaca, interruptOff, RISING);
}

void interruptOff()
{
    br++;
    if(br > 10) br = 1;
    detachInterrupt(idPrekidaca);
    attachInterrupt(idPrekidaca, interruptOn, FALLING);
}

void blinkanje(int id, void * tptr)
{
    digitalWrite(lampica, !digitalRead(lampica));
}

void serialMonitor(int id, void * tptr)
{
    if(Serial.available())
    {
        char input = Serial.read();
        if(input <= '9' && input >= '0')
        {
            br = (input - '0') + 1;
            setTaskPeriod(lampica_id, 1000/(2*br));
        }
    }
}

void buttonTask(int id, void * tptr)
{
    int vrednost = digitalRead(dugme);

    if(vrednost == HIGH && buttonState == LOW)
    {
        Serial.print("Period blinkanja je: ");
        Serial.println(1000/(2*br));
    }
    else if(vrednost == LOW && buttonState == HIGH)
    {
        setTaskPeriod(lampica_id, 1000/(2*br));
    }

    buttonState = vrednost;
}

void setup()
{
    pinMode(lampica, OUTPUT);

    Serial.begin(9600);

    if(digitalRead(prekidac))
    {
        attachInterrupt(idPrekidaca, interruptOn, FALLING);
    }
    else
    {
        attachInterrupt(idPrekidaca, interruptOff, RISING);
    }

    br = 1;
    buttonState = LOW;

    lampica_id = createTask(blinkanje, 1000/(2*br), TASK_ENABLE, NULL);
    createTask(serialMonitor, 50, TASK_ENABLE, NULL);
    createTask(buttonTask, 50, TASK_ENABLE, NULL);
}

void loop()
{

}
