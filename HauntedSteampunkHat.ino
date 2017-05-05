/*
Haunted Steampunk Hat - lifts the lid and eyes peek out

Normal processing lights the eyes, blinking them periodically.

The momentary touch switch activates an interrupt, setting
a boolean which is tested in the main loop.  When the boolean
is set, it activates a routine to extinguish the eye lights,
light the head lamp, raise the hat lid of the hat exposing
the googly eyes, lower the lid, extinguish the head lamp,
and re-enable the interrupt.

*/
 
#include <Servo.h>
#include <EnableInterrupt.h>

int headLight = 3; 
int leftGoggle = 5;  
int rightGoggle = 6;
int capSwitchPin = 7;
int servoPin = 9;

int headLightBrightness = 0;
bool peekRequest = false;

#define LIGHT_RAMP_DELAY    4   //  adjust this delay to change the speed the headLight lights and extinguishes
#define SERVO_RAMP_DELAY    20  //  adjust this delay to change the speed with which the hat lid is raised or lowered
#define EYES_OPEN_DELAY   3000  //  adjust this delay to change how long the had lid is in the raised position
#define GOGGLES_LIGHT_RAMP_DELAY 5   //  adjust this delay to change the time it takes for the goggle lights ramp up or down
#define GOGGLES_LIGHT_ON_MAX  150    //  number of loops the goggle lights stay on
#define GOGGLES_LIGHT_OFF_MAX  5     //  number of loops the goggle lights stay off (when "blinking")
#define GOGGLES_LIGHT_DELAY 30       //  loop delay when goggle lights are on or off

enum     gogglesLightState_t {gogglesLightOff = 0, gogglesLightIncreasing = 1, gogglesLightOn = 2, gogglesLightDecreasing = 3};
gogglesLightState_t gogglesLightState = gogglesLightOff;
int gogglesLightDelayCount = 0; 
 
Servo servo;  
 
int servoAngle = 0;   // servo position in degrees

void rampLight (int upDown);
void goggleLightsOut (void);

 void interruptFunction() {
  disableInterrupt(capSwitchPin);
  peekRequest = true;
}

void setup()
{
  Serial.begin(9600);  
  servo.attach(servoPin);
  servo.write(0);              
  pinMode(headLight, OUTPUT);
  pinMode(leftGoggle, OUTPUT);
  pinMode(rightGoggle, OUTPUT);
  pinMode(capSwitchPin, INPUT);
  enableInterrupt(capSwitchPin, interruptFunction, HIGH);
}
 
 
void loop()
{
//control the servo's direction and the position of the motor
switch (gogglesLightState) {
  case gogglesLightOff:
    gogglesLightDelayCount++;
    analogWrite(rightGoggle, 0);
    analogWrite(leftGoggle, 0);
    delay(GOGGLES_LIGHT_DELAY);
    if (gogglesLightDelayCount > GOGGLES_LIGHT_OFF_MAX) {
      gogglesLightState = gogglesLightIncreasing;
    }
    break;
  case gogglesLightIncreasing:
    for (int ndx = 0; ndx < 255; ndx++) {
      delay(GOGGLES_LIGHT_RAMP_DELAY);
      analogWrite(rightGoggle, ndx);
      analogWrite(leftGoggle, ndx);
    }
    gogglesLightDelayCount = 0;
    gogglesLightState = gogglesLightOn;
    break;
  case gogglesLightOn:
    gogglesLightDelayCount++;
    analogWrite(rightGoggle, 255);
    analogWrite(leftGoggle, 255);
    delay(GOGGLES_LIGHT_DELAY);
    if (gogglesLightDelayCount > GOGGLES_LIGHT_ON_MAX) {
      gogglesLightState = gogglesLightDecreasing;
    }
    break;
  case gogglesLightDecreasing:
      goggleLightsOut();
    break;
    default:
    break;
  }
  if (peekRequest) {
    switch (gogglesLightState) {
      case gogglesLightOff:
        gogglesLightDelayCount = 0;
      break;
      case gogglesLightIncreasing:
      case gogglesLightOn:
      case gogglesLightDecreasing:
        goggleLightsOut();
      break;
      default:
      break;
    }
    rampLight (1);
 
    for(servoAngle = 0; servoAngle < 180; servoAngle++)  //move the micro servo from 0 degrees to 180 degrees
    {                                  
      servo.write(servoAngle);              
      delay(SERVO_RAMP_DELAY);                  
    }
    delay(EYES_OPEN_DELAY);
    for(servoAngle = 180; servoAngle > 0; servoAngle--)  //now move back the micro servo from 0 degrees to 180 degrees
    {                                
      servo.write(servoAngle);          
      delay(SERVO_RAMP_DELAY);      
    }
    rampLight (-1);
    peekRequest = false;
    enableInterrupt(capSwitchPin, interruptFunction, HIGH);
  }
  //end control the servo's speed  
}

void rampLight (int upDown) {
  for (int ndx = 0; ndx < 255; ndx++) {
    headLightBrightness += upDown;
    delay(LIGHT_RAMP_DELAY);
    analogWrite(headLight, headLightBrightness);
  }
}

void goggleLightsOut (void) {
  for (int ndx = 255; ndx > 0; ndx--) {
    delay(GOGGLES_LIGHT_RAMP_DELAY);
    analogWrite(rightGoggle, ndx);
    analogWrite(leftGoggle, ndx);
  }
  analogWrite(rightGoggle, 0);
  analogWrite(leftGoggle, 0);
  gogglesLightDelayCount = 0;
  gogglesLightState = gogglesLightOff;
}


