#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
//-----------------------SUBMISSION DESCRIPTION---------------------
/*
Group Number: I have emailed the professor about separating from my group.
My original group number was number 8.

Names of your teammates: Neha Ghosh

Net IDs: ng2525

Video link: https://youtube.com/shorts/e7U1joTkaHY?feature=share
*/
//---------------------GESTURE NUMBER--------------------------------



int gesture_num = -1;

//--------------SAMPLE TIME, NUMBER OF READINGS FOR ARRAY---------------------------

#define SAMPLE_TIME 100 // Time interval between readings in milliseconds
#define NUM_READINGS 20 // Number of readings for each axis

//--------------OSCILLATION MACROS AND VARIABLES---------------------------
//For oscillation, each direction must have a minimum and maximum acceleration
//to pass for it to count as an oscillation
//these values are the min and max for each direction
#define MINIMUM_X -3
#define MINIMUM_Y -5
#define MINIMUM_Z 2 // minimum must be less than 2 because at
//default position, the Z axis should have a value of 0

//through performing oscillations and checking data on the serial plotter, 
//I have come up with these values for the X, Y, and Z maximums
#define MAXIMUM_X 4
#define MAXIMUM_Y 6
#define MAXIMUM_Z 8

//The oscillation function won't output true for 2 directions at the same time
//Thus I have implemented a counter to check over a small window of time
//counts oscillations in each direction
int CTR_X = 0;
int CTR_Y = 0;
int CTR_Z = 0;

//--------------END OF OSCILLATION MACROS AND VARIABLES---------------------------

//-------------READINGS AND ARRAYS OF THOSE READINGS------------------------------

//current x,y,z readings 
float xVal;
float yVal;
float zVal;

//array of the current and previous 19 values
float xValues[NUM_READINGS] = {0};
float yValues[NUM_READINGS] = {0};
float zValues[NUM_READINGS] = {0};

//-------------END OF READINGS AND ARRAYS OF THOSE READINGS------------------------------


//-------------SETUP FUNCTION------------------------------

//sets up circuit playground library
//starts serial with baud rate of 9600
void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);

}

//------------------------ARRAY MANIPULATION--------------------
//adds current reading to the array and shifts all the other data
//to the left
void add_data(float values[], float new_data)
{
  int x;
  //for all indexes, the previous value equals the next value
  for(x = 1; x <= (NUM_READINGS - 1);x++)
  {
    values[x-1]=values[x];
  }
  //the last spot in array equals the new reading
  values[NUM_READINGS - 1] = new_data;
}

//-------------END OF ARRAY MANIPULATION------------------------------

//--------------------------BASE OSCILLATION FUNCTION----------------------------------
//checks whether microcontroller is oscillating in one direction
int evaluate_oscillation(float values[], float minimum_check, float maximum_check)
{
  //-------------variables setup-----------------------
  //in the readings array, there are indices for the first minimum, second minimum, and maximum in between 
  //these variables are used for storing those indices
  int minimum1, maximum, minimum2;

  //counter is used for traveling along the hill and assigning indices
  int counter = 0;
  //steps to the maximum
  int step_to_max =0;
  //steps to the second minimum
  int step_to_min2=0;
  //--------------end of variables setup---------------

  //if the value at the beginning satisfies the minimum value for that direction
  if(values[counter] < minimum_check)
  {
    //already satisfied minimum check so used that value
    minimum1 = 0;
    
    //Check if the next value is smaller
    //keep traveling until not true
    while(values[counter + 1] < values[counter])
    {
      counter++;
    }

    //if the next value is greater, start counting steps to maximum
    while(values[counter+1] > values[counter])
     {
      counter++;//check next value
      step_to_max++;
     }
    //once while loop stops, counter is at the maximum
    //maximum is equal to the index of the maximum in the array
    maximum = counter;
    //if maximum value in the readings does not satisfy the check, oscillation has failed
    if (values[maximum] < maximum_check)
    {
      return 0;
    } 

    //while the next value is smaller, add to the steps to reach to the minimum and the counter
    while(values[counter] > values[counter + 1]){
      counter++;//check next value
      step_to_min2++;
    }
    //minimum is equal to the counter index
    minimum2 = counter;

    //calculating increasing and decreasing slope
    float first_slope = (values[maximum]- values[minimum1])/step_to_max;
    float second_slope = (values[maximum] - values[minimum2])/step_to_min2;
    // //------------------readings for serial plotter-----------------
    // Serial.print("first_slope:");
    // Serial.print(first_slope);
    // Serial.print(",");

    // Serial.print("second_slope:");
    // Serial.print(second_slope);
    // Serial.print(",");

    // Serial.print("maximum:");
    // Serial.print(values[maximum]);
    // Serial.print(",");

    // Serial.print("minimum:");
    // Serial.print(values[minimum1]);
    // Serial.print(",");
    // //------------------end of readings for serial plotter-----------------

    //if the slopes differ by a difference less than 3
    if (abs(first_slope - second_slope) < 5)
    {
      //if the steps to the second minimum and maximum are greater than 1
      //chose one to control the speed of the motion
      if ((step_to_min2> 1) && (step_to_max > 1))
      {
        //if first slope is greater with a magnitude of 1.2 (to be flexible with slower oscillations)
        if (first_slope > 1.5)
        {
          // //--------------checking on serial plotter-------------
          // Serial.print("Output:");
          // Serial.println(1);
          // Serial.print(",");
          // //--------------end of checking on serial plotter-------------
          
          //the motion is considered oscillation
          return 1; 
        }
      } 
    }
  }
  
  //motion failed to be oscillation
  return 0;
}
//--------------------------END OF BASE OSCILLATION FUNCTION----------------------------------

