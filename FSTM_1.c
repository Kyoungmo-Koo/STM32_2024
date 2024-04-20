#include <iostream>
#include <conio.h> // For getch() and kbhit()
#include <io.h>     // For _setmode()
#include <fcntl.h>  // For _O_BINARY
#include <windows.h>
//idle = 0
//raster = 1
//radial = 2
//stop = ESC
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
            else if (ch == 48)
                state = 0;
            else if (ch == 49)
                state = 1;
            else if (ch == 50)
                state = 2;
        }
        Sleep(1000);
        std::cout << "State: " << state << std::endl;
    }


    return 0;
}
