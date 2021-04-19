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
float pulse_count;

// initialize the counter for the heart rate and
// calculating the heart rate
int heart_rate;
float calculating_heart_rate;
float averaged_calculating_heart_rate;

// initialize the start time counters
unsigned long start_time;
unsigned long derivative_start_time;

// initialize the time counter for the time after a change in 
// sign is detected (-) until the next one
unsigned long time_after_derivative;

// initialize the time counter for the heart rate calculation (milliseconds)
float heart_rate_time_counter = 50;

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
  // CircuitPlayground.setPixelColor(0, 0x00FF00);

  // initialize the start time at the beginning
  start_time = millis();

}

//FUNCTIONS--------------------------------------------------

int moving_average(int measurement_value) {

  // for each index, remove the oldest reading value to make 
  // room for a new reading value
  sum = sum - readings[index];

  // set the readings vector at the specified index to the
  // light sensor value
  readings[index] = measurement_value;

  // Serial.print("Readings:");
  // Serial.print(readings[index]);

  // add the newest measurement value to the sum of the
  // readings
  sum = sum + measurement_value;

  // Serial.print("Sum:");
  // Serial.print(sum);

  // go through the specified index values until it reaches 
  // the maximum index value, then go back to the first index
  // value of 0
  index = (index + 1)%num_readings;

  // Serial.print("Index:");
  // Serial.print(index);

  // take the average of the recorded readings
  averaged = sum/num_readings;

  return averaged;
  
}


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

  // Light Sensor Value Moving Average Filter ---------------------------------

  averaged = moving_average(light_sensor_value);

  // print the averaged light sensor value on the serial monitor
  // and have it display as a variable on the serial plotter
  // Serial.print("LightSensorAverageValue:");
  // Serial.println(averaged);

  // Derivative -------------------------------------------
  unsigned long past_time = current_time;
   current_time = millis();

   //Serial.print("CurrentTime:");
   //Serial.println(current_time);

  unsigned long time_interval = current_time - past_time;

   //Serial.print("TimeInterval:");
   //Serial.println(time_interval);
  
  // int derivative = (averaged - past_averaged)/time_interval;
  int derivative = (averaged - past_averaged)/time_interval;
  past_averaged = averaged;

  // Serial.print("PastAveraged:");
  // Serial.println(past_averaged);

  // print the derivative of the averaged light sensor value 
  // over time on the serial monitor and have it display as 
  // a variable on the serial plotter
  // Serial.print("DerivativeLightSensorAverageValue:");
  // Serial.println(derivative);

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
  unsigned long interval_5_seconds = millis() - start_time;
  // Serial.print("Interval5Seconds:");
  // Serial.println(interval_5_seconds);
  // Serial.print("TimeAfterDerivative:");
  // Serial.println(time_after_derivative);

  // check if the derivative is +/- and if 
  // the time after the derivative is greater than
  // a threshold to ensure that multiple +/- 
  // derivatives occurring very close together
  // for the same pulse signal will be read as just 
  // one pulse
  if (derivative != 0 && time_after_derivative > 500) {
    
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

  // update the time after derivative variable 
   time_after_derivative = millis() - derivative_start_time;

  // calculate the heart rate for the given loop by dividing the
  // pulses by the time elapsed (milliseconds) and multiply by
  // 1000*60 to convert to heart beats per minute
  calculating_heart_rate = pulse_count/heart_rate_time_counter;

  // Serial.print("Heart Rate:");
  // Serial.println(heart_rate);
  
  calculating_heart_rate = calculating_heart_rate*1000*60;
  Serial.print("Calculate Heart Rate:");
  Serial.println(calculating_heart_rate);

  averaged_calculating_heart_rate = moving_average(calculating_heart_rate);
  Serial.print("Average Calculate Heart Rate:");
  Serial.println(averaged_calculating_heart_rate);

  // add the 50 ms of delay that occurs with each loop
  heart_rate_time_counter = heart_rate_time_counter + 50;

  // Serial.print("PulseCount:");
  // Serial.println(pulse_count);

  // Serial.print("HeartRateTimeCounter:");
  // Serial.println(heart_rate_time_counter);

  // Serial.print("Heart Rate:");
  // Serial.println(calculating_heart_rate);
  
  // Notify the user that the heart rate is still being
  // calculated (15 seconds)
  if (heart_rate_time_counter < 15000) {
    
    Serial.println("Calculating Heart Rate...");

    // time left to wait for the calculation as an integer 
    // in seconds (for a 15 second calculation
    int calculation_time_left = 15 - heart_rate_time_counter/1000;
    Serial.print(calculation_time_left);
    Serial.println(" Seconds Remaining...");
   
  }

  // Display the heart rate if a value has been calculated
  if (heart_rate_time_counter > 15000) {

    // turn the heart rate calculation float value into an int
    // heart_rate = averaged_calculating_heart_rate;
    heart_rate = calculating_heart_rate;
    
    Serial.print("Heart Rate: ");
    Serial.print(heart_rate);
    Serial.println(" BPM");
    
  }

  // delay the loop for 50 milliseconds
  delay(50);

  // light up the 7th NeoPixel with a black color (off)
  // to turn off the pulse LED when there is no pulse
  CircuitPlayground.setPixelColor(7, 0x000000);
  
}
