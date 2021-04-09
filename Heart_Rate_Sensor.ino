// Currently in the process of troubleshooting the code to get it
// to stop breaking the circuit playground

/*
 Note: When the code breaks the Circuit Playground,
 here are the steps to unbreak it:
 1) Unplug the Circuit Playground from your computer
 2) Close the Arduino IDE
 3) Plug in the Circuit Playground to your computer
 4) Open the Arduino IDE
 5) Open the Circuit Playground Example code in
    File > Examples > Examples from Custom Libraries
    > Arduino Circuit Playground > Hello_CircuitPlayground
    > Hello_LightSensor
 6) Compile the Hello_LightSensor Code
 7) Double press the reset button on the Circuit Playground
    to activate the "Bootloader" mode
 8) Quickly upload the Hello_LightSensor Code
 9) If the CircuitPlayground malfunctions, repeat
    steps 7 and 8 until the code successfully uploads
*/

//LIBRARIES--------------------------------------------------

// include the Adafruit Circuit Playground library
#include <Adafruit_CircuitPlayground.h>

// define how many readings will be used in the moving average
// filter
#define num_readings 12

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
int past_averaged = 0;

// initialize the current time value
unsigned long current_time = 0;

// intialize the counter for the pulses
int pulse_count;

// initialize the counter for the heart rate and
// calculating the heart rate
int heart_rate;
int calculating_heart_rate;

// initialize the start time counters
unsigned long start_time;
unsigned long derivative_start_time;

// initialize the time counter for the time after a change in 
// sign is detected (-) until the next one
unsigned long time_after_derivative;

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

  // initialize the start time at the beginning
  start_time = millis();

}

//FUNCTIONS--------------------------------------------------

//LOOP-------------------------------------------------------
void loop() {

  // read the value from the light sensor on the Adafruit
  // Circuit Playground
  light_sensor_value = CircuitPlayground.lightSensor();

  // print the light sensor value on the serial monitor
  // and have it display as a variable on the serial plotter
  // Serial.print("LightSensorValue:");
  // Serial.print(light_sensor_value);
  // Serial.print(",");

  // Moving Average Filter ---------------------------------

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
  // Serial.print("LightSensorAverageValue:");
  // Serial.println(averaged);

  // Derivative -------------------------------------------
  unsigned long past_time = current_time;
  // current_time = millis();

  // Serial.print("CurrentTime:");
  // Serial.println(current_time);

  unsigned long time_interval = current_time - past_time;

  // Serial.print("TimeInterval:");
  // Serial.println(time_interval);
  
  int derivative = (averaged - past_averaged)/time_interval;
  past_averaged = averaged;

  // Serial.print("PastAveraged:");
  // Serial.println(past_averaged);

  // print the derivative of the averaged light sensor value 
  // over time on the serial monitor and have it display as 
  // a variable on the serial plotter
   Serial.print("DerivativeLightSensorAverageValue:");
   Serial.println(derivative);

  // if the derivative is 
  if (derivative > 0) {
    
    // Serial.println("Increase");
    
  }
  if (derivative < 0) {
    
    // Serial.println("Decrease");
    
  }

  // Serial.print("StartTime:");
  // Serial.println(start_time);
  // Serial.print("CurrentTime:");
  // Serial.println(millis());
  unsigned long interval_15_seconds = millis() - start_time;
  // Serial.print("Interval15Seconds:");
  // Serial.println(interval_15_seconds);
  // Serial.print("TimeAfterDerivative:");
  // Serial.println(time_after_derivative);

  // check if the derivative is negative and if 
  // the time after the derivative is greater than
  // a threshold to ensure that multiple negative 
  // derivatives occurring very close together
  // for the same pulse signal will be read as just 
  // one pulse
  if (derivative < 0 && time_after_derivative > 500) {
    
    derivative_start_time = millis();
    pulse_count = pulse_count + 1;
    // Serial.print("PulseCount:");
    // Serial.println(pulse_count);
    
    // light up the 7th NeoPixel with a red color
    // to show the pulse visually using LED flashes
    CircuitPlayground.setPixelColor(7, 0xFF0000);
    // delay the loop for 50 milliseconds
    delay(25);  
    
  }

/*
  // Note: Looks like this portion of the code breaks
  // the Circuit Playground :0
  // update the time after derivative variable
  time_after_derivative = millis() - derivative_start_time;

*/

  // calculate the heart rate from the pulse counter
  // where the pulse counter counts the number of pulses
  // for 15 seconds, and the heart rate (in beats per minute)
  // is the number of pulses*4 (since there are 60 seconds in
  // one minute, and 60 seconds divided by 15 seconds is 
  // 4 intervals)
  if (interval_15_seconds < 15000) {
    
    calculating_heart_rate = pulse_count*4;
    // Serial.print("CalculatingHeartRate:");
    // Serial.println(calculating_heart_rate);
    
  }

  // if it has passed 15 seconds of counting the pulses
  // then reinitialize the counters to count the pulses
  // for another 15 seconds
  if (interval_15_seconds > 15000) {
    
    // reinitialize the counter for the pulses,  
    // 15 second interval, and start time
    pulse_count = 0;
    interval_15_seconds = 0;
    start_time = millis();
    
    // set the heart rate
    heart_rate = calculating_heart_rate;
  }

  // Notify the user that the heart rate is still being
  // calculated
  if (heart_rate == 0) {
    
    Serial.println("Calculating Heart Rate...");
   
  }

  // Display the heart rate if a value has been calculated
  if (heart_rate != 0) {
    
    Serial.print("HeartRate:");
    Serial.println(heart_rate);
    
  }


  // delay the loop for 50 milliseconds
  delay(50);

  // light up the 7th NeoPixel with a white color
  // Note: Is there a way to make just the one NeoPixel turn off instead?
  CircuitPlayground.setPixelColor(7, 0xFFFFFF);
  
}
