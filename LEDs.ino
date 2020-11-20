
/*  
 This command controls the Power LED by two data bytes: color and intensity.
 color: Green = 0; 128 = Orange; red=255 
 intensity low=0; max=255 */
void setPowerLED(byte setColor, byte setIntensity)
{
  color = setColor;
  intensity = setIntensity;
  Serial.write(139);
  Serial.write((byte)0x00);
  Serial.write((byte)color);
  Serial.write((byte)intensity);
}

//---------------------------------------------------------------
void setDebrisLED(bool enable)
{
  debrisLED = enable;
  Serial.write(139);
  Serial.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Serial.write((byte)color);
  Serial.write((byte)intensity);
}

//---------------------------------------------------------------
void setSpotLED(bool enable)
{
  spotLED = enable;
  Serial.write(139);
  Serial.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Serial.write((byte)color);
  Serial.write((byte)intensity);
}

//---------------------------------------------------------------
void setDockLED(bool enable)
{
  dockLED = enable;
  Serial.write(139);
  Serial.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Serial.write((byte)color);
  Serial.write((byte)intensity);
}

//---------------------------------------------------------------
void setWarningLED(bool enable)
{
  warningLED = enable;
  Serial.write(139);
  Serial.write((debrisLED ? 1 : 0) + (spotLED ? 2 : 0) + (dockLED ? 4 : 0) + (warningLED ? 8 : 0));
  Serial.write((byte)color);
  Serial.write((byte)intensity);
}

/*--------------------------------------------------------------------------
This command controls the four 7 segment displays on the Roomba.*/
void setDigitLEDs(byte digit1, byte digit2, byte digit3, byte digit4)
{
    Serial.write(163);
    Serial.write(digit1);
    Serial.write(digit2);
    Serial.write(digit3);
    Serial.write(digit4);
}

/*--------------------------------------------------------------------------
This command controls the four 7 segment displays on the Roomba using ASCII character codes.*/
void setDigitLEDFromASCII(byte digit, char letter)
{
  switch (digit){
  case 1:
    digit1 = letter;
    break;
  case 2:
    digit2 = letter;
    break;
  case 3:
    digit3 = letter;
    break;
  case 4:
    digit4 = letter;
    break;
  }
  Serial.write(164);
  Serial.write(digit1);
  Serial.write(digit2);
  Serial.write(digit3);
  Serial.write(digit4);
}

//---------------------------------------------------------------
void cleanDigitLED (void)
{
  setDigitLEDFromASCII(1, ' ');
  setDigitLEDFromASCII(2, ' ');
  setDigitLEDFromASCII(3, ' ');
  setDigitLEDFromASCII(4, ' ');
}
//---------------------------------------------------------------
void writeLEDs (char a, char b, char c, char d)
{
  setDigitLEDFromASCII(1, a);
  setDigitLEDFromASCII(2, b);
  setDigitLEDFromASCII(3, c);
  setDigitLEDFromASCII(4, d);
}
