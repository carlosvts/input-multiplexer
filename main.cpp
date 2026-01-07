#include "mouse.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        std::cout << "Usage: " << argv[0] << '\n';
        std::cout << "No parameters or flags required.\n";
        exit(0);
    }    
    
    Listener listener;
    Mouse mouse(MOUSE_DEVICE_PATH);

    listener.bind(mouse);
    
    listener.run();
}

