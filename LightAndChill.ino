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
#define RAINBOW 1
#define OFF 2

// Globales variables
char state = BLUE_PLUS;
int r = 255;
int g = 0;
int b = 0;
int LED = LOW;
int mode = RAINBOW;
int limit = 20;
int limitOld = 20;
BridgeServer server;

void setup()
{
  pinMode(pin_LED_Blue,  OUTPUT);
  pinMode(pin_LED_Red,   OUTPUT);
  pinMode(pin_LED_Green, OUTPUT);
  pinMode(pin_LED_Ready, OUTPUT);

  //Serial.begin(9600);
  Bridge.begin();

  server.listenOnLocalhost();
  server.begin();

  digitalWrite(pin_LED_Ready, HIGH);
}

void loop()
{
  BridgeClient client = server.accept();

  if (client)
  {
    processCommand(client);
    client.stop();
  }

  limit = analogRead(A1) * 0.04;

  if (limitOld != limit)
  {
    //Serial.println(limit);
    limitOld = limit;
  }
  
  processMode();

  delay(2);
}

void processCommand(BridgeClient client)
{
  String command = client.readStringUntil('/');
  command.trim();

  if (command == "rainbow")
  {
    mode = RAINBOW;
  }

  if (command == "off")
  {
    mode = OFF;
  }
}

void processMode()
{
  switch (mode)
  {
    case RAINBOW:
      rainbow();
      break;
    case OFF:
    default:
      off();
      break;
  }
}

void rainbow()
{
  getMic();
  calculColor();

  if (LED == HIGH)
  {
    
    setColor(r, g, b);
  }
  else
  {
    setColor(0, 0, 0);
  }
}

void off()
{
  setColor(0, 0, 0);
}

void getMic()
{
  int valueMic = analogRead(A0);
  
  if (valueMic > limit)
  {
    LED = HIGH;
  }
  else
  {
    LED = LOW;
  }
}

void setColor(char red, char green, char blue)
{
  analogWrite(pin_LED_Red,   red);
  analogWrite(pin_LED_Green, green);
  analogWrite(pin_LED_Blue,  blue);
  
  /*Serial.print((int)r);
  Serial.print(" ");
  Serial.print((int)g);
  Serial.print(" ");
  Serial.println((int)b);*/
}

void calculColor()
{
  switch (state)
  {
    case BLUE_PLUS:
    {
      b++;
      if (b == 255)
      {
        state = RED_MINUS;
      }
      break;
    }
    case RED_MINUS:
    {
      r--;
      if (r == 0)
      {
        state = GREEN_PLUS;
      }
      break;
    }
    case GREEN_PLUS:
    {
      g++;
      if (g == 255)
      {
        state = BLUE_MINUS;
      }
      break;
    }
    case BLUE_MINUS:
    {
      b--;
      if (b == 0)
      {
        state = RED_PLUS;
      }
      break;
    }
    case RED_PLUS:
    {
      r++;
      if (r == 255)
      {
        state = GREEN_MINUS;
      }
      break;
    }
    case GREEN_MINUS:
    {
      g--;
      if (g == 0)
      {
        state = BLUE_PLUS;
      }
      break;
    }
  }
}
