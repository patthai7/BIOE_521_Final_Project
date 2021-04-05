//LIBRARIES--------------------------------------------------

// include the Adafruit Circuit Playground library
#include <Adafruit_CircuitPlayground.h>

// define how many readings will be used in the moving average
// filter
#define num_readings 5

//VARIABLES--------------------------------------------------

// set up a variable for the light sensor measurement
int light_sensor_value;

// initialize the index, value, and sum for the moving 
// average filter
int index = 0;
int value = 0;
int sum = 0;

// initialize an array that holds the reading values and
// is the size of the number of readings for the moving 
// average filter
int readings[num_readings];

// initialize the averaged value for the specified number 
// of measurements for the moving average filter
int averaged = 0;

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

  // read the value from the light sensor on the Adafruit
  // Circuit Playground
  light_sensor_value = CircuitPlayground.lightSensor();

  // print the light sensor value on the serial monitor
  // and have it display as a variable on the serial plotter
  Serial.print("LightSensorValue:");
  Serial.print(light_sensor_value);
  Serial.print(",");

  // Moving Average Filter

  // for each index, remove the oldest reading value to make 
  // room for a new reading value
  sum = sum - readings[index];

  // set the readings vector at the specified index to the
  // light sensor value
  readings[index] = light_sensor_value;

  // add the newest light sensor value to the sum of the
  // readings
  sum = sum + light_sensor_value;

  // go through the specified index values until it reaches 
  // the maximum index value, then go back to the first index
  // value of 0
  index = (index + 1)%num_readings;

  // take the average of the recorded readings
  averaged = sum/num_readings;

  // print the averaged light sensor value on the serial monitor
  // and have it display as a variable on the serial plotter
  Serial.print("LightSensorAverageValue:");
  Serial.println(averaged);

  // delay the loop for 50 milliseconds
  delay(50);
  
}
