#include "devices.hpp"
int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: sudo " << argv[0] << " <mouse-path> <keyboard-path>"<< std::endl;
        std::cout << "Example: sudo input-listener /dev/input/event10 /dev/input/event16.\n";
        exit(1);
    }    
    
    // /dev/input/event from user input
    std::string MOUSE_DEVICE_PATH = argv[1]; 
    std::string KEYBOARD_DEVICE_PATH = argv[2];
       
    Listener listener;
    Mouse mouse(MOUSE_DEVICE_PATH.c_str());
    Keyboard keyboard(KEYBOARD_DEVICE_PATH.c_str());

    listener.bind(mouse);
    listener.bind(keyboard);

    listener.run();
}

