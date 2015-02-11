import time
import serial
import numpy as np
import matplotlib.pyplot as plt

# simple script to receive data from an arduino 
# via serial port and plot the data 

port_name = "/dev/tty.usbmodem1421"
baud = 4000000
ser = serial.Serial(port=port_name, baudrate=baud)
data_buffer = [] # buffer to store incoming data
duration = 10.0 # duration (s) of reading from the serial port
elapsed = 0 # elapsed (s) time reading from the serial port

print "beginning to read data"
ser.flushInput() # get rid of any junk in the serial buffer
t_start = time.time()
while (elapsed <= duration): 
    data_buffer += ser.read(ser.inWaiting()) # blocks until all bytes read
    elapsed = time.time() - t_start
ser.close()

data = np.array([ord(c) for c in data_buffer]) # convert buffer to an numpy array
dt = elapsed / data.size # approximate sample interval
time = dt * np.arange(data.size) # time axis

print "read", data.size, "samples"

# plot the result
plt.figure(1)
plt.plot(time, data)
plt.xlabel("time (s)")
plt.ylabel("sensor output")
plt.show()
