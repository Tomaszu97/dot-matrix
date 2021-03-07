#include <ShiftRegister74HC595.h>
#include <TimerOne.h>

#define DS 2   //DATA
#define SHCP 3 //CLOCK
#define STCP 4 //LATCH
#define R0 5
#define R1 6
#define R2 7
#define R3 8
#define R4 9
#define R5 10
#define R6 11
#define R7 12

ShiftRegister74HC595<2> sr(DS, SHCP, STCP);

uint16_t brightnessBuffer = 700;
uint8_t animationBuffer[100][16];
int animationFrameCount = 3;
int animationCurrentFrame = 0;
int animationFPS = 4;

uint8_t flipByte(uint8_t c)
{
  c = ((c >> 1) & 0x55) | ((c << 1) & 0xAA);
  c = ((c >> 2) & 0x33) | ((c << 2) & 0xCC);
  c = (c >> 4) | (c << 4);
  return c;
}

void setRowValues(uint8_t rowvalueH, uint8_t rowvalueL)
{
  //negate
  rowvalueH = ~rowvalueH;
  rowvalueL = ~rowvalueL;
  //flip over bytes
  rowvalueH = flipByte(rowvalueH);
  rowvalueL = flipByte(rowvalueL);
  //put them in reverse order because displays are swapped on a board
  uint8_t values[] = {rowvalueL, rowvalueH};
  sr.setAll(values);
}

void deactivateAllRows()
{
  digitalWrite(R0, 1);
  digitalWrite(R1, 1);
  digitalWrite(R2, 1);
  digitalWrite(R3, 1);
  digitalWrite(R4, 1);
  digitalWrite(R5, 1);
  digitalWrite(R6, 1);
  digitalWrite(R7, 1);
}

void activateSingleRow(int rowNumber)
{
  deactivateAllRows(); //safety feature
  digitalWrite(rowNumber + 5, 0);
}

void deactivateSingleRow(int rowNumber)
{
  digitalWrite(rowNumber + 5, 1);
}

void singleMultiplex()
{
  if (brightnessBuffer == 0)
  {
    delayMicroseconds(8000);
    return;
  }
  for (int i = 0; i < 8; i++)
  {
    setRowValues(animationBuffer[animationCurrentFrame][2 * (7 - i) + 1], animationBuffer[animationCurrentFrame][2 * (7 - i)]);
    activateSingleRow(i);
    delayMicroseconds(brightnessBuffer);
    deactivateSingleRow(i);
    delayMicroseconds(1000 - brightnessBuffer);
  }
}

void updateFrame()
{
  animationCurrentFrame++;
  if (animationCurrentFrame >= animationFrameCount)
    animationCurrentFrame = 0;
}

void setFPS(int FPS)
{
  Timer1.setPeriod(1 / float(FPS) * 1000000);
  animationFPS = FPS;
}

int getFPS()
{
  return animationFPS;
}

void setBrightness(int brightness)
{
  brightnessBuffer = brightness * 100;
}

void setup()
{
  pinMode(R0, OUTPUT);
  digitalWrite(R0, 1);
  pinMode(R1, OUTPUT);
  digitalWrite(R1, 1);
  pinMode(R2, OUTPUT);
  digitalWrite(R2, 1);
  pinMode(R3, OUTPUT);
  digitalWrite(R3, 1);
  pinMode(R4, OUTPUT);
  digitalWrite(R4, 1);
  pinMode(R5, OUTPUT);
  digitalWrite(R5, 1);
  pinMode(R6, OUTPUT);
  digitalWrite(R6, 1);
  pinMode(R7, OUTPUT);
  digitalWrite(R7, 1);
  sr.setAllHigh(); //leave it like that for safety

  Serial.begin(38400);
  Timer1.initialize(16666); //60FPS
  Timer1.attachInterrupt(updateFrame);
}

void loop()
{
  if (Serial.available())
  {
    Timer1.stop();

    String command = Serial.readStringUntil(':');
    // command.remove(command.indexOf('\r'));
    // command.remove(command.indexOf('\n'));
    // command.remove(command.indexOf('\t'));
    // command.remove(command.indexOf(' '));

    if (command == "FPS")
    {
      int value = Serial.readStringUntil('#').toInt();
      setFPS(value);
    }

    else if (command == "BRIGHTNESS")
    {
      int value = Serial.readStringUntil('#').toInt();
      setBrightness(value);
    }

    else if (command == "DATA")
    {
      int counter = 0;
      char currentChar = '@';
      while (currentChar != '#')
      {
        currentChar = Serial.read();
        if (currentChar == '1')
        {
          animationBuffer[int(counter / 128)][int(counter / 8 % 16)] |= 1 << (counter % 8);
          counter++;
        }
        else if (currentChar == '0')
        {
          animationBuffer[int(counter / 128)][int(counter / 8 % 16)] &= ~(1 << (counter % 8));
          counter++;
        }
      }
      animationFrameCount = int(counter / 128);
    }

    animationCurrentFrame = 0;

    Timer1.resume();
  }

  singleMultiplex();
}