//--------------------------OSCILLATION PARAMETERS TUNING FOR EACH DIRECTION------------------
//tunes oscillation function for each direction
//evaluates whether oscillation is in x
int x_oscillation(float values[])
{
  return evaluate_oscillation(values, MINIMUM_X, MAXIMUM_X);
}//evaluates whether oscillation is in y
int y_oscillation(float values[])
{
  return evaluate_oscillation(values, MINIMUM_Y, MAXIMUM_Y);
}//evaluates whether oscillation is in z
int z_oscillation(float values[])
{
  return evaluate_oscillation(values, MINIMUM_Z, MAXIMUM_Z);
}
//------------------END OF OSCILLATION PARAMETERS TUNING FOR EACH DIRECTION-----------------------

//------------------MESSAGE SELECTION FOR EACH OSCILLATION GESTURE--------------------------------
uint8_t return_gesture_number()
{
  //if one of the counters is greater than 0 (indicating the oscillation )
  if (CTR_X > 0 || CTR_Y > 0 || CTR_Z > 0)
  {
      //if oscillation in x and y
      if (CTR_X> 1 && CTR_Y> 1 && CTR_Z == 0)
      {
        Serial.println("Cooking!");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        Serial.println("Oscillation counters reset");
        return 0;
      }

      //if oscillation in x and z
      else if (CTR_X> 1 && CTR_Y== 0 && CTR_Z > 1)
      {
        Serial.println("Washing Windows!");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        Serial.println("Oscillation counters reset");
        return 1;
      }

      //if oscillation in y and z
      else if (CTR_X== 0 && CTR_Y> 1 && CTR_Z > 1)
      {
        Serial.println("Cat Scratches!");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        Serial.println("Oscillation counters reset");
        return 2;
      }

      
      //counter must be greater than 5 so that oscillation in two directions
      //is not mistaken for one direction

      //if oscillation in y
      else if (CTR_Y > 5)
      {
        //Serial.println("Punching");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        Serial.println("Oscillation counters reset");
        return 3;
      }

      //if oscillation in z
      else if (CTR_Z > 5)
      {
        //Serial.println("Cheering");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        Serial.println("Oscillation counters reset");
        return 4;
      }
      else if (CTR_X > 5)
      {
        //Serial.println("Cleaning table");
        CTR_X = 0;
        CTR_Y = 0;
        CTR_Z = 0;
        return 5;
      }
      else
      {
        return -1;
      }
  }
  return -1;
}

//------------------END OF MESSAGE SELECTION FOR EACH OSCILLATION GESTURE--------------------------------

//lights up the neopixels to christmas colors
void light_neopixel(int gesture_number){
  
  //CircuitPlayground.clearPixels();
  //CircuitPlayground.setBrightness(1);
  if (gesture_number%2 == 0)
  {
    //green
    CircuitPlayground.setBrightness(3);
    CircuitPlayground.setPixelColor(gesture_number, 0, 200, 0);
  }
  else //(gesture_number%2 == 1)
  {
    //red
    CircuitPlayground.setBrightness(3);
    CircuitPlayground.setPixelColor(gesture_number, 200, 0, 0);
  }
 
}

void loop() {

  gesture_num = -1;
  //----------gets readings and puts in readings array-------------
  xVal = CircuitPlayground.motionX();
  yVal = CircuitPlayground.motionY();
  zVal = CircuitPlayground.motionZ();

  add_data(xValues, xVal);
  add_data(yValues, yVal);
  add_data(zValues, zVal);
    
  //one means detected, 0 means not detected
  int resultx = x_oscillation(xValues);
  int resulty = y_oscillation(yValues);
  int resultz = z_oscillation(zValues);

  //-------if oscillation in either direction, add to the counter
  if (resultx == 1)
    CTR_X++;
    
  if (resulty == 1)
    CTR_Y++;

  if (resultz == 1)
    CTR_Z++;
  //----------end of adding to counters----------------------------

  //check for the gesture
  gesture_num = return_gesture_number();

  if (gesture_num != -1){
    light_neopixel(gesture_num);
  }
  
  // Serial.print("gesture: ");
  // Serial.println(gesture_num);
  // Serial.print(",");
  //delay for samples for oscillation
  // Serial.print("Xvalues:");
  // Serial.println(xVal);
  // Serial.print(",");

  // Serial.print("Yvalues:");
  // Serial.println(yVal);
  // Serial.print(",");
  
  Serial.print("Zvalues:");
  Serial.println(zVal);
  Serial.print(",");
  Serial.print("x: ");
  Serial.println(CTR_X);
  Serial.print("y: ");
  Serial.println(CTR_Y);
  Serial.print("z: ");
  Serial.println(CTR_Z);

  delay(SAMPLE_TIME);
}