#include <TM1637Display.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);
uint8_t data[] = {0xff, 0xff, 0xff, 0xff};

const byte led_p2 = 4;                    //player 2 main LED
const byte switch_p2 = 5;                 //player 2 switch
const byte led_p1 = 6;                    //player 1 main LED
const byte switch_p1 = 7;                 //player 1 switch

const byte led_score_p1[] = {8, 9, 10};   //player 1 score LEDs
const byte led_score_p2[] = {11, 12, 13}; //player 2 score LEDs

const byte led_avg =  14;                 //avg score LED
const byte led_stop = 15;                 //game stop LED
const byte multiplayer_switch = 16;       //multiplayer if HIGH

const byte MAX_SCORE = 3;
const byte blink_count = 5;               //initial blinks
const int timebetween = 1000;             //time between games

byte multiplayer = 1;
unsigned long p1Total = 0;
unsigned long p2Total = 0;
byte scoreP1 = 0;
byte scoreP2 = 0;

void active_delay(int time)
{
  digitalWrite(led_stop, HIGH);
  while (time)
  {
    if ((digitalRead(switch_p1) == LOW) || (digitalRead(switch_p2) == LOW))
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

void updateDisplay(unsigned long value, int scoreP1, int scoreP2)
{
  for (int i = 0; i < MAX_SCORE; i++)
  {
    digitalWrite(led_score_p1[i], LOW);
    digitalWrite(led_score_p2[i], LOW);
  }

  for (int i = 0; i < scoreP1; i++)
  {
    digitalWrite(led_score_p1[i], HIGH);
  }

  for (int i = 0; i < scoreP2; i++)
  {
    digitalWrite(led_score_p2[i], HIGH);
  }

  if(value == 0)
  {
    display.clear();
    return;
  }

  value /= 1000;
  
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

void displayAverage(unsigned long p1Total, unsigned long p2Total, byte scoreP1, byte scoreP2)
{
  digitalWrite(led_avg, HIGH);

  if(multiplayer == 1)
  {
    //p1 average
    if(p1Total > 0)
    {
      digitalWrite(led_p1, HIGH);
      digitalWrite(led_p2, LOW);
      updateDisplay(p1Total/(scoreP1 + scoreP2), scoreP1, scoreP2);

      active_delay(timebetween);
    }

    //p2 average
    if(p2Total > 0)
    {
      digitalWrite(led_p1, LOW);
      digitalWrite(led_p2, HIGH);
      updateDisplay(p2Total/(scoreP1 + scoreP2), scoreP1, scoreP2);

      active_delay(timebetween);
    }
  }
  else //singleplayer
  {
    //p1 average
    if(p1Total > 0)
    {
      digitalWrite(led_p1, HIGH);
      digitalWrite(led_p2, LOW);
      updateDisplay(p1Total/(scoreP1), scoreP1, scoreP2);

      active_delay(timebetween);
    }

    //p2 average
    if(p2Total > 0)
    {
      digitalWrite(led_p1, LOW);
      digitalWrite(led_p2, HIGH);
      updateDisplay(p2Total/(scoreP2), scoreP1, scoreP2);

      active_delay(timebetween);
    }
  }

  digitalWrite(led_avg, LOW);
}

bool falseStart()
{
  unsigned long waitTime = random(3200, 7200) * 1000UL;
  unsigned long startTime = micros();

  while (micros() - startTime < waitTime)
  {
    if ((digitalRead(switch_p1) == LOW) || (digitalRead(switch_p2) == LOW))
    {
      data[3] = display.encodeDigit(15);
      data[2] = display.encodeDigit(15);
      data[1] = display.encodeDigit(15);
      data[0] = display.encodeDigit(15);
      display.setBrightness(0x01);
      display.setSegments(data);

      digitalWrite(led_stop, HIGH);

      active_delay(timebetween);
      return 1;
    }
  }

  return 0;
}

void reflexTest()
{
  bool finished_players[2]={0, 0};
  digitalWrite(led_p1, HIGH);
  digitalWrite(led_p2, HIGH);

  unsigned long startTime = micros();
  unsigned long reactionTime = 0;

  while(finished_players[0] == 0 || finished_players[1] == 0)
  {
    //calculate time after falsestart()
    reactionTime = micros() - startTime;

    //player 1
    if (digitalRead(switch_p1) == LOW && finished_players[0] == 0)
    {
      if(finished_players[1] == 0)
      {
        digitalWrite(led_p1, HIGH);
        digitalWrite(led_p2, LOW);
        scoreP1++;
        updateDisplay(reactionTime, scoreP1, scoreP2);
      }

      p1Total += reactionTime;
      finished_players[0] = 1;

      if(multiplayer == 0)
        return;
    }

    //player 2
    if (digitalRead(switch_p2) == LOW && finished_players[1] == 0)
    {
      if(finished_players[0] == 0)
      {
        digitalWrite(led_p1, LOW);
        digitalWrite(led_p2, HIGH);
        scoreP2++;
        updateDisplay(reactionTime, scoreP1, scoreP2);
      }

      p2Total += reactionTime;
      finished_players[1] = 1;

      if(multiplayer == 0)
        return;
    }

    if (reactionTime > 9990000)
    {
      digitalWrite(led_p1, LOW);
      digitalWrite(led_p2, LOW);

      //both players didnt finish - doesnt count 
      if((finished_players[0] == 0) && (finished_players[1] == 0))
      {
        updateDisplay(9999000, scoreP1, scoreP2);
        return;
      }

      if(finished_players[0] == 0)
        p1Total += reactionTime;

      if(finished_players[1] == 0)
        p2Total += reactionTime;

      return;
    }
  }
}

void handleGame()
{
  display.clear();
  digitalWrite(led_p1, LOW);
  digitalWrite(led_p2, LOW);

  if (falseStart() == 0)
    reflexTest();

  active_delay(timebetween);
}

void setup() 
{
  randomSeed(analogRead(3));

  pinMode(switch_p1, INPUT_PULLUP);
  pinMode(switch_p2, INPUT_PULLUP);

  pinMode(led_p1, OUTPUT);
  pinMode(led_p2, OUTPUT);

  for (int i = 0; i < MAX_SCORE; i++) 
  {
    pinMode(led_score_p1[i], OUTPUT);
    pinMode(led_score_p2[i], OUTPUT);
  }

  pinMode(led_avg, OUTPUT);
  pinMode(led_stop, OUTPUT);
  pinMode(multiplayer_switch, INPUT_PULLUP);

  blinkMainLEDs();
}

void loop()
{
  p1Total = 0;
  p2Total = 0;
  scoreP1 = 0;
  scoreP2 = 0;
  updateDisplay(0, scoreP1, scoreP2);

  multiplayer = digitalRead(multiplayer_switch);

  while(scoreP1 < MAX_SCORE && scoreP2 < MAX_SCORE)
  {
    handleGame();
  }

  displayAverage(p1Total, p2Total, scoreP1, scoreP2);
}
