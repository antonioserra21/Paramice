import serial
import time
from datetime import datetime

def open(param):
    if param == 1:
        #script to open gate 1 to be added
        globals()['gate1'] = False
        print("Opening gate 1")
        pass
    elif param == 2:
        #script to open gate 2 to be added
        globals()['gate2'] = False
        print("Opening gate 2")
        pass
    pass

def close(param):
    if param == 1:
        globals()['gate1'] = True
        print("Closing gate 1")
        pass
    elif param == 2:
        globals()['gate2'] = True
        print("Closing gate 2")
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

gate1 = True #True means closed
gate2 = True
mAreaFree = True
tAreaFree = True
seconds = 30

# set up the serial line
ser = serial.Serial('/dev/ttyACM0')
print("Reading informations from port", ser.name)

skipHardInfo(ser)

input = str(ser.readline())
now = datetime.now()
current_time = now.strftime("%H:%M:%S")
detLocation = getReader(input)
who = getWho(input)
print(detLocation+ "\t" + who + "\t" + current_time)
if (detLocation == '0' and mAreaFree and tAreaFree and gate1 and gate2):
    open(1)
    while(not(gate1)):
        input = str(ser.readline())
        if getReader(input)== '2' and getWho(input) == who:
            close(1)
        pass
    start_time = time.time()
    while True:
        current_time = time.time()
        elapsed_time = current_time - start_time #these two lines will make the while loop last 30 sec
        if elapsed_time > seconds: #if 30 seconds passed
            open(2)
            start_time = time.time()
            break
        input = str(ser.readline()) #the readers 1 or 2 need to read something or the code will get stuck here
        who1 = getWho(input)
        if who != who1:
            print("ANOTHER MICE WAS DETECTED!")
            open(1)
            break
    #sctipt for the task to be added
    print("Initiating the task")
    time.sleep(5) #just for now to simulate the mouse doing the task
    #while breaks that ends when we get the signal that the mouse has completed the task
    current_time = time.time()
    elapsed_time = current_time - start_time
    print("Mouse " + who + " has completed the task in " + str(elapsed_time) + "seconds")
    pass






ser.close()
