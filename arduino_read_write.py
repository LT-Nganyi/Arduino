import serial
import time
from datetime import datetime

arduino = serial.Serial(port='COM4', baudrate=115200, timeout=.1)

date = []
speed = []
def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline()
    if len(data.strip())>0:
        return data
    else:
         return ''

def serial_read():
    data = arduino.readline()
    return data

file = r'C:\Users\HP\Desktop\arduino_logs.txt'
while True:
    if len(speed)<10:

    #time.sleep(2)
        distance = str(serial_read())
        distance = distance.split(sep = '\'')
        if distance[1] == '':
            pass
        else:
            date.append(datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))
            speed.append(distance[1])
            print(distance[1])
    else:

        with open(file=file,mode='a+') as wFile:
            for x in range(len(speed)):
                log = date[x] + ' - '+ speed[x] + '\n'
                wFile.write(log)
        date.clear()
        speed.clear()
        print('Logs saved!')
    
    