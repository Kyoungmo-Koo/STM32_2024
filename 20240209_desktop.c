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
    channelConfig.ClockRate = 1000000;
    channelConfig.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConfig.LatencyTimer = 75;
    status = SPI_InitChannel(handle, &channelConfig);
    if (status != FT_OK)
        print_and_quit("Error while initializing the MPSSE channel.");

    DWORD transferCount = 0;
    LPDWORD ptransferCount = &transferCount;
    DWORD options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
    UCHAR tx_buffer[6] = { 1, 2, 3, 4, 5, 6 };
    while (1) {
        status = SPI_Write(handle, tx_buffer, 6, ptransferCount, options);
    }
    

    if (status!= FT_OK)
        print_and_quit("Error while configuring the servo");
    Cleanup_libMPSSE();
    return 0;
}
