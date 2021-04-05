//LIBRARIES--------------------------------------------------
#include <Adafruit_CircuitPlayground.h>

#define window_size 5

//VARIABLES--------------------------------------------------

// set up a variable for the light sensor measurement
int light_sensor_value;

int index = 0;
int value = 0;
int sum = 0;
int readings[window_size];
int averaged = 0;

int threshold = 50;

//SETUP------------------------------------------------------
void setup() {

  // initialize the serial connection at 9600 bits per second
  Serial.begin(9600);

  // initialize the circuit playground library
  CircuitPlayground.begin();

  // set the brightness of the circuit playground NeoPixels to the max (255)
  CircuitPlayground.setBrightness(255);

  // light up the 0th NeoPixel with a red color
  CircuitPlayground.setPixelColor(0, 0xFF0000);


  // light up the 0th NeoPixel with a green color
  //CircuitPlayground.setPixelColor(0, 0x00FF00);

}

//FUNCTIONS--------------------------------------------------

//LOOP-------------------------------------------------------
void loop() {

  light_sensor_value = CircuitPlayground.lightSensor();
  Serial.print("LightSensorValue:");
  Serial.print(light_sensor_value);
  Serial.print(",");

  sum = sum - readings[index];
  readings[index] = light_sensor_value;
  sum = sum + light_sensor_value;
  index = (index + 1)%window_size;

  averaged = sum/window_size;

//  Serial.print("LightSensorValueAverage: ");
  Serial.print("LightSensorAverageValue:");
  Serial.println(averaged);

  delay(50);
  
}
