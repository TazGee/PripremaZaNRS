#include<arduinoPlatform.h>
#include<tasks.h>
#include<interrupts.h>
#include<stdio.h>
#include<serial.h>
#include <pwm.h>
#include<data.h>

extern serial Serial;

const int pocetnaLampica = 26;

const int SW1 = 2;
const int SW2 = 7;
const int SW3 = 8;
const int SW4 = 35;
const int SW4_ID = 4;

const int BTN1 = 4;
const int BTN2 = 34;
const int BTN3 = 36;

int x1, x2;
int oldState[3], newState[3];

int pressTime;
bool handled;

void swInterrupt()
{
    int zbir = x1 + x2;
    Serial.print("interrup SW4: ");
    Serial.println(zbir);

    int lamp[4];
    for(int i = 0; i < 4; i++)
        lamp[i] = LOW;

    for (int i = 3; i >= 0; i--) {
        lamp[i] = zbir % 2;
        zbir /= 2;
    }

    for(int i = 0; i < 4; i++)
        digitalWrite(30+i, lamp[i]);
}

int fun1()
{
    bool stanje[3];

    stanje[0] = digitalRead(SW1);
    stanje[1] = digitalRead(SW2);
    stanje[2] = digitalRead(SW3);

    for(int i = 0; i < 3; i++)
        digitalWrite(pocetnaLampica+i, stanje[i]);

    return stanje[0] + stanje[1] + stanje[2];
}

void task1(int id, void* tptr)
{
    newState[0] = digitalRead(BTN1);
    newState[1] = digitalRead(BTN2);
    if((newState[0] && !oldState[0]) || (newState[1] && !oldState[1]))
    {
        x1 = fun1();
        Serial.print("task1: ");
        if(newState[1] && !oldState[1])
        {
            x2 = 2*x1;
            Serial.print("x2=");
            Serial.println(x2);
        }
        else
        {
            Serial.print("x1=");
            Serial.println(x1);
        }
    }
    oldState[0] = newState[0];
    oldState[1] = newState[1];
}

void task2(int id, void* tptr)
{
    newState[2] = digitalRead(BTN3);
    int currentTime = millis();

    if(!oldState[2] && newState[2])
    {
        pressTime = currentTime;
        handled = false;
    }

    if(oldState[2] && newState[2] && (currentTime - pressTime > 2000) && !handled)
    {
        Serial.println("task2: brisanje interup-a sa prekidaca SW4");
        detachInterrupt(SW4_ID);
        handled = true;
    }

    oldState[2] = newState[2];
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Nikola Tasic PR68/2023");

    for(int i = 0; i < 8; i++)
        pinMode(pocetnaLampica+i, OUTPUT);

    oldState[0] = LOW;
    oldState[1] = LOW;
    oldState[2] = LOW;
    pressTime = 0;
    handled = false;

    createTask(task1, 30, TASK_ENABLE, NULL);
    createTask(task2, 25, TASK_ENABLE, NULL);

    attachInterrupt(SW4_ID, swInterrupt, RISING);
}

void loop()
{

}
