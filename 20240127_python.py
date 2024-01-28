count = 0
import time
import struct
start_time = 0
end_time = 0

binary_data1 = b''.join(struct.pack('<I', data) for data in step_position_data[0:250])
binary_data2 = b''.join(struct.pack('<I', data) for data in step_position_data[500:750])
binary_data3 = b''.join(struct.pack('<I', data) for data in sine_position_data[0:250])
binary_data4 = b''.join(struct.pack('<I', data) for data in sine_position_data[500:750])
    
while True:
    if ser.in_waiting > 0:
        start_time = time.time()
        print(start_time - end_time)
        received_data = ser.readline().decode().strip()
        ser.reset_input_buffer()

        if (received_data.lower() == "1"):
            ser.write(binary_data1)
            end_time = time.time()
        elif (received_data.lower() == "2"):
            ser.write(binary_data2)
            end_time = time.time()
        elif (received_data.lower() == "3"):
            ser.write(binary_data3)
            end_time = time.time()
        elif (received_data.lower() == "4"):
            ser.write(binary_data4)
            end_time = time.time()
        #print(received_data.lower())
            
ser.close()
