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

void active_delay(int time)
{
  digitalWrite(led_stop, HIGH);
  while (time)
  {
    if ((digitalRead(switch1) == LOW) || (digitalRead(switch2) == LOW))
    {
      delay(1);
      time--;
    }
  }
  digitalWrite(led_stop, LOW);
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

void updateDisplay(int value)
{
  data[3] = display.encodeDigit(value % 10);
  value /= 10;
  data[2] = display.encodeDigit(value % 10);
  value /= 10;
  data[1] = display.encodeDigit(value % 10);
  value /= 10;
  data[0] = display.encodeDigit(value % 10);

  display.setBrightness(0x01);
  display.setSegments(data);
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

  int timeToStart = random(3200, 7200);
  int reactionTime = 0;
  bool stop = 0;

  while(timeToStart)
  {
    delay(1);
    timeToStart--;

    if ((digitalRead(switch1) == LOW) || (digitalRead(switch2) == LOW))
    {
      data[3] = display.encodeDigit(15);
      data[2] = display.encodeDigit(15);
      data[1] = display.encodeDigit(15);
      data[0] = display.encodeDigit(15);
      display.setBrightness(0x01);
      display.setSegments(data);

      digitalWrite(led_stop, HIGH);

      stop = 1;
      timeToStart = 0;
      active_delay(timebetween);
    }
  }

  while (!stop)
  {
    digitalWrite(led_p1, HIGH);
    digitalWrite(led_p2, HIGH);

    if (digitalRead(switch1) == LOW)
    {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);

      updateDisplay(reactionTime);

      active_delay(timebetween);
      break;
    }

    if (digitalRead(switch2) == LOW)
    {
      digitalWrite(led2, HIGH);
      digitalWrite(led1, LOW);

      updateDisplay(reactionTime);

      active_delay(timebetween);
      break;
    }
    if (reactionTime > 9999)
    {
      updateDisplay(9999);

      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);

      active_delay(timebetween);
      break;
    }
    delay(1);
    reactionTime++;
  }
}
