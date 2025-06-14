QGifer
======

## Introduction

QGifer is a tool for extracting part of a video to an animated GIF file.

Since this is a fork and has no issue tracker associated with it, the issues I'm working on will go here:

## Issues

* [ ] Unable to save gif image. Creates a black gif file of 3.4 Kb. Message below may be part of the issue:
```
QObject::killTimer(): Error: timer id -1 is not valid for object 0x55a6d1e2aa80 (FramePlayer, player), timer has not been killed
```

Qt5 Port created by Daniel Kettle <initial.dann@gmail.com>

Area of concern:

* giflib removed the Quantizer code. 
* - https://github.com/mldbai/giflib/blob/master/lib/quantize.c
* - https://bugs.gentoo.org/682198
* - https://github.com/rainycape/magick/issues/36
* - https://sourceforge.net/p/giflib/bugs/142/

## TODO

* [ ] Set the capture range to the entire video
* [ ] Figure out why preview isn't working
* [ ] 

## Original Author License

http://sourceforge.net/projects/qgifer/


    Author:  	Lukasz Chodyla <chodak166@op.pl>
    Author:		Arkadii Ivanov <apkawa@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Niniejszy program jest wolnym oprogramowaniem; możesz go
    rozprowadzać dalej i/lub modyfikować na warunkach Powszechnej
    Licencji Publicznej GNU, wydanej przez Fundację Wolnego
    Oprogramowania - według wersji 3 tej Licencji lub (według twojego
    wyboru) którejś z późniejszych wersji.

    Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
    użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
    gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
    ZASTOSOWAŃ. W celu uzyskania bliższych informacji sięgnij do
    Powszechnej Licencji Publicznej GNU.

    Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
    Powszechnej Licencji Publicznej GNU (GNU General Public License);
    jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
    Place, Fifth Floor, Boston, MA  02110-1301  USA


Requirements
============

 * Qt version 5.3.2, or higher.
 * OpenCV (core, highgui, imgproc) version 4 or higher.
 * giflib version 4.1 or higher?
 * CMake version 3.10 or higher (may work in prior versions)


Compilation and installation
============

    git clone git@github.com:NucleaPeon/QGifer.git
    cd QGifer
    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX="/usr/local/" -DQUIET_MODE=ON
    make
    sudo make install
    

note: use

    cmake .. -G "MinGW Makefiles" -DQUIET_MODE=ON

for Windows with MinGW.

For debugging you can add cmake option *RELEASE_MODE=OFF* 
As example:
    
    cmake .. -DRELEASE_MODE=OFF


Build debian package
=============

    cmake .. -DCMAKE_INSTALL_PREFIX="/usr/local/" -DQUIET_MODE=ON
    make package
    
