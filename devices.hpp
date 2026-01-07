#ifndef MOUSE_H
#define MOUSE_H

#include <cerrno>
#include <string>
#include <sstream>
#include <vector>
#include <fstream> 
#include <iostream>
#include <linux/input-event-codes.h>
#include <sys/epoll.h> // for epoll 
#include <sys/time.h> 
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <linux/input.h> // for struct input_event, event codes
 
constexpr const int MAX_EVENTS = 15;

std::string findPathByFile(const std::string& target)
{
    std::ifstream file("/proc/bus/input/devices");
    std::string line, path; 
    bool finded = false;

    while (std::getline(file, line))
    {
        if (line.find("N: Name=") != std::string::npos && line.find(target) != std::string::npos)
        { finded = true; }

        if (finded && line.find("H: Handlers= ") != std::string::npos)
        {
            std::stringstream ss(line);
            std::string word;
            while (ss >> word)
            {
                if (word.find("event") != std::string::npos)
                {   
                    std::string basePath = "/dev/input/";
                    return basePath + word; 
                }
            }
        }
        if (line.empty()) { finded = false; }
    }
    return "";
}

class InputDevice
{
    public:
        virtual ~InputDevice()
        {
            if (m_fd >= 0)
            {
                close(m_fd);
            }
        }

        virtual void handleEvent() = 0;

        int getFd() const { return m_fd; }

    protected:
        int m_fd = -1;
        const char* path; 
};

struct Keyboard : public InputDevice
{
    Keyboard(const char* keyboardDevicePath)
    {
        m_keyboardListener = open(keyboardDevicePath, O_RDONLY, 0);
        m_fd = m_keyboardListener;
        if (m_keyboardListener < 0)
        {
            std::cerr << "Unable to instantiate keyboard file descriptor: " << errno << '\n';
            exit(1);
        }
    }

    void handleEvent() override
    {
        std::cout << "keyboard event handler \n";
        ssize_t keyboardBytesRead = read(this->m_fd, &m_inputEvent, sizeof(m_inputEvent));
        if (keyboardBytesRead < 0)
        {
            std::cerr << "Error while fetching keyboard data: " << errno << '\n';
            exit(1);
        }
        std::cout << "Event type: " << m_inputEvent.type << std::endl;
        std::cout << "Code: " << m_inputEvent.code << std::endl;
        std::cout << "Value: " << m_inputEvent.value << std::endl;
    }

    private:
        int m_keyboardListener {}; 
        struct input_event m_inputEvent {};
};

struct Mouse : public InputDevice 
{
    Mouse(const char* mouseDevicePath)
    {
        m_mouseListener = open(mouseDevicePath, O_RDONLY, 0);
        m_fd = m_mouseListener;
        if (m_mouseListener < 0)
        {
           std::cerr << "Unable to instantiate mouse file descriptor: " << errno << '\n';
           exit(1);
        }
    }

    void handleEvent() override 
    {
        ssize_t mouseBytesRead = read(this->m_fd, &m_inputEvent, sizeof(m_inputEvent));
        if (mouseBytesRead < 0)
        {
            std::cerr << "Error while fetching mouse data: " << errno << '\n';
            exit(1);
        }

        if (m_inputEvent.type == EV_REL)
        {    
            if (m_inputEvent.code == REL_X)
            {
                m_mouse_x += m_inputEvent.value;
            }
            else if (m_inputEvent.code == REL_Y)
            {
                m_mouse_y += m_inputEvent.value;
            }
            std::cout << "[MOUSE POS (X, Y)]" << '(' << m_mouse_x << ", " << m_mouse_y << ")\n";
            exit(1);
        }

    }

    private:
        struct input_event m_inputEvent {}; 
        int m_mouseListener {};
        int m_mouse_x {};
        int m_mouse_y {}; 
};


class Listener 
{
    public:
        Listener()
        {
            m_epoll = epoll_create(2);
            if (m_epoll < 0)
            {
                std::cerr << "error while creating epoll: " << errno << '\n';
                exit(1);
            }
        }

        ~Listener()
        {
            if (m_epoll != -1)
            {
                close(m_epoll);
            }
        }

        // listens for a fd 
        void bind(InputDevice& device)
        {
            events.resize(MAX_EVENTS);

            struct epoll_event event {}; 
            event.data.ptr = &device; 
            event.events = EPOLLIN;
            
            m_epollController = epoll_ctl(m_epoll, EPOLL_CTL_ADD, device.getFd(), &event); 
            if (m_epollController < 0)
            {
                std::cerr << "Error in epoll controller: " << errno << '\n';
                exit(1);
            }
        }

        void run()
        {
            while (true)
            {
                int epollCheck = epoll_wait(m_epoll, events.data(), MAX_EVENTS, -1);
                if (epollCheck < 0)
                {
                    std::cerr << "epoll wait error: " << errno << '\n';
                    exit(1);
                }
                for(int i {}; i < epollCheck; ++i)
                {
                    std::cout << "for loop\n";
                    InputDevice *pdevice = static_cast<InputDevice*>(events.at(i).data.ptr);
                    pdevice->handleEvent();
                }
            }
        }

    private:
        int m_epoll {};
        int m_epollController {};
        std::vector<struct epoll_event> events;

};
#endif
