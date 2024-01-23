#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define MAX_POS 65535
#define ARRAY_SIZE 1000
#define SEND_SIZE 500
#define UART_PORT "COM5" // Adjust as needed
#define BAUD_RATE 4000000

uint32_t pack_position_data(uint16_t x_pos, uint16_t y_pos) {
    return ((uint32_t)(x_pos & 0xFFFF) << 16) | (y_pos & 0xFFFF);
}

void generate_sine_position_data(uint32_t *position_data_array, int array_size) {
    for (int i = 0; i < array_size; i++) {
        uint16_t x = (uint16_t)(MAX_POS * sin(M_PI * i / array_size)) & 0xFFFF;
        uint16_t y = (uint16_t)(MAX_POS * sin(M_PI * i / array_size)) & 0xFFFF;
        position_data_array[i] = pack_position_data(x, y);
    }
}

void generate_step_position_data(uint32_t *position_data_array, int step, int array_size) {
    for (int i = 0; i < array_size; i++) {
        uint16_t x = (uint16_t)(step * (i + 1)) & 0xFFFF;
        uint16_t y = (uint16_t)(step * (i + 1)) & 0xFFFF;
        position_data_array[i] = pack_position_data(x, y);
    }
}

int main() {
    uint32_t sine_position_data[ARRAY_SIZE];
    uint32_t step_position_data[ARRAY_SIZE];
    uint8_t binary_data1[SEND_SIZE * 4]; // 4 bytes for each uint32_t
    uint8_t binary_data2[SEND_SIZE * 4];
    uint8_t binary_data3[SEND_SIZE * 4];
    uint8_t binary_data4[SEND_SIZE * 4];

    generate_sine_position_data(sine_position_data, ARRAY_SIZE);
    generate_step_position_data(step_position_data, 65, ARRAY_SIZE);

    // Packing the data
    memcpy(binary_data1, step_position_data, sizeof(binary_data1));
    memcpy(binary_data2, step_position_data + SEND_SIZE, sizeof(binary_data2));
    memcpy(binary_data3, sine_position_data, sizeof(binary_data3));
    memcpy(binary_data4, sine_position_data + SEND_SIZE, sizeof(binary_data4));

    HANDLE hSerial = initializeSerialPort(UART_PORT, BAUD_RATE);
    if (hSerial == INVALID_HANDLE_VALUE) {
        return 1;
    }

    // Reset input buffer
    if (!PurgeComm(hSerial, PURGE_RXCLEAR)) {
    fprintf(stderr, "Failed to purge serial port input buffer\n");
    CloseHandle(hSerial);
    return 1;
    }

    // Main loop for serial communication
    DWORD bytesWritten;
    DWORD bytesRead;
    char readBuffer[100];
    BOOL status;

    while (1) {
        status = ReadFile(hSerial, readBuffer, sizeof(readBuffer) - 1, &bytesRead, NULL);
        if (!status) {
            fprintf(stderr, "Error reading from serial port\n");
            break;
        }

        if (bytesRead > 0) {
            readBuffer[bytesRead] = '\0'; // Null-terminate the string
            strlwr(readBuffer); // Convert to lowercase

            if (strcmp(readBuffer, "4") == 0) {
                WriteFile(hSerial, binary_data1, sizeof(binary_data1), &bytesWritten, NULL);
            } else if (strcmp(readBuffer, "1") == 0) {
                WriteFile(hSerial, binary_data2, sizeof(binary_data2), &bytesWritten, NULL);
            } else if (strcmp(readBuffer, "2") == 0) {
                WriteFile(hSerial, binary_data3, sizeof(binary_data3), &bytesWritten, NULL);
            } else if (strcmp(readBuffer, "3") == 0) {
                WriteFile(hSerial, binary_data4, sizeof(binary_data4), &bytesWritten, NULL);
            }
        }
    }

    CloseHandle(hSerial); // Close serial port
    return 0;
}
