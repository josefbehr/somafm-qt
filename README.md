SomaFM Radio Player (somafm-qt)
===============================
written in C++ and QT5

Features
--------
It does the basics:
* Show the channel list
* Double-click a channel to play it
* Rudimentary player control
* Manage favorite channels

![Screenshot](https://raw.githubusercontent.com/josefbehr/somafm-qt/master/screenshot.png "Screenshot")

### ToDo
* Song history of current channel
* Settings tab
* Some error handling
* (Use official API)
* Tray icon

Build and Install
-----------------
### Linux
Clone the git repository:
    git clone https://github.com/josefbehr/somafm-qt.git

Make sure to have installed the qt5 development headers as well as the
network and multimedia components. Then build using qmake:
    cd somafm-qt
    qmake
    make

If everything works, you'll get a binary called somafm-qt, which you can run:
    ./somafm-qt

### Windows and MacOS
The principle steps here should be the same as above, but if you can provide
specifics to include here, please share and I'll add the information.

Bugs and Problems
-----------------
This program is an early work in progress, and if you encounter any
problems or bugs, please feel free to report them on the github issure
tracker.
