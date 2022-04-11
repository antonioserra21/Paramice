// Auditory detection
// One speaker and one lick detector
// The mouse has to lick the detector after a pure tone is played by the speaker in order to receive water reward

// assuming arduino nano is used 

// Inputs

// (assuming) the signal from the rp is HIGH while there is a mouse in the task area
const int rpIn = 4;             // raspberry pi input
int doTask = 0;                 // rpInState
int prevState = 0;              // the value of doTask from the previous iteration of the task loop
// to be added: another input related to the amount of water for that specific mouse

const int sensor = 12;
int sensorState = 0;

// Outputs

const int speaker = 10;
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
char attemptOutcome;            // outcome of a single attempt, 'n' for no response, 'c' for the  sensor activated
String outcomes = "";           // string of attempts' results in sequence

// to be added:
// time taken to respond

void setup() {
  
  Serial.begin(9600);
  
  srand(time(NULL)); 
  
  pinMode(rpIn,INPUT);
  pinMode(sensor,INPUT);

  pinMode(speaker,OUTPUT);
  pinMode(waterValve,OUTPUT);
  
}

void loop() {
  
  doTask = digitalRead(rpIn);
  
  if (doTask == HIGH){          // if the rp signals that the mouse is in the task area
    
    // perform task
    delay(3000);                // arbitrary 3 second wait
    randNum = random(2);        // pick a random number from 0 to 1
          
    tone(speaker, soundFrequency, soundDuration); // play sound from left speaker
            
    startTime = millis();
    endTime = startTime;
    attemptOutcome = 'n';
    // loop until the sensor is activated or the wait time has passed
    while (((endTime - startTime) <= waitTime)){    // while the wait time has not passed
      
      sensorState = digitalRead(sensor);
      
      if (sensorLeftState==HIGH){                   // if the sensor was activated

        attemptOutcome = 'c';                       // activating the sensor was the correct response    
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
