
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
int past_averaged = 0;

// initialize the current time value
unsigned long current_time = 0;

// intialize the counter for the pulses
float pulse_count;

// initialize the counter for the heart rate and
// calculating the heart rate
int heart_rate;
float calculating_heart_rate;

// initialize the start time counters
unsigned long start_time;
unsigned long derivative_start_time;

// initialize the time counter for the time after a change in 
// sign is detected (-) until the next one
unsigned long time_after_derivative;

// initialize the time counter for the heart rate calculation (milliseconds)
float heart_rate_time_counter = 50;

// initialize age variables for the user input
int age = 0;
boolean age_question_answered;

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

  // light up the 1st NeoPixel with a red color
  // CircuitPlayground.setPixelColor(1, 0xFF0000);

  // light up the 0th NeoPixel with a green color
  // CircuitPlayground.setPixelColor(0, 0x00FF00);

  // initialize the start time at the beginning
  start_time = millis();


 Serial.println("Thank you for using the Pulse at the Palm of Your Hand Heart Rate Monitor!");
 Serial.println("What is your age so that we can calculate how your heart rate is compared to your age group?");
 

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

  if (Serial.available() > 0 && age_question_answered == false) {

    if (age_question_answered == false && age == 0) {

        age = Serial.parseInt();
        age_question_answered = true;
        Serial.print("The age (in years) you inputted is: ");
        Serial.println(age);
        //Serial.println(age_question_answered);
    }
    
  }

  if (age_question_answered == true && age != 0) {
  
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
  // Serial.print("Calculate Heart Rate:");
  // Serial.println(calculating_heart_rate);

  // add the 50 ms of delay that occurs with each loop
  heart_rate_time_counter = heart_rate_time_counter + 50;

  // Serial.print("PulseCount:");
  // Serial.println(pulse_count);

  // Serial.print("HeartRateTimeCounter:");
  // Serial.println(heart_rate_time_counter);

  // Serial.print("Heart Rate:");
  // Serial.println(calculating_heart_rate);

  // Heart Rate Range 40 - 200 BPM
  // Source: https://www.heart.org/en/healthy-living/fitness/fitness-basics/target-heart-rates
  // if the heart rate is not within 40 - 200 BPM, then keep calculating
  
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

  // Notify the user that the heart rate is still being if the heart rate is out of the realistic range
  if (heart_rate_time_counter > 15000 && (calculating_heart_rate < 40 | calculating_heart_rate > 200)) {

    Serial.println("Heart Rate Seems Abnormal...");
    Serial.println("Continuing to Calculate Heart Rate...");
    
  }

  // Display the heart rate if a value has been calculated
  if (heart_rate_time_counter > 15000 && (calculating_heart_rate >= 40 | calculating_heart_rate <= 200)) {

    heart_rate = calculating_heart_rate;
    
    Serial.print("Heart Rate: ");
    Serial.print(heart_rate);
    Serial.println(" BPM");

   // Display how the user's heart rate compares with their age group
  
    if (age < 1) {
      if (heart_rate < 80 && heart_rate > 160) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 80 && heart_rate <= 160) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
  
    if (age >= 1 && age <= 2) {
      if (heart_rate < 80 && heart_rate > 130) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 80 && heart_rate <= 130) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
  
    if (age >= 3 && age <= 4) {
      if (heart_rate < 80 && heart_rate > 120) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 80 && heart_rate <= 120) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
    
    if (age >= 5 && age <= 6) {
      if (heart_rate < 75 && heart_rate > 115) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 75 && heart_rate <= 115) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
  
    if (age >= 7 && age <= 9) {
      if (heart_rate < 75 && heart_rate > 110) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 75 && heart_rate <= 110) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
  
    if (age >= 10 && age <= 15) {
      if (heart_rate < 60 && heart_rate > 100) {
        Serial.println("Your heart's fitness level is considered abnormal for your age group!");
      }
      if (heart_rate >= 60 && heart_rate <= 100) {
        Serial.println("Your heart's fitness level is considered normal for your age group!");
      }
    }
    
    if (age >= 18 && age <= 25) {
      if (heart_rate >= 49 && heart_rate <= 60) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 61 && heart_rate <= 65) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 66 && heart_rate <= 73) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 74 && heart_rate <= 84) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 85) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }
  
    if (age >= 26 && age <= 35) {
      if (heart_rate >= 49 && heart_rate <= 59) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 60 && heart_rate <= 64) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 65 && heart_rate <= 72) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 73 && heart_rate <= 82) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 83) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }
  
    if (age >= 36 && age <= 45) {
      if (heart_rate >= 50 && heart_rate <= 59) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 60 && heart_rate <= 64) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 65 && heart_rate <= 73) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 74 && heart_rate <= 84) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 85) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }
  
    if (age >= 46 && age <= 55) {
      if (heart_rate >= 50 && heart_rate <= 60) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 61 && heart_rate <= 65) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 66 && heart_rate <= 73) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 74 && heart_rate <= 83) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 84) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }
  
    if (age >= 56 && age <= 65) {
      if (heart_rate >= 51 && heart_rate <= 59) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 60 && heart_rate <= 64) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 65 && heart_rate <= 73) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 74 && heart_rate <= 83) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 84) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }
  
    if (age > 65) {
      if (heart_rate >= 50 && heart_rate <= 59) {
        Serial.println("Your heart's fitness level is considered athletic for your age group!");
      }
      if (heart_rate >= 60 && heart_rate <= 64) {
        Serial.println("Your heart's fitness level is considered excellent for your age group!");
      }
      if (heart_rate >= 65 && heart_rate <= 72) {
        Serial.println("Your heart's fitness level is considered good for your age group!");
      }
      if (heart_rate >= 73 && heart_rate <= 83) {
        Serial.println("Your heart's fitness level is considered average for your age group!");
      }
      if (heart_rate >= 84) {
        Serial.println("Your heart's fitness level is considered below average for your age group!");
      }
    }

    
  }

 

  // delay the loop for 50 milliseconds
  delay(50);

  // light up the 7th NeoPixel with a black color (off)
  // to turn off the pulse LED when there is no pulse
  CircuitPlayground.setPixelColor(7, 0x000000);

  }

}
