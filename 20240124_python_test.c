count = 0
import time
import struct

binary_data1 = b''.join(struct.pack('<I', data) for data in step_position_data[0:500])
  
while True:
    if ser.in_waiting > 0:
        byte = ser.read(1)

        if (byte == '0'):
            ser.write(binary_data1)
            
        print("sent")
        
ser.close()
