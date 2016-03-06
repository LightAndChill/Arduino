#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Pins
#define pin_LED_Ready 13
#define pin_LED_Blue  11
#define pin_LED_Red   10
#define pin_LED_Green 9

// Colors loop
#define BLUE_PLUS 0
#define RED_MINUS 1
#define GREEN_PLUS 2
#define BLUE_MINUS 3
#define RED_PLUS 4
#define GREEN_MINUS 5

// Modes
#define MUSIC1 1
#define MUSIC2 2
#define RAINBOW 5
#define OFF 10

#define DEBUG false

/////////////// Globales variables ///////////////

// Colors
int red   = 255;
int green = 0;
int blue  = 0;

char state = BLUE_PLUS;
int LED = LOW;
int mode = MUSIC2;

// Vars
int limit = 20;
int step = 1;
int interval = 2;
int value = 0;

BridgeServer server;

/////////////// BASE ///////////////

void setup()
{
  pinMode(pin_LED_Blue,  OUTPUT);
  pinMode(pin_LED_Red,   OUTPUT);
  pinMode(pin_LED_Green, OUTPUT);
  pinMode(pin_LED_Ready, OUTPUT);

  Serial.begin(9600);
  Bridge.begin();

  server.listenOnLocalhost();
  server.begin();

  digitalWrite(pin_LED_Ready, HIGH);
}

// ----------------------------------

void loop()
{
  getLimit();
  
  processCommand();
  processMode();

  delay(interval);
}

/////////////// PROCESS ///////////////

void processCommand()
{
  BridgeClient client = server.accept();

  if (!client)
  {
    return;
  }
  
  String command = client.readStringUntil('/');
  command.trim();

  if (command == "music1")
  {
    mode = MUSIC1;
  }

  if (command == "music2")
  {
    mode = MUSIC2;
  }

  if (command == "rainbow")
  {
    mode = RAINBOW;
  }

  if (command == "off")
  {
    mode = OFF;
  }

  client.stop();
}

// ----------------------------------

void processMode()
{
  switch (mode)
  {
    case MUSIC1:
      music1();
      break;
    case MUSIC2:
      music2();
      break;
    case RAINBOW:
      rainbow();
      break;
    case OFF:
    default:
      off();
      break;
  }
}

/////////////// MODES ///////////////

void music1()
{
  //getMic();
  getSerial();
  
  calculColor();

  if (LED == HIGH)
  {
    setColor(red, green, blue);
  }
  else
  {
    off();
  }
}

// ----------------------------------

void music2()
{
  interval = 50; // TODO: move this shit
  
  if (Serial.available() > 0)
  {
    step = Serial.read() + 1;
    //state = map(value, 0, 255, 0, 5);
  }

  calculColor();
  setColor(red, green, blue);
}

// ----------------------------------

void rainbow()
{
  calculColor();
  setColor(red, green, blue);
}

// ----------------------------------

void off()
{
  setColor(0, 0, 0);
}

/////////////// DATAS ///////////////

void getMic()
{
  int value = analogRead(A0);
  
  if (value > limit)
  {
    LED = HIGH;
  }
  else
  {
    LED = LOW;
  }
}

// ----------------------------------

void getSerial()
{
  if (Serial.available() > 0)
  {
    value = Serial.read();
  }
  
  if (value > limit)
  {
    LED = HIGH;
  }
  else
  {
    LED = LOW;
  }
}

// ----------------------------------

void getLimit()
{
  //limit = analogRead(A1) * 0.04;
  //limit = analogRead(A1) / 4;
  limit = map(analogRead(A1), 0, 1023, -10, 255);
  
  //Serial.println(limit);
}

/////////////// UTILS ///////////////

void setColor(int _red, int _green, int _blue)
{
  if (_red   > 255) _red   = 255;
  if (_green > 255) _green = 255;
  if (_blue  > 255) _blue  = 255;
  
  if (_red   < 0) _red   = 0;
  if (_green < 0) _green = 0;
  if (_blue  < 0) _blue  = 0;
  
  analogWrite(pin_LED_Red,   _red);
  analogWrite(pin_LED_Green, _green);
  analogWrite(pin_LED_Blue,  _blue);

  if (DEBUG)
  {
    Serial.print((int)_red);
    Serial.print(" ");
    Serial.print((int)_green);
    Serial.print(" ");
    Serial.println((int)_blue);
  }
}

// ----------------------------------

void calculColor()
{
  switch (state)
  {
    case BLUE_PLUS:
    {
      blue += step;
      if (blue >= 255)
      {
        blue = 255;
        state = RED_MINUS;
      }
      break;
    }
    case RED_MINUS:
    {
      red -= step;
      if (red <= 0)
      {
        red = 0;
        state = GREEN_PLUS;
      }
      break;
    }
    case GREEN_PLUS:
    {
      green += step;
      if (green >= 255)
      {
        green = 255;
        state = BLUE_MINUS;
      }
      break;
    }
    case BLUE_MINUS:
    {
      blue -= step;
      if (blue <= 0)
      {
        blue = 0;
        state = RED_PLUS;
      }
      break;
    }
    case RED_PLUS:
    {
      red += step;
      if (red >= 255)
      {
        red = 255;
        state = GREEN_MINUS;
      }
      break;
    }
    case GREEN_MINUS:
    {
      green -= step;
      if (green <= 0)
      {
        green = 0;
        state = BLUE_PLUS;
      }
      break;
    }
  }
}
