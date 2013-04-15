Groovuino
=========

Sound library for Arduino Due to make a groovebox

To use this library with an Arduino Due, you have to plug the DAC1 out of the Arduino to the line IN of an amplifier. 
You can put a 500 Ohm resistor between this out and your amplifier to prevent harming your Arduino.
I made a shield, which is used in analogread.h and interface.h, to control Arduino.
You can build this shield (see on my blog below the schematics) and use it, but you can use this other components of the library without this shield.


You can use too a MIDI shield and the arduino MIDI library to control your Arduino : http://playground.arduino.cc/Main/MIDILibrary


If you want to use a sampler, the samples are stored in a SD card, and Groovuino needs SdFat library. You can find it here :
http://code.google.com/p/sdfatlib/


Thanks to Duane B, which blog permitted to start this library :
http://rcarduino.blogspot.com/2012/12/arduino-due-dds-part-1-sinewaves-and.html


Details on library and hardware are on my blog :
http://groovuino.blogspot.com/2013/03/the-goal-is-to-build-groovebox-using.html
