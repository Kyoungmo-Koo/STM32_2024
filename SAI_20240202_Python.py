import math
import serial

MAX_POS = 65535
ARRAY_SIZE = 10000
SEND_SIZE = 5000
UART_PORT = 'COM5'
BAUD_RATE = 5000000

def pack_position_data(x_pos, y_pos):
    return ((x_pos & 0xFFFF) << 16) | (y_pos & 0xFFFF)

def generate_sine_position_data(array_size):
    position_data_array = []
    for i in range(array_size):
        x = int(MAX_POS * math.sin(1.0 * math.pi * i / array_size)) & 0xFFFF
        y = int(MAX_POS * math.sin(1.0 * math.pi * i / array_size)) & 0xFFFF
        position_data_array.append(pack_position_data(x, y))
    return position_data_array

def generate_step_position_data(step, array_size):
    position_data_array = []
    for i in range(1, array_size + 1):
        x = step * i
        y = step * i
        position_data_array.append(pack_position_data(x, y))
    return position_data_array

-------------------------------------------------------------------------------------------------

# Generate data
sine_position_data = generate_sine_position_data(ARRAY_SIZE)
step_position_data = generate_step_position_data(6, ARRAY_SIZE)

# UART setup
ser = serial.Serial(UART_PORT, BAUD_RATE, timeout=1)
ser.reset_input_buffer()
ser.reset_output_buffer()

---------------------------------------------------------------------------------------------------

import time
import struct
start_time = 0
end_time = 0

binary_data1 = b''.join(struct.pack('<I', data) for data in step_position_data[0: 5000])
binary_data2 = b''.join(struct.pack('<I', data) for data in step_position_data[5000:10000])
binary_data3 = b''.join(struct.pack('<I', data) for data in sine_position_data[0:5000])
binary_data4 = b''.join(struct.pack('<I', data) for data in sine_position_data[5000:10000])

ser.write(b'@')

while True:
    if ser.in_waiting > 0:
        start_time = time.time()
        received_data = ser.readline().decode().strip()
        ser.reset_input_buffer()

        if (received_data.lower() == "1"):
            ser.write(binary_data1)
            print(time.time() - start_time) 
        elif (received_data.lower() == "2"):
            ser.write(binary_data2)
            print(time.time() - start_time) 
        elif (received_data.lower() == "3"):
            ser.write(binary_data3)
            print(time.time() - start_time) 
        elif (received_data.lower() == "4"):
            ser.write(binary_data4)
            print(time.time() - start_time) 

-------------------------------------------------------------------------------------------------

ser.close()
