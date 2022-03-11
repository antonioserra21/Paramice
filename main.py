import RPi.GPIO as GPIO
import serial
import time
from datetime import datetime

SERVO_1 = 16 # adapt to your wiring
SERVO_2 = 15
fPWM = 50  # Hz (not higher with software PWM)
a = 10
b = 2
gate1 = True #True means closed
gate2 = True
mAreaFree = True
tAreaFree = True
seconds = 30

def setup():
    global pwm1
    global pwm2
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(SERVO_1, GPIO.OUT)
    GPIO.setup(SERVO_2, GPIO.OUT)
    pwm1 = GPIO.PWM(SERVO_1, fPWM)
    pwm2 = GPIO.PWM(SERVO_2, fPWM)
    pwm1.start(0)
    pwm2.start(0)


def setDirection(motor, direction):
    duty = a / 180 * direction + b
    if motor == 1:
        pwm1.ChangeDutyCycle(duty)
        #print("Motor 1 direction =", direction, "-> duty =", duty)
        time.sleep(1) # allow to settle
    elif motor == 2:
        pwm2.ChangeDutyCycle(duty)
        #print("Motor 1 direction =", direction, "-> duty =", duty)
        time.sleep(1) # allow to settle

def open(param, direction):
    if param == 1 and direction == 'R':
        setDirection(1, 170)
        globals()['gate1'] = False
        #print("Opening gate 1")
        pass
    elif param == 1 and direction == 'L':
        setDirection(1, 0)
        globals()['gate1'] = False
        #print("Opening gate 1")
        pass
    elif param == 2 and direction == 'R':
        setDirection(2, 170)
        globals()['gate2'] = False
        #print("Opening gate 2")
        pass
    elif param == 2 and direction == 'L':
        setDirection(2, 0)
        globals()['gate2'] = False
        #print("Opening gate 2")
        pass
    pass

def close(param):
    if param == 1:
        globals()['gate1'] = True
        #print("Closing gate 1")
        setDirection(1, 85)
        pass
    elif param == 2:
        globals()['gate2'] = True
        #print("Closing gate 2")
        setDirection(2, 85)
        pass
    pass

def skipHardInfo(serialLine):
    for x in range(3):  #the first three lines contain informations on the version of the hardware
        serialLine.readline()
    pass

def getReader(line):
    id = line[9]
    return id

def getWho(line):
    id = line[13:24]
    return id

# setup the serial line
ser = serial.Serial('/dev/ttyUSB0')
print("Reading informations from port", ser.name)

#setup servo motors configuration and be sure that both gates are closed
setup()
close(1)
close(2)

skipHardInfo(ser)

input = str(ser.readline())
#now = datetime.now()
#current_time = now.strftime("%H:%M:%S")
detLocation = getReader(input) #what reader has detected something?
who = getWho(input) #who was detected from the reader at location detLocation?
#print(detLocation+ "\t" + who + "\t" + current_time)
if (detLocation == '0' and mAreaFree and tAreaFree and gate1 and gate2):
    open(1, 'R')
    while not(gate1): #this loop will iterate until gate 1 is closed
        input = str(ser.readline())
        if getReader(input)== '2' and getWho(input) == who: #if the same mouse is detectd at reader 2
            close(1)
            mAreaFree = False
        pass
    start_time = time.time() #essential to iterate the next loop for 30 seconds
    while True:
        current_time = time.time()
        elapsed_time = current_time - start_time #these two lines will make the while loop last 30 sec
        if elapsed_time > seconds: #if 30 seconds passed
            open(2, 'R')
            break
        input = str(ser.readline()) #the readers 1 or 2 need to read something or the code will get stuck here
        who1 = getWho(input)
        if who != who1:
            print("ANOTHER MICE WAS DETECTED!")
            open(1, 'L')
            mAreaFree = True
            break
    #sctipt for the task to be added
    if mAreaFree == False:
        print("Initiating the task")
        start_time = time.time()
        time.sleep(5) #just for now to simulate the mouse doing the task
        #while breaks that ends when we get the signal that the mouse has completed the task
        current_time = time.time()
        elapsed_time = current_time - start_time
        print("Mouse " + who + " has completed the task in " + str(elapsed_time) + "seconds")
        open(2, 'L')
        open(1,'L')
        ser.readlines()
        pass

    while not(gate1): #wait for the mouse to exit the staging area and closes all the gates
        input = str(ser.readline())
        if getReader(input)== '0' and getWho(input) == who: #if the same mouse is detectd at reader 2
            close(1)
            close(2)
            mAreaFree = True
        pass
pass

ser.close()
