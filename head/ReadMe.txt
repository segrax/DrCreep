The Castles of Dr. Creep
------------------------


About:
------
 The Castles of Dr. Creep is an open source reimplementation of the 
  original 1984 Commodore 64 game by Ed Hobbs.
 Now with a built in castle Builder, allowing the player to create their
  own castles and build onto an existing castle.


Required To Play:
----------------- 
 Commodore 64 Disk Image (D64) of "The Castles of Dr. Creep", 
  which should be placed inside the game 'data' folder. 
 
 This file may have any name, but the 'D64' extension must remain.


Whats Added:
------------
 *Castle Builder: Editing of existing castles, or creation of new ones


Whats Missing:
--------------
  Nothing!


Whats Broken:
-------------
 *occasional lock-up, caused by walking, or mummy/franky walking
 *sound is not perfect
 *graphic glitch caused by floor-switch toggling
 


Save Game Support:
------------------
 D64s will be created in the data\save folder, and all save games will be stored in these files.
 These files are compatible with the original Commodore64 game, and in return the original save games are supported.



Wii Version:
------------

 Required:
 ---------
 
 SD Card with folder '/apps/drcreep/data' containing your Dr.Creep D64 and CHAR.ROM
 


 Controls:
 ---------
 Hold WiiMote on its side,

  In-Game:
    DPAD 	= Movement
    Button 2 	= Fire Button
    Button A 	= Self Kill

  Intro:	
    Button Plus = Option Menu
    Button 1 	= HighScores




Game Controls:
--------------

 General:
 --------

  F4  = Start Builder using current castle
  F5  = Start Builder with empty castle
  F10 = Fullscreen Toggle

 Intro:
 ------

  F1 = Options
  F3 = Display Highscores
 

 In Map Screen:
 --------------

  F1 = Save Game
  F2 = Load Game


 In Game:
 --------

  Escape = Self Kill
  F1  = Restore Key (Pause/Resume)



   Player 1:
   ---------

     Right Ctrl = Button 
     Arrows = Left/Right/Up/Down   (NOT the numpad)


   Player 2:
   ---------
 
     Numpad 0 = Button
     Numpad Arrows = Left/Right/Up/Down





Builder Controls:
-----------------

 General:
 --------
  Escape = Quit Builder

  F2  = Save Castle to empty 'Castle Disk'
  F3  = Edit Map Screen

  F4  = Return from Builder, in test castle mode

 Objects:
 --------
 
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


 Map Screen:
 -----------

  - = Previous Room
  = = Next Room

  [ = Decrease Width
  ] = Increase Width
  ; = Decrease Height
  ' = Increase Height

  Cursor Keys = Move Room

    
  
Playing:
--------

 From the Intro,

  To start a 1 player game, press the Player1 Button
  To start a 2 player game, press the Player2 Button


Command Line Parameters:
------------------------

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
