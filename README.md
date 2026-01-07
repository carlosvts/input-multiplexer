# Linux Input Multiplexer (Epoll-based)

A high-performance asynchronous input listener developed in C++ using the Linux `epoll` API.

This project interacts directly with the Linux Kernel's input subsystem, utilizing `sys/epoll.h` to monitor multiple hardware device nodes (`/dev/input/event`) simultaneously within a single-threaded event loop.

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

---

## About the Project
A low-level systems programming project designed to explore I/O multiplexing, kernel-to-userspace communication, and the Linux input protocol. This multiplexer interfaces directly with raw device file descriptors to process hardware interrupts as structured events.

The engine implements a polymorphic architecture where different input devices (Mouse, Keyboard) inherit from a base abstraction, allowing a unified `Listener` kernel to dispatch events based on real-time hardware activity.

**Main Objectives:**
* Implement an **Asynchronous Listener** using the Linux **epoll** syscall.
* Understand more about system calls and the linux architecture.
* Use inheritance.

The architecture separates the hardware-specific parsing logic from the event-monitoring kernel, providing a clean interface for adding new types of input devices.

## Technologies

* **Language:** ISO C++17
* **API:** Linux Kernel Input Subsystem (`linux/input.h`), Epoll (`sys/epoll.h`)
* **Compiler:** g++

## How to Build and Run

### Prerequisites

Ensure you are running a Linux environment. Accessing `/dev/input/` nodes requires **root privileges**.

#### Compiling

To compile the project, run:

```bash
g++ main.cpp -o multiplexer
```

#### Running

Identify your device paths (e.g., via `/proc/bus/input/devices`) and run with `sudo`:

```bash
sudo ./multiplexer /dev/input/eventX /dev/input/eventY
```

---

## Input Devices & Event Handling

### Mouse (Relative Pointer)
* **Behavior**: Intercepts `EV_REL` (relative) event types.
* **Logic**: Calculates displacement by tracking `REL_X` and `REL_Y` axis values.

### Keyboard (Key Events)
* **Behavior**: Intercepts `EV_KEY` event types.
* **Logic**: Distinguishes between key presses (`value: 1`), releases (`value: 0`), and autorepeat (`value: 2`).
* **Interaction**: Supports mapping specific `KEY_` codes (e.g., W, A, S, D, CapsLock) for custom logic.

---

## Project Structure

```Markdown
📁 Project
├── 📄 devices.hpp      # Base Class and inherit classes (Mouse and Keyboard)
├── 📄 main.cpp         # Entry point: Device instantiation and listener binding
└── 📄 .gitignore       # Prevents tracking of binaries
```

## Resources
Key resources used for understanding the Linux Kernel API, I/O multiplexing, and memory alignment:

* **Man epoll(7)**: [Linux manual page for I/O event notification](https://man7.org/linux/man-pages/man7/epoll.7.html) (and other man-pages)
* **Linux Input Documentation**: [The Linux Input Subsystem](https://www.kernel.org/doc/html/latest/input/input.html)
* **The Cherno - C++ Unions**: [Understanding memory overlap](https://www.youtube.com/watch?v=6uqU9Y578n4)
* **Daniel Hirsch**: [Coding a Mouse Tracker in C](https://www.youtube.com/watch?v=B_tHQMxuej8) (Inspiration to make this project came from this video)

--- 
## Reminder
This project was developed for educational purposes to master Linux system calls, file descriptors, and low-level memory management. While higher-level libraries (like SDL or Raylib [or SFML at cpp) abstract these details (use one them or other great library), the goal here was to "suffer" through the manual implementation to understand how data flows from the hardware to the application.

