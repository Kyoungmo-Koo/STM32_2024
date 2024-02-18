//Red Wire = 3V3
//Black Wire = Ground
//Orange Wire = SCLK
//Yellow Wire = MOSI
//Green Wire = MISO
//Brown Wire = CS

#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include "ftd2xx.h"
#include "libmpsse_spi.h"

HANDLE hComm;

void print_and_quit(const char cstring[]) {
    printf("%s\n", cstring);
    getc(stdin);
    exit(1);
}

void Init_UART() {
    BOOL status;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytesRead;
    UCHAR Buffer[5] = { 0, 0, 0, 0, 0 };
    DWORD dwEventMask;

    // Open the serial port
    hComm = CreateFile(L"\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

    dcbSerialParams.BaudRate = CBR_9600;  // Set baud rate to 9600 (you can change this)
    dcbSerialParams.ByteSize = 8;         // Data bit = 8
    dcbSerialParams.StopBits = ONESTOPBIT; // One stop bit
    dcbSerialParams.Parity = NOPARITY;     // No parity
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

int main(int argc, char** argv)
{
    Init_UART();
    Init_libMPSSE();

    DWORD bytesRead;
    DWORD bytesWrite;
    UCHAR Buffer[4] = { 1, 1, 1, 1 };
    UCHAR RxBuffer[4] = { 1, 2, 3, 4 };

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
    channelConfig.LatencyTimer = 45;

    status = SPI_InitChannel(handle, &channelConfig);
    if (status != FT_OK)
        print_and_quit("Error while initializing the MPSSE channel.");

    DWORD transferCount = 0;
    LPDWORD ptransferCount = &transferCount;
    DWORD options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
    UCHAR tx_buffer[20000];
    UCHAR tx_buffer2[20000];
    UCHAR rx_buffer[10000];
    WORD tx_buffer_16[10000];
    WORD tx_buffer2_16[10000];
    clock_t start, end;
    double time_used;

    for (int i = 0; i < 10000; i++) {
        tx_buffer_16[i] = 6 * (i / 2);
    }

    for (int i = 0; i < 10000; i++) {
        tx_buffer[2 * i + 1] = tx_buffer_16[i] & 0xFF;
        tx_buffer[2 * i] = (tx_buffer_16[i] >> 8) & 0xFF;
    }

    for (int i = 0; i < 10000; i++) {
        double value = 65535 * sin((i / 2) * 3.141592 / 5000);
        tx_buffer2_16[i] = (uint16_t)(value);
    }

    for (int i = 0; i < 10000; i++) {
        tx_buffer2[2 * i + 1] = tx_buffer2_16[i] & 0xFF;
        tx_buffer2[2 * i] = (tx_buffer2_16[i] >> 8) & 0xFF;
    }

    status = WriteFile(hComm, &RxBuffer[0], 1, &bytesRead, NULL);
    status = ReadFile(hComm, &Buffer[0], 1, &bytesWrite, NULL);
    printf("%d \n", Buffer[0]);


    for (int i = 0; i < 10000; i++) {
        start = clock();
        Sleep(1);
        status = SPI_Read(handle, &rx_buffer[0], 10000, ptransferCount, options);
        Sleep(1);
        if (Buffer[0] == 1) {
            status = SPI_Write(handle, &tx_buffer[0], 10000, ptransferCount, options);
        }
        else if (Buffer[0] == 2) {
            status = SPI_Write(handle, &tx_buffer[10000], 10000, ptransferCount, options);
        }
        else if (Buffer[0] == 3) {
            status = SPI_Write(handle, &tx_buffer2[0], 10000, ptransferCount, options);
        }
        else if (Buffer[0] == 0) {
            status = SPI_Write(handle, &tx_buffer2[10000], 10000, ptransferCount, options);
        }
        status2 = ReadFile(hComm, &Buffer[0], 1, &bytesWrite, NULL);
        end = clock();

        time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        //printf("Time taken : %f seconds \n", time_used);
        //printf("%d \n", rx_buffer[9990]);
        //printf("%d \n", rx_buffer[9991]);
        //printf("%d \n", rx_buffer[9992]);
        //printf("%d \n", rx_buffer[9993]);
        //printf("%d \n", rx_buffer[9994]);
        //printf("%d \n", rx_buffer[9995]);
        //printf("%d \n", rx_buffer[9996]);
        //printf("%d \n", rx_buffer[9997]);
        //printf("%d \n", Buffer[0]);
    }

    if (status != FT_OK)
        print_and_quit("Error elsewhere");
    if (status2 != FT_OK)
        print_and_quit("Error while configuring the servo");

    Cleanup_libMPSSE();
    return 0;
}
