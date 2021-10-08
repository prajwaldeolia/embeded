'''import serial

ser = serial.Serial(
    port='/dev/ttyACM0',\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)
count=1

while True:
    for line in ser.read():

        print(str(count) + str(': ') + chr(line) )
        count = count+1

ser.close()




import serial as s
import time
#s=serial.Serial("/dev/ttyACM0",timeout=5)
time.sleep(5) # wait a couple seconds
#s.write(b"\xe6\x00\x00\x00&")
# or alternatively
s.Write(bytes(bytearray([0xe6,0x00,0x00,0x00,0x26])))
print(repr(s.read(1000)));
'''




from serial import Serial

ser = serial.Serial('/dev/ttyACM0',9600)
s = [0]
while True:
	read_serial=ser.readline()
	s[0] = str(int (ser.readline(),16))
	print (s[0])
	print (read_serial)



