#include <windows.h>
#include <stdio.h>
#include <time.h>

int main() {
    HANDLE hComm;
    BOOL status;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytesRead;
    DWORD bytesWrite;
    UCHAR Buffer[4] = {0, 0, 0, 0};
    UCHAR RxBuffer[4] = { 1, 2, 3, 4 };
    DWORD dwEventMask;

    clock_t start, end;
    double time_used;

    // Open the serial port
    hComm = CreateFile(L"\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error in opening serial port\n");
        return 1;
    }
    else {
        printf("Opening serial port successful\n");
    }
    status = PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);
    if (!status) {
        printf("Error in purging comm\n");
        CloseHandle(hComm);
        return 1;
    }
    printf("Data received: %d\n", Buffer[0]);
    printf("Data received: %d\n", Buffer[1]);

    // Set device parameters (9600 baud, 1 start bit, 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    status = GetCommState(hComm, &dcbSerialParams);
    if (!status) {
        printf("Error in GetCommState\n");
        CloseHandle(hComm);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Set baud rate to 9600 (you can change this)
    dcbSerialParams.ByteSize = 8;         // Data bit = 8
    dcbSerialParams.StopBits = ONESTOPBIT; // One stop bit
    dcbSerialParams.Parity = NOPARITY;     // No parity
    status = SetCommState(hComm, &dcbSerialParams);

    if (!status) {
        printf("Error in setting DCB structure\n");
        CloseHandle(hComm);
        return 1;
    }


    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 10; 
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 10;
    if (!SetCommTimeouts(hComm, &timeouts)) {
        printf("Error in setting timeouts\n");
        CloseHandle(hComm);
        return 1;
    }
    while (1) {
        start = clock();
        status = WriteFile(hComm, &RxBuffer[0], 1, &bytesRead, NULL);
        status = ReadFile(hComm, &Buffer[0], 1, &bytesWrite, NULL);
        Sleep(1);
        end = clock();
        printf("Bytes received: %d\n", bytesRead);
        printf("Data received: %d\n", Buffer[0]);
        printf("Data received: %d\n", Buffer[1]);
        time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time taken : %f seconds \n", time_used);
    }
    if (!status) {
        printf("Error in reading\n");
        CloseHandle(hComm);
        return 1;
    }

    printf("Bytes received: %d\n", bytesRead);
    printf("Data received: %d\n", Buffer[0]);
    printf("Data received: %d\n", Buffer[1]);
    printf("Data received: %d\n", Buffer[2]);
    printf("Data received: %d\n", Buffer[3]);

    // Close the serial port
    CloseHandle(hComm);

    return 0;
}
