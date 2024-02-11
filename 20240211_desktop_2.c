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
#include "ftd2xx.h"
#include "libmpsse_spi.h"

void print_and_quit(const char cstring[]) {
    printf("%s\n", cstring);
    getc(stdin);
    exit(1);
}

int main(int argc, char **argv)
{
    Init_libMPSSE();

    FT_STATUS status;
    FT_DEVICE_LIST_INFO_NODE channelInfo;
    FT_HANDLE handle;

    DWORD channelCount = 0;
    status = SPI_GetNumChannels( &channelCount );

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
    channelConfig.ClockRate = 30000000;
    channelConfig.configOptions = SPI_CONFIG_OPTION_MODE2 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConfig.LatencyTimer = 1;
    status = SPI_InitChannel(handle, &channelConfig);
    if (status != FT_OK)
        print_and_quit("Error while initializing the MPSSE channel.");

    DWORD transferCount = 0;
    LPDWORD ptransferCount = &transferCount;
    DWORD options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
    UCHAR tx_buffer[20000];
    UCHAR rx_buffer[20000];
    //status = SPI_Write(handle, &tx_buffer[0], 10, ptransferCount, options);
    clock_t start, end;
    double time_used;

    for (int i = 0; i < 20000; i++) {
        tx_buffer[i] = (i * 2) % 256;
    }

    start = clock();
    //status = SPI_ReadWrite(handle, &rx_buffer[0], &tx_buffer[0], 1000, ptransferCount, options);
    status = SPI_Write(handle, &tx_buffer[0], 20000, ptransferCount, options);
    end = clock();

    time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    //printf("rx_buffer[900] = %d \n", rx_buffer[0]);
    //printf("rx_buffer[901] = %d \n", rx_buffer[1]);
    //printf("rx_buffer[902] = %d \n", rx_buffer[2]);
    //printf("rx_buffer[903] = %d \n", rx_buffer[3]);
    //printf("rx_buffer[904] = %d \n", rx_buffer[4]);
    //printf("rx_buffer[905] = %d \n", rx_buffer[5]);
    //printf("rx_buffer[906] = %d \n", rx_buffer[6]);
    //printf("rx_buffer[907] = %d \n", rx_buffer[7]);
    //printf("rx_buffer[908] = %d \n", rx_buffer[8]);
    //printf("rx_buffer[909] = %d \n", rx_buffer[9]);
    printf("Time taken : %f seconds \n", time_used);

    //status = SPI_Write(handle, &tx_buffer[0], 1, ptransferCount, options);
    //status = SPI_Write(handle, &tx_buffer[1], 1, ptransferCount, options);
    //status = SPI_Write(handle, &tx_buffer[2], 1, ptransferCount, options);
    //status = SPI_Write(handle, &tx_buffer[3], 1, ptransferCount, options);
    //status = SPI_Write(handle, &tx_buffer[4], 1, ptransferCount, options);
    //status = SPI_Write(handle, &tx_buffer[5], 1, ptransferCount, options);

    if (status!= FT_OK)
        print_and_quit("Error while configuring the servo");
 
    Cleanup_libMPSSE();
    return 0;
}
