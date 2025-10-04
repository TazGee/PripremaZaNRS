#include<arduinoPlatform.h>
#include<tasks.h>
#include<interrupts.h>
#include<stdio.h>
#include<serial.h>
#include <pwm.h>
#include<data.h>

extern serial Serial;

#define maxBrVrednosti 30
#define maxBrUredjaja 21
int brojVrednosti = 0;
short napon[maxBrUredjaja];
short brUpozorenja[maxBrUredjaja];

void brojevi(int id, void *tptr)
{
    if(available())
    {
        char *buffer = readAll();
        int velicina = *((int *) buffer);
        for(int i=0; i<velicina; i++)
        {
            int brUredjaja = *(int*)(buffer + sizeof(int) + i * slogSize() + 32);
            int izmereniNapon = *(short*)(buffer + sizeof(int) + i * slogSize() + 36);
            int izmerenaJacina = *(short*)(buffer + sizeof(int) + i * slogSize() + 38);
            Serial.print(brUredjaja);
            Serial.print(" ");
            Serial.print(izmereniNapon);
            Serial.print(" ");
            Serial.println(izmerenaJacina);
            brojVrednosti++;
            if (abs(izmereniNapon-napon[brUredjaja])>30 && napon[brUredjaja]!=0) {
                brUpozorenja[brUredjaja]++;
                Serial.print("Upozorenje, neodgovarajuc napon na uredjaju broj: ");
                Serial.println(brUredjaja);
            }
            napon[brUredjaja] = izmereniNapon;

            if (brojVrednosti%maxBrVrednosti==0) {
                int maxJ = 0;
                for (int j=1; j<brUredjaja; j++)
                    if (brUpozorenja[maxJ]<brUpozorenja[j])
                        maxJ = j;
                Serial.print("Najvise upozorenja je na uredjaju broj: ");
                Serial.println(maxJ);
            }
        }
        delete[] buffer;
    }
}

void setup()
{
    Serial.begin(9600);
    for (int i = 0; i<maxBrUredjaja; i++) {
        napon[i] = 0;
        brUpozorenja[i] = 0;
    }
    brojVrednosti = 0;

    startStopDataGeneration(START_GENERATION, RANDOM, 0, 20, 0.0, 1000);

    createTask(brojevi, 1000, TASK_ENABLE,  NULL);
}

void loop()
{

}
