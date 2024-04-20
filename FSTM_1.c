#include <iostream>
#include <conio.h> // For getch() and kbhit()
#include <io.h>     // For _setmode()
#include <fcntl.h>  // For _O_BINARY
#include <windows.h>
//idle = 0
//raster = 1
//radial = 2
//stop = 3
int state = 0;

int main() {
    // Enable non-blocking mode for keyboard input
    _setmode(_fileno(stdin), _O_BINARY);

    while (true) {
        // Check if a key is pressed
        if (_kbhit()) {
            // Get the character without blocking
            char ch = _getch();

            // Print the character
            std::cout << "Key pressed: " << ch << std::endl;

            // If the ESC key is pressed, break out of the loop
            if (ch == 27) // ASCII code for ESC key
                break;
        }
        Sleep(1000);
        printf("hi");
    }


    return 0;
}
