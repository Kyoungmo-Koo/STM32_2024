//Red Wire = 3V3
//Black Wire = Ground
//Orange Wire = SCLK
//Yellow Wire = MOSI
//Green Wire = MISO
//Brown Wire = CS

#include <chrono>
#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>

#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include "ftd2xx.h"
#include "libmpsse_spi.h"

HANDLE hComm;
int NUM_OF_POSITIONS = 5000;
int COUNT = 200;

void print_and_quit(const char cstring[]) {
    printf("%s\n", cstring);
    getc(stdin);
    exit(1);
}

void Init_UART() {
    BOOL status;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

    hComm = CreateFile(L"\\\\.\\COM7", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error in opening serial port\n");
    }
    else {
        printf("Opening serial port successful\n");
    }
    status = PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);
    if (!status) {
        printf("Error in purging comm\n");
        CloseHandle(hComm);
    }
    status = GetCommState(hComm, &dcbSerialParams);
    if (!status) {
        printf("Error in GetCommState\n");
        CloseHandle(hComm);
    }

    dcbSerialParams.BaudRate = CBR_9600;  
    dcbSerialParams.ByteSize = 8;        
    dcbSerialParams.StopBits = ONESTOPBIT; 
    dcbSerialParams.Parity = NOPARITY;     

    status = SetCommState(hComm, &dcbSerialParams);
    if (!status) {
        printf("Error in setting DCB structure\n");
        CloseHandle(hComm);
    }

    timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 100;

    if (!SetCommTimeouts(hComm, &timeouts)) {
        printf("Error in setting timeouts\n");
        CloseHandle(hComm);
    }
}

void reversePacketOrder(UCHAR* buffer, size_t bufferSize) {
    for (size_t i = 0; i < bufferSize; i += 4) {
        // Swap bytes within each packet
        for (size_t j = 0; j < 2; ++j) {
            UCHAR temp = buffer[i + j];
            buffer[i + j] = buffer[i + 3 - j];
            buffer[i + 3 - j] = temp;
        }
    }
}

void Init_SPI(){
    FT_STATUS status, status2;
    FT_DEVICE_LIST_INFO_NODE channelInfo;
    FT_HANDLE handle;

    DWORD channelCount = 0;
    status = SPI_GetNumChannels(&channelCount);

    if (status != FT_OK)
        print_and_quit("Error while checking the number of available MPSSE channels.");
    else if (channelCount < 1)
        print_and_quit("Error: No MPSSE channels are available.");

    printf("There are %d channels available. \n\n", channelCount);

    for (int i = 0; i < channelCount; i++) {
        status = SPI_GetChannelInfo(i, &channelInfo);
        if (status != FT_OK)
            print_and_quit("Error while getting details for an MPSSE channel.");
        printf("Channel number : %d\n", i);
        printf("Description: %s\n", channelInfo.Description);
        printf("Serial Number : %d\n", channelInfo.SerialNumber);
    }

    uint32_t channel = 0;
    printf("\n Enter a channel number to use: ");
    scanf_s("%d", &channel);

    status = SPI_OpenChannel(channel, &handle);
    if (status != FT_OK)
        print_and_quit("Error while opening the MPSSE channel.");

    ChannelConfig channelConfig;
    channelConfig.ClockRate = 15000000;
    channelConfig.configOptions = SPI_CONFIG_OPTION_MODE2 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConfig.LatencyTimer = 100;

    status = SPI_InitChannel(handle, &channelConfig);
    if (status != FT_OK)
        print_and_quit("Error while initializing the MPSSE channel.");
}

void Open_File(fp, fr, fr2){
    fopen_s(&fp, "data.csv", "w+");
    fopen_s(&fr, "sine_position_data.txt", "r");
    fopen_s(&fr2, "step_position_data.txt", "r");
}

void Read_File(RxByte, tx_buffer, rx_buffer, fr, fr2, input_data){
    if (RxByte == 0 || RxByte == 1) {
            for (int k = 0; k < NUM_OF_POSITIONS; ++k) {
                fscanf_s(fr, "%x", &input_data[k]);
            }
            fseek(fr, 0, SEEK_SET);
        }
        else {
            for (int k = 0; k < NUM_OF_POSITIONS; ++k) {
                fscanf_s(fr2, "%x", &input_data[k]);
            }
            fseek(fr2, 0, SEEK_SET);
        }

        for (int k = 0; k < NUM_OF_POSITIONS; ++k) {
            uint32_t data = input_data[k];
            tx_buffer[4*k + 3] = (uint8_t)((data >> 24) & 0xFF);
            tx_buffer[4*k + 2] = (uint8_t)((data >> 16) & 0xFF);
            tx_buffer[4*k + 1] = (uint8_t)((data >> 8) & 0xFF);
            tx_buffer[4*k] = (uint8_t)(data & 0xFF);
        }
}

void Write_File(fp, rx_buffer){
    for (int j = 0; j < sizeof(rx_buffer); j += 4) {
        x = rx_buffer[j+3] * 256 + rx_buffer[j+2];
        y = rx_buffer[j+1] * 256 + rx_buffer[j];
        fprintf(fp, "x:%d y:%d ,", x, y);
    }
        fprintf(fp, "\n");
}

int test(int argc, char** argv)
{
    FILE* fp;
    FILE* fr;
    FILE* fr2;

    Init_UART();
    Init_libMPSSE();
    Init_SPI();
    Open_File(fp, fr, fr2);

    DWORD bytesRead;
    DWORD bytesWrite;
    UCHAR StartByte = 0;
    UCHAR RxByte;

    DWORD transferCount = 0;
    LPDWORD ptransferCount = &transferCount;
    DWORD options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
    DWORD input_data[NUM_OF_POSITIONS];
    UCHAR tx_buffer[4 * NUM_OF_POSITIONS];
    UCHAR rx_buffer[2 * NUM_OF_POSITIONS];

    int x;
    int y;
    long position;

    status = WriteFile(hComm, &StartByte, 1, &bytesWrite, NULL);
    status = ReadFile(hComm, &RxByte, 1, &bytesRead, NULL);
    printf("%d \n", RxByte);

    for (int i = 0; i < COUNT; i++) {
        Sleep(1);
        status = SPI_Read(handle, &rx_buffer[0], 2 * NUM_OF_POSITIONS, ptransferCount, options);
        Sleep(1);
        Read_File(RxByte, tx_buffer, rx_buffer, fr, fr2, input_data);

        if (RxByte == 0) {
            status = SPI_Write(handle, &tx_buffer[0], 2 * NUM_OF_POSITIONS, ptransferCount, options);
        }
        else if (RxByte == 1) {
            status = SPI_Write(handle, &tx_buffer[2 * NUM_OF_POSITIONS], 2 * NUM_OF_POSITIONS, ptransferCount, options);
        }
        else if (RxByte == 2) {
            status = SPI_Write(handle, &tx_buffer[0], 2 * NUM_OF_POSITIONS, ptransferCount, options);
        }
        else if (RxByte == 3) {
            status = SPI_Write(handle, &tx_buffer[2 * NUM_OF_POSITIONS], 2 * NUM_OF_POSITIONS, ptransferCount, options);
        }
        status2 = ReadFile(hComm, &RxByte, 1, &bytesRead, NULL);
        Write_File(fp, rx_buffer);
        printf("%d \n", RxByte);
    }
    Cleanup_libMPSSE();
    return 0;
}

int main(int argc, char** argv) {
    test(argc, argv);
}
