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
    channelConfig.ClockRate = 5000000;
    channelConfig.configOptions = SPI_CONFIG_OPTION_MODE2 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConfig.LatencyTimer = 1000;

    status = SPI_InitChannel(handle, &channelConfig);
    if (status != FT_OK)
        print_and_quit("Error while initializing the MPSSE channel.");

    DWORD transferCount = 0;
    LPDWORD ptransferCount = &transferCount;
    DWORD options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
    UCHAR tx_buffer[20000];
    UCHAR rx_buffer[10000];
    //status = SPI_Write(handle, &tx_buffer[0], 10, ptransferCount, options);
    clock_t start, end;
    double time_used;

    for (int i = 0; i < 20000; i++) {
        tx_buffer[i] = (i * 2) % 256;
    }

    start = clock();
    //status = SPI_ReadWrite(handle, &rx_buffer[0], &tx_buffer[0], 1000, ptransferCount, options);
    status = SPI_Write(handle, &tx_buffer[0], 20000, ptransferCount, options);
    status = SPI_Read(handle, &rx_buffer[0], 10000, ptransferCount, options);
    end = clock();

    time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("rx_buffer[0] = %d \n", rx_buffer[0]);
    printf("rx_buffer[1] = %d \n", rx_buffer[1]);
    printf("rx_buffer[2] = %d \n", rx_buffer[2]);
    printf("rx_buffer[3] = %d \n", rx_buffer[3]);
    printf("rx_buffer[4] = %d \n", rx_buffer[4]);
    printf("rx_buffer[5] = %d \n", rx_buffer[5]);
    printf("rx_buffer[6] = %d \n", rx_buffer[6]);
    printf("rx_buffer[7] = %d \n", rx_buffer[7]);
    printf("rx_buffer[8] = %d \n", rx_buffer[8]);
    printf("rx_buffer[9] = %d \n", rx_buffer[9]);
    printf("rx_buffer[190] = %d \n", rx_buffer[190]);
    printf("rx_buffer[191] = %d \n", rx_buffer[191]);
    printf("rx_buffer[192] = %d \n", rx_buffer[192]);
    printf("rx_buffer[193] = %d \n", rx_buffer[193]);
    printf("rx_buffer[194] = %d \n", rx_buffer[194]);
    printf("rx_buffer[195] = %d \n", rx_buffer[195]);
    printf("rx_buffer[196] = %d \n", rx_buffer[196]);
    printf("rx_buffer[197] = %d \n", rx_buffer[197]);
    printf("rx_buffer[198] = %d \n", rx_buffer[198]);
    printf("rx_buffer[199] = %d \n", rx_buffer[199]);
    printf("rx_buffer[301] = %d \n", rx_buffer[301]);
    printf("rx_buffer[302] = %d \n", rx_buffer[302]);
    printf("rx_buffer[303] = %d \n", rx_buffer[303]);
    printf("rx_buffer[401] = %d \n", rx_buffer[401]);
    printf("rx_buffer[402] = %d \n", rx_buffer[402]);
    printf("rx_buffer[403] = %d \n", rx_buffer[403]);
    printf("rx_buffer[501] = %d \n", rx_buffer[501]);
    printf("rx_buffer[502] = %d \n", rx_buffer[502]);
    printf("rx_buffer[503] = %d \n", rx_buffer[503]);
    printf("rx_buffer[601] = %d \n", rx_buffer[601]);
    printf("rx_buffer[602] = %d \n", rx_buffer[602]);
    printf("rx_buffer[603] = %d \n", rx_buffer[603]);
    printf("rx_buffer[701] = %d \n", rx_buffer[701]);
    printf("rx_buffer[702] = %d \n", rx_buffer[702]);
    printf("rx_buffer[703] = %d \n", rx_buffer[703]);

    printf("rx_buffer[901] = %d \n", rx_buffer[901]);
    printf("rx_buffer[902] = %d \n", rx_buffer[902]);
    printf("rx_buffer[903] = %d \n", rx_buffer[903]);
    printf("rx_buffer[1001] = %d \n", rx_buffer[1001]);
    printf("rx_buffer[1002] = %d \n", rx_buffer[1002]);
    printf("rx_buffer[1003] = %d \n", rx_buffer[1003]);
    printf("rx_buffer[1101] = %d \n", rx_buffer[1101]);
    printf("rx_buffer[1102] = %d \n", rx_buffer[1102]);
    printf("rx_buffer[1103] = %d \n", rx_buffer[1103]);
    printf("rx_buffer[1201] = %d \n", rx_buffer[1201]);
    printf("rx_buffer[1202] = %d \n", rx_buffer[1202]);
    printf("rx_buffer[1203] = %d \n", rx_buffer[1203]);

    printf("rx_buffer[1501] = %d \n", rx_buffer[1501]);
    printf("rx_buffer[1502] = %d \n", rx_buffer[1502]);
    printf("rx_buffer[1503] = %d \n", rx_buffer[1503]);
    printf("rx_buffer[1601] = %d \n", rx_buffer[1601]);
    printf("rx_buffer[1602] = %d \n", rx_buffer[1602]);
    printf("rx_buffer[1603] = %d \n", rx_buffer[1603]);
    printf("rx_buffer[1701] = %d \n", rx_buffer[1701]);
    printf("rx_buffer[1702] = %d \n", rx_buffer[1702]);
    printf("rx_buffer[1703] = %d \n", rx_buffer[1703]);
    printf("rx_buffer[1801] = %d \n", rx_buffer[1801]);
    printf("rx_buffer[1802] = %d \n", rx_buffer[1802]);
    printf("rx_buffer[1803] = %d \n", rx_buffer[1803]);

    printf("rx_buffer[2001] = %d \n", rx_buffer[2001]);
    printf("rx_buffer[2002] = %d \n", rx_buffer[2002]);
    printf("rx_buffer[2003] = %d \n", rx_buffer[2003]);
    printf("rx_buffer[3001] = %d \n", rx_buffer[3001]);
    printf("rx_buffer[3002] = %d \n", rx_buffer[3002]);
    printf("rx_buffer[3003] = %d \n", rx_buffer[3003]);
    printf("rx_buffer[4001] = %d \n", rx_buffer[4001]);
    printf("rx_buffer[4002] = %d \n", rx_buffer[4002]);
    printf("rx_buffer[4003] = %d \n", rx_buffer[4003]);

    printf("rx_buffer[5001] = %d \n", rx_buffer[5001]);
    printf("rx_buffer[5002] = %d \n", rx_buffer[5002]);
    printf("rx_buffer[5003] = %d \n", rx_buffer[5003]);
    printf("rx_buffer[6001] = %d \n", rx_buffer[6001]);
    printf("rx_buffer[6002] = %d \n", rx_buffer[6002]);
    printf("rx_buffer[6003] = %d \n", rx_buffer[6003]);
    printf("rx_buffer[7001] = %d \n", rx_buffer[7001]);
    printf("rx_buffer[7002] = %d \n", rx_buffer[7002]);
    printf("rx_buffer[7003] = %d \n", rx_buffer[7003]);

    printf("rx_buffer[8001] = %d \n", rx_buffer[8001]);
    printf("rx_buffer[8002] = %d \n", rx_buffer[8002]);
    printf("rx_buffer[8003] = %d \n", rx_buffer[8003]);
    printf("rx_buffer[9001] = %d \n", rx_buffer[9001]);
    printf("rx_buffer[9002] = %d \n", rx_buffer[9002]);
    printf("rx_buffer[9003] = %d \n", rx_buffer[9003]);
    printf("rx_buffer[9901] = %d \n", rx_buffer[9901]);
    printf("rx_buffer[9902] = %d \n", rx_buffer[9902]);
    printf("rx_buffer[9903] = %d \n", rx_buffer[9903]);


    //printf("rx_buffer[19900] = %d \n", rx_buffer[190]);
    //printf("rx_buffer[19900] = %d \n", rx_buffer[191]);
    //printf("rx_buffer[19900] = %d \n", rx_buffer[192]);
    //printf("rx_buffer[19900] = %d \n", rx_buffer[193]);
    //printf("rx_buffer[19900] = %d \n", rx_buffer[194]);
    //printf("rx_buffer[19901] = %d \n", rx_buffer[19901]);
    //printf("rx_buffer[19902] = %d \n", rx_buffer[19902]);
    //printf("rx_buffer[19903] = %d \n", rx_buffer[19903]);
    //printf("rx_buffer[19904] = %d \n", rx_buffer[19904]);
    //printf("rx_buffer[19905] = %d \n", rx_buffer[19905]);
    //printf("rx_buffer[19906] = %d \n", rx_buffer[19906]);
    //printf("rx_buffer[19907] = %d \n", rx_buffer[19907]);
    //printf("rx_buffer[19908] = %d \n", rx_buffer[19908]);
    //printf("rx_buffer[19909] = %d \n", rx_buffer[19909]);
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
