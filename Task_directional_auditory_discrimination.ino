// Directional auditory discrimination
// Two speakers (L & R)  and two lick detectors (L & R)
// The mouse has to lick the L or R detector if a pure tone was plyed by the L or R speaker to receive water reward


// the assigned digital pins were chosen arbitrarily, assuming arduino nano is used 

// Inputs

// (assuming) the signal from the rp is HIGH while there is a mouse in the task area
const int rpIn = 4;             // raspberry pi input
int doTask = 0;                 // rpInState
int prevState = 0;              // the value of doTask from the previous iteration of the task loop
// to be added: another input related to the amount of water for that specific mouse

const int sensorLeft = 12;
int sensorLeftState = 0;
const int sensorRight = 11;
int sensorRightState = 0;

// Outputs

const int speakerLeft = 10;
const int speakerRight = 9;
int randNum;                    // for randomly selecting which speaker to play the sound from
int soundFrequency = 12000;     // in Hz, frequency of the tone to be played
long soundDuration = 1000;      // in ms, duration of the tone to be played

int waitTime = 5000;            // in ms, how long the system waits for a response after providing the stimulus
long startTime;                 // for tracking when the wait time has elapsed
long endTime;

const int waterValve = 8;
int waterTime = 2000;           // in ms, depending on amount of water to be delivered, duration for the solenoid to release water
const int waterAmount = 75;     // in ul, how much the water is released after a single successful trial (75ul in ToneBox)
int totalWaterAmount = 0;       // in ul, to track the total amount consumed by the mouse in the sequence of trials

// to send to rp after the mouse leaves the task area
char attemptOutcome;            // outcome of a single attempt, 'n' for no response, 
                                // 'c' for the correct sensor activated, 'i' for the wrong sensor
String outcomes = "";           // string of attempts' results in sequence

// to be added:
// time taken to respond

void setup() {
  
  Serial.begin(9600);
  
  srand(time(NULL)); 
  
  pinMode(rpIn,INPUT);
  pinMode(sensorLeft,INPUT);
  pinMode(sensorRight,INPUT);

  pinMode(speakerLeft,OUTPUT);
  pinMode(speakerRight,OUTPUT);
  pinMode(waterValve,OUTPUT);
  
}

void loop() {
  
  doTask = digitalRead(rpIn);
  
  if (doTask == HIGH){          // if the rp signals that the mouse is in the task area
    
    // perform task
    delay(3000);                // arbitrary 3 second wait
    randNum = random(2);        // pick a random number from 0 to 1
    
    if (randNum == 0){          // play sound from left speaker
      tone(speakerLeft, soundFrequency, soundDuration);
    } else {                    // play sound from right speaker
      tone(speakerRight, soundFrequency, soundDuration);   
    }
        
    startTime = millis();
    endTime = startTime;
    attemptOutcome = 'n';
    // loop until one of the sensors is activated or the wait time has passed
    while (((endTime - startTime) <= waitTime)){    // while the wait time has not passed
      
      sensorLeftState = digitalRead(sensorLeft);
      sensorRightState = digitalRead(sensorRight); 
      
      if (sensorLeftState==HIGH){                   // if the left sensor was activated
        
        if (randNum == 0){                          // if the left speaker played the tone
          attemptOutcome = 'c';                     // activating the left sensor was the correct response
        } else {                                    // if the right speaker played the tone
          attemptOutcome = 'i';                     // activating the left sensor was the incorrect response
        }      
        break;                                      // end attempt
        
      } else if (sensorRightState==HIGH){           // if the right sensor was activated
        
        if (randNum == 0){                          // if the left speaker played the tone
          attemptOutcome = 'i';                     // activating the right sensor was the incorrect response
        } else {                                    // if the right speaker played the tone
          attemptOutcome = 'c';                     // activating the right sensor was the correct response
        }
        break;                                      // end attempt
        
      }
      endTime = millis();
    }
    
    
    outcomes += attemptOutcome;                     // record result of attempt: append attemptOutcome to outcomes
    
    if(attemptOutcome == 'c'){                      // water reward for correct response
      digitalWrite(waterValve,HIGH);                // open solenoid valve
      delay(waterTime);                             // wait
      digitalWrite(waterValve,LOW);                 // close solenoid valve
      totalWaterAmount += waterAmount;              // add the amount of water administered to the water consumption counter
    }
    
    prevState = doTask;

    delay(2000);
    
  } else if (prevState == HIGH){                    // when the mouse has left the task area and it is done with all its attemps
    
    // to do: incluse total water amount to be sent to rp
    Serial.println(outcomes);                       // send info to rp
    outcomes = "";                                  // reset results string
    totalWaterAmount = 0;                           // reset water consumption tracking
    prevState = 0;
  }
  
}
