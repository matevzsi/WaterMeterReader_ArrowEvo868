# Read serial port and write it to log file with timestamp of received data
import serial
import datetime

s = serial.Serial('COM10')

while True:
    if s.in_waiting > 0:
        serialString = s.readline()

        dataLine = str(datetime.datetime.now()) + " " + serialString.decode().replace("\r\n","")
        print(dataLine)
        try:
            file_object = open('waterLogNew.txt', 'a')
            file_object.write(dataLine + "\n")
            file_object.close()
        except:
            print("Error writing to file")
            pass
            
            