#include <SparkFunTSL2561.h>
#include <Wire.h>
#include <StepperMotor.h>

StepperMotor motor(8,9,10,11);

// Create an SFE_TSL2561 object, here called "light":

SFE_TSL2561 light;

// Global variables:

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
double spot[12];

void setup()
{
  // Initialize the Serial port:
  
  Serial.begin(9600);
  motor.setStepDuration(1);
  Serial.println("TSL2561 example sketch");

  // Initialize the SFE_TSL2561 library

  // You can pass nothing to light.begin() for the default I2C address (0x39),
  // or use one of the following presets if you have changed
  // the ADDR jumper on the board:
  
  // TSL2561_ADDR_0 address with '0' shorted on board (0x29)
  // TSL2561_ADDR   default address (0x39)
  // TSL2561_ADDR_1 address with '1' shorted on board (0x49)

  // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

  light.begin();

  // Get factory ID from sensor:
  // (Just for fun, you don't need to do this to operate the sensor)

  unsigned char ID;
  
  if (light.getID(ID))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID,HEX);
    Serial.println(", should be 0X5X");
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
    byte error = light.getError();
    printError(error);
  }
  gain = 0;

  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):
  
  Serial.println("Set timing...");
  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:
  
  Serial.println("Powerup...");
  light.setPowerUp();
  
  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.
}

void loop()
{


  delay(ms);
  unsigned int data0, data1;
  int bestSpot = 0;
  for(int i =0; i <12; i++){
    if (light.getData(data0,data1))
    { 
      Serial.print("data0: ");
      Serial.print(data0);
      Serial.print(" data1: ");
      Serial.print(data1);    
      double lux;    // Resulting lux value
      boolean good;  // True if neither sensor is saturated
      good = light.getLux(gain,ms,data0,data1,lux);
      Serial.print(" lux: ");
      Serial.print(lux);
      if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
      motor.step(400);
      spot[i] = lux;
      if(spot[i] >spot [bestSpot]){
        bestSpot = i;
      }
    }
    else
    {
      // getData() returned false because of an I2C error, inform the user.
  
      byte error = light.getError();
      printError(error);
      i--;
    }
  }
  for(int i =0; i < 12-bestSpot;i++){
    motor.step(-400);
  }
  delay(10000);
  for(int i =0; i < bestSpot;i++){
    motor.step(-400);
  }

}

void printError(byte error)

{
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}
