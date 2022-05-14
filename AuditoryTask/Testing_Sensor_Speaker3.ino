// Declaration and initialization of input pins
const int sensorLeft = 3; // digital output of the sensor
int sensorStateLeft;

const int sensorRight = 4; 
int sensorStateRight;

const int speakerLeft = 5;
const int speakerRight = 6;

int randNum;
int soundFrequency = 2000;     // in Hz, frequency of the tone to be played
long soundDuration = 1000;      // in ms, duration of the tone to be played

int waitTime = 5000;            // in ms, how long the system waits for a response after providing the stimulus
long startTime;                 // for tracking when the wait time has elapsed
long endTime;

String attemptOutcome = "#";
String outcomes = "";           // string of attempts' results in sequence

const int rpIn = 7;             // raspberry pi input
int doTask = 0;                 // rpInState
int prevState = 0;

/*
const int waterValve = 8;
int waterTime = 2000;           // in ms, depending on amount of water to be delivered, duration for the solenoid to release water
const int waterAmount = 75;     // in ul, how much the water is released after a single successful trial (75ul in ToneBox)
int totalWaterAmount = 0;       // in ul, to track the total amount consumed by the mouse in the sequence of trials
*/
  
void setup  ( )
{
  Serial.begin (9600) ;  //  Serial output with 9600 bps
  pinMode (sensorLeft, INPUT);
  pinMode (sensorRight, INPUT);     
  
  pinMode(speakerLeft,OUTPUT);
  pinMode (speakerRight, INPUT);

  pinMode(rpIn,INPUT);
}
  
void loop  ( )
{
  
  doTask = digitalRead(rpIn);
  
  if (doTask == 1){ 
     if(prevState==0){
        Serial.println("start of attempts sequence");
     }
   
     delay(3000);                // arbitrary 3 second wait
  
     randNum = random(2);        // pick a random number from 0 to 1
     
     if (randNum == 0){          // play sound from left speaker
       Serial.println("playing Left sound");
       attemptOutcome += "L";
       tone(speakerLeft, soundFrequency, soundDuration);
     } else {                    // play sound from right speaker
       Serial.println("playing Right sound");
       attemptOutcome += "R";
       tone(speakerRight, soundFrequency, soundDuration);   
     }
  
     startTime = millis();
     endTime = startTime;
     // loop until the sensor is activated or the wait time has passed
     Serial.println("checking sensor");
     while (((endTime - startTime) <= waitTime)){    // while the wait time has not passed
        
       sensorStateLeft = digitalRead(sensorLeft);
       sensorStateRight = digitalRead(sensorRight);
  
       if  (sensorStateLeft==1) {
        attemptOutcome += "L";
        Serial.println ("-----YES LEFT                                        ");
        Serial.println("detected left response");
        break;   
        
       } else if (sensorStateRight==1){           // if the right sensor was activated
        attemptOutcome += "R";
        Serial.println ("                                        YES RIGHT-----");
        Serial.println("detected right response");
        break;  
           
       } else {
        Serial.println ("____no________________");
        
       }
        
       // add delay
       delay(200);
       endTime = millis();
      }
      if(attemptOutcome.length() == 2){
        attemptOutcome+="n";
        }
      outcomes += attemptOutcome;
  
      if(attemptOutcome == "#LL" || attemptOutcome == "#RR"){                      // water reward for correct response
        Serial.println("CORRECT!");
        //digitalWrite(waterValve,HIGH);                // open solenoid valve
        //delay(waterTime);                             // wait
        //digitalWrite(waterValve,LOW);                 // close solenoid valve
        //totalWaterAmount += waterAmount;              // add the amount of water administered to the water consumption counter
      }
      Serial.println("attempt outcome: ");
      Serial.println(attemptOutcome);
      attemptOutcome="#";

      prevState = doTask;
      delay(2000);
      
      
  } else if (prevState == 1){
    Serial.println("Outcomes:");
    Serial.println(outcomes);
    outcomes = "";                                  
    prevState = 0;
  }
}
