#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hComm;
    BOOL status;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytesRead;
    UCHAR Buffer[5] = {0, 0, 0, 0, 0 };
    DWORD dwEventMask;

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
    printf("Data received: %d\n", Buffer[2]);
    printf("Data received: %d\n", Buffer[3]);
    printf("Data received: %d\n", Buffer[4]);

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

    // Set timeouts
    // Setting the ReadIntervalTimeout to MAXDWORD to ensure the ReadFile function waits for the arrival of the first byte
    timeouts.ReadIntervalTimeout = MAXDWORD;

    // Setting the ReadTotalTimeoutMultiplier to 0 to ensure the total read timeout is not dependent on the number of bytes read
    timeouts.ReadTotalTimeoutMultiplier = 10;

    // Setting the ReadTotalTimeoutConstant to 20000 (20 seconds in milliseconds)
    timeouts.ReadTotalTimeoutConstant = 200000; // 20 seconds

    // Write timeouts can be set as required, but for your use case, they seem less relevant
    timeouts.WriteTotalTimeoutMultiplier = 10; // Example value
    timeouts.WriteTotalTimeoutConstant = 50;   // Example value
    if (!SetCommTimeouts(hComm, &timeouts)) {
        printf("Error in setting timeouts\n");
        CloseHandle(hComm);
        return 1;
    }

    // Read data
    if (!SetCommMask(hComm, EV_RXCHAR)) {
        printf("Error in setting CommMask\n");
        return 1;
    }
    status = WaitCommEvent(hComm, &dwEventMask, NULL);
    if (!status) {
        printf("Error in setting WaitCommEvent\n");
        return 1;
    }

    status = ReadFile(hComm, &Buffer, sizeof(Buffer), &bytesRead, NULL);
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
    printf("Data received: %d\n", Buffer[4]);

    // Close the serial port
    CloseHandle(hComm);

    return 0;
}
