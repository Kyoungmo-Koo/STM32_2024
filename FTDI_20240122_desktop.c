#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include "libMPSSE_spi.h"

#define MAX_POS 65535
#define ARRAY_SIZE 1000
#define SEND_SIZE 500
#define UART_PORT "COM5" // Adjust as needed
#define BAUD_RATE 4000000

void print_and_quit(char cstring[]){
    printf("%s\n", cstring);
    getc(stdin);
    exit(1);
}

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
    uint32_t rx_buffer[SEND_SIZE * 4];

    generate_sine_position_data(sine_position_data, ARRAY_SIZE);
    generate_step_position_data(step_position_data, 65, ARRAY_SIZE);

    // Packing the data
    memcpy(binary_data1, step_position_data, sizeof(binary_data1));
    memcpy(binary_data2, step_position_data + SEND_SIZE, sizeof(binary_data2));
    memcpy(binary_data3, sine_position_data, sizeof(binary_data3));
    memcpy(binary_data4, sine_position_data + SEND_SIZE, sizeof(binary_data4));

    Init_libMPSSE();
    FT_STATUS status;
    FT_DEVICE_LIST_INFO_NODE channelInfo;
    FT_HANDLE handle;
    uint32 channelCount = 0;

    status = SPI_GetNumChannels(&channelCount);
    if(status != FT_OK){
        print_and_quit("Error while checking the number of available MPSSE channels.");
    }
    else if(channelCount < 1){
        print_and_quit("Error: no MPSSE channels are available.");
    }
    printf("There are %d channels available \n\n", channelCount);

    for (int i = 0; i < channelCount; i ++){
        status = SPI_GetChannelInfo(i, &channelInfo);
        if(status != FT_OK){
            print_and_quit("Error while getting details for an MPSSE channel.");
        }
        printf("Channel number : %d \n", i);
        printf("Description: %s \n", channelInfo.Description);
        printf("Serial Number: %d \n", channelInfo.SerialNumber);
    }

    uint32 channel = 0;
    printf("\n Enter a channel number to use: ");
    scanf_s("%d", &channel);

    status = SPI_OpenChannel(channel, &handle);
    if(status != FT_OK){
        print_and_quit("Error while opening the MPSSE channel.");
    }

    ChannelConfig channelConfig;
    channelConfig.ClockRate = 1000000;
    channelConfig.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConfig.LatencyTimer = 75;
    status = SPI_InitChannel(handle, &channelConfig);
    if(status != FT_OK){
        print_and_quit("Error while initializing the MPSSE channel.");
    }

    uint32 transferCount1 = 0;
    uint32 transferCount2 = 0;
    uint32 options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

uint8_t rx_byte = '0';



while (1) {
    status = SPI_Read(&handle, rx_byte, 1, &transferCount1, options);

    if(status != FT_OK){
        print_and_quit("Error while configuring the servo");
    }

        if (strcmp(readBuffer, "4") == 0) {
           SPI_ReadWrite(handle, rxbuffer, binary_data1, SEND_SIZE, &transferCount2, options);
        } else if (strcmp(readBuffer, "1") == 0) {
            SPI_ReadWrite(handle, rxbuffer, binary_data2, SEND_SIZE, &transferCount2, options);
        } else if (strcmp(readBuffer, "2") == 0) {
            SPI_ReadWrite(handle, rxbuffer, binary_data3, SEND_SIZE, &transferCount2, options);
        } else if (strcmp(readBuffer, "3") == 0) {
            SPI_ReadWrite(handle, rxbuffer, binary_data4, SEND_SIZE, &transferCount2, options);
    }

    Cleanup_libMPSSE();
    return 0;
}
