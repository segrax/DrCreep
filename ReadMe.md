The Castles of Dr. Creep
------------------------

[![Build Status](https://travis-ci.org/segrax/DrCreep.svg?branch=master)](https://travis-ci.org/segrax/DrCreep)

####About

 The Castles of Dr. Creep is an open source reimplementation of the 
  original 1984 Commodore 64 game by Ed Hobbs.
 Now with a built in castle Builder, allowing the player to create their
  own castles and build onto an existing castle.


Enjoy,
  
Robert Crossfield <robcrossfield@gmail.com>
<br>
####History
 
" As a programmer, I appreciate the difficulty of reverse-engineering the 6502 object code to C++.  
 When I wrote the game, the C-64 assembler was very primitive (it couldn’t handle programs larger than 16 kb), 
 so I had to write and assemble the source code on an Apple II, and then transfer the binary to the C-64 through cables
 connected to the joystick ports of the respective computers (my first experience with a “computer network”).
 So even though the game could not be executed on the Apple II, this computer still played a vital part in Dr. Creep’s
 creation.
 
 One of the biggest problems was that the source code had to fit onto a single 144 kb floppy disk in order to work 
 with the Apple 6502 assembler.  The only way to do this was by eliminating all comments in the source code—there 
 just wasn't any room for them.  I maintained a notebook comprising hundreds of pages of flowcharts and data-layout
 diagrams.  The nodes of the flowcharts were labeled to match the nametags in the source code.  In this way, I was
 able to visually grasp what was going on in the code, and still fit it into the requisite space.

 Furthermore, there was no such thing as a C-64 debugger, which added an additional twist to the programming.
 
 I also didn’t have a level editor.  
 I drew the levels out on custom graph paper, and then used these drawings to hand-code the level data." 
 
- Ed Hobbs 2012
<br>
####Required To Play
 Commodore 64 Disk Image (D64) of "The Castles of Dr. Creep", 
  which should be placed inside the game 'data' folder. 
 
 This file may have any name, but the 'D64' extension must remain.


####Whats Added
 *Castle Builder: Editing of existing castles, or creation of new ones


####Whats Broken
 *sound is not perfect
 


####Save Game Support

 D64s will be created in the data\save folder, and all save games will be stored in these files.
 These files are compatible with the original Commodore64 game, and in return the original save games are supported.


####Wii Version

#####Required
 
 SD Card with folder '/apps/drcreep/data' containing your Dr.Creep D64 and CHAR.ROM
 


####Controls
    Hold WiiMote on its side,
    
#####In-Game
    DPAD 	= Movement
    Button 2 	= Fire Button
    Button A 	= Self Kill

#####Intro	
    Button Plus = Option Menu
    Button 1 	= HighScores
    

####Game Controls

#####General
    F4  = Start Builder using current castle
    F5  = Start Builder with empty castle
    F6  = Enable Player 1 Joystick 
    F7  = Enable Player 2 Joystick
    F10 = Fullscreen Toggle

#####Intro
    F1 = Options
    F3 = Display Highscores
 

#####In Map Screen
    F1 = Save Game
    F2 = Load Game


#####In Game
    Escape = Self Kill
    F1  = Restore Key (Pause/Resume)

######Player 1
    Right Ctrl = Button
    Spacebar   = Button  (MACOSX Version)
    Arrows = Left/Right/Up/Down   (NOT the numpad)

######Player 2
    Numpad 0 = Button
    Numpad Arrows = Left/Right/Up/Down



####Builder Controls

#####General
    Escape = Quit Builder

    F2  = Save Castle to empty 'Castle Disk'
    F3  = Edit Map Screen

    F4  = Return from Builder, in test castle mode

#####Objects
    q = Cursor Create Object (Rotate Up)
    a = Cursor Create Object (Rotate Down)

    [ = Rotate up through Placed Objects
    ] = Rotate down through placed objects

    l = Link Object

    - = Previous Room
    = = Next Room
  
    f = Switch to exit room

    s = Change object state

    c = Change Key Color Down
    v = Change Key COlor Up

    'Delete' = Delete selected 'placed' object

    Cursor Keys move the "red" square pointer

    Ctrl = Place Cursor Object, Select object to link to


#####Map Screen
    - = Previous Room
    = = Next Room

    [ = Rotate up through doors in this room
    ] = Rotate down through doors in this room

    k = Increase Door X on map
    = Increase Door Y on map
    d = Change Door Entry Direction

    ; = Decrease Room Width
    ' = Increase Room width

    . = Decrease Room Height
    / = Increase Room Height

    c = Change room color

    Cursor Keys = Move Room 


####Playing

#####From the Intro
    To start a 1 player game, press the Player1 Button
    To start a 2 player game, press the Player2 Button


#####Command Line Parameters
    -u    : Unlimited Lives
    -c    : Display Console
    -l xx : Start Castle number 'xx'


Thanks:
-------

Ed Hobbs, for the original game

Christian Bauer, for his extremely detailed 'VIC-II' documentation (https://sh.scs-trc.net/vic/)

Dag Lem, for 'reSid' the MOS6581 SID emulation library (http://en.wikipedia.org/wiki/ReSID)

Peter Schepers and the authors of the documents contained in his collection (http://ist.uwaterloo.ca/~schepers/formats.html), for the detailed disk format information

Thomas Bätzler, for his website 'Castles of Dr.Creep Revisited' (http://www.baetzler.de/c64/games/creep/)
