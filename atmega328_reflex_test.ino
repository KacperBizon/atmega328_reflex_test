#include <TM1637Display.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);

const byte switch_p1 = 4;                 //player 1 switch
const byte switch_p2 = 5;                 //player 2 switch
const byte led_p1 = 6;                    //player 1 main LED
const byte led_p2 = 7;                    //player 2 main LED
const byte led_score_p1[] = {8, 9, 10};   //player 1 score LEDs
const byte led_score_p2[] = {11, 12, 13}; //player 2 score LEDs

const byte led_avg =  14;                 //avg score LED
const byte led_stop = 15;                 //game stop LED

const byte blink_count = 5;               //initial blinks
const int timebetween = 1000;             //time between games

uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
uint8_t blank[] = {0x00, 0xff, 0x00, 0x00};

void between(int k)
{
  while (k)
  {
    //odejmowanie czasu
    if ((digitalRead(switch1) == LOW) || (digitalRead(switch2) == LOW))
    {
      delay(1);
      k--;
    }
  }
}

void blinkMainLEDs()
{
  for (int i = 0; i < blink_count; i++)
  {
    digitalWrite(led_p1, HIGH);
    digitalWrite(led_p2, HIGH);
    delay(300);

    digitalWrite(led_p1, LOW);
    digitalWrite(led_p2, LOW);
    delay(300);
  }
}

void setup() 
{
  display.clear();
  randomSeed(analogRead(2));

  pinMode(switch_p1, INPUT_PULLUP);
  pinMode(switch_p2, INPUT_PULLUP);

  pinMode(led_p1, OUTPUT);
  pinMode(led_p2, OUTPUT);

  for (int i = 0; i < sizeof(led_score_p1) / sizeof(led_score_p1[0]); i++) 
  {
    pinMode(led_score_p1[i], OUTPUT);
  }

  for (int i = 0; i < sizeof(led_score_p2) / sizeof(led_score_p2[0]); i++) 
  {
    pinMode(led_score_p2[i], OUTPUT);
  }

  pinMode(led_avg, OUTPUT);
  pinMode(led_stop, OUTPUT);

  blinkMainLEDS();
}

void loop()
{
  display.clear();
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  int randNumber = random(3000, 7000);
  int currtime = 0;
  int play = 1;

  for (randNumber; randNumber > 0; randNumber--)
  {
    delay(1);

    if ((digitalRead(switch1) == LOW) || (digitalRead(switch2) == LOW))
    {
      data[3] = display.encodeDigit(15);
      data[2] = display.encodeDigit(15);
      data[1] = display.encodeDigit(15);
      data[0] = display.encodeDigit(15);
      display.setBrightness(0x01);
      display.setSegments(data);

      digitalWrite(led_stop, HIGH);

      play = 0;
      randNumber = 0;
      between(timebetween);
    }
  }

  while (play)
  {
    digitalWrite(led_p1, HIGH);
    digitalWrite(led_p2, HIGH);

    if (digitalRead(switch1) == LOW)
    {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);

      data[3] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[2] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[1] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[0] = display.encodeDigit(currtime % 10);
      display.setBrightness(0x01);
      display.setSegments(data);

      between(timebetween);
      break;
    }

    if (digitalRead(switch2) == LOW)
    {
      digitalWrite(led2, HIGH);
      digitalWrite(led1, LOW);

      data[3] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[2] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[1] = display.encodeDigit(currtime % 10);
      currtime /= 10;
      data[0] = display.encodeDigit(currtime % 10);
      display.setBrightness(0x01);
      display.setSegments(data);

      between(timebetween);
      break;
    }
    if (currtime > 9999)
    {
      data[3] = display.encodeDigit(9);
      data[2] = display.encodeDigit(9);
      data[1] = display.encodeDigit(9);
      data[0] = display.encodeDigit(9);
      display.setSegments(data);
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);

      between(timebetween);
      break;
    }
    delay(1);
    currtime++;
  }
}
