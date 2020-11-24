# MPU9250_legacy

- MPU9250.h and MPU9250.c were taken from https://github.com/sparkfun/SparkFun_MPU-9250_Breakout_Arduino_Library

- Everything except the initialization routine is likely the exact same as what is seen in the sparkfun library. 

- The main script
  - Setup - Initializes devices
  - Loop - Read Accel, Store in buffer(teeny tiny buffer), Write to SD. 

- configs excel file is something i made long time ago that helped me keep track of my initialization routine based on what i learned from the data sheet. I don't remember the specifics of it anymore


I've left the print statements in there. There isn't anything elegant about this code :) 
