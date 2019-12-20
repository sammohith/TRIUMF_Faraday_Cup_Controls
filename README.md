# TRIUMF_Faraday_Cup_Controls
Contains the .ino and .htm files for the GRIFFIN movable faraday cup control system.

.htm file goes on SD card, .ino goes in the Arduino IDE editor.

Instructions for uploading code and html to arduino.

To change the .ino sketch and upload to the Arduino:

1) Go to https://www.arduino.cc/en/Main/Software and download the Arduino IDE, I was using ver 1.8.10, but as far as I know any
   later version should also be OK.
2) Copy the contents of the .ino file into the IDE editor. Connect computer to Arduino via USB-to-Printer cable and go to the IDE options
   select the correct board. (Arduino Mega 2560 in this case)
3) Click Upload and wait.
4) Open serial monitor. If the SD card read is successful (which it ideally should be), then wait till it prints the server IP on the serial
   monitor.
5) Copy the IP onto your browser, and voila.

-- The above steps only need to be done whenever the source code for the Arduino is modified. If instead you only change the SD card
   contents, just put the SD card back in the slot and hit the little black reset button on the Arduino shield and wait for a couple 
   minutes before going to the IP on your browser.
   
To edit contents of SD card:

1) Remove SD card from its slot. Put it in a card reader and plug it into computer. Change whatever you need to. Plug it back in. Recompile
   .ino file just to be safe. You should be good to go. Sometimes, the Serial monitor will say it can't read the SD card. Just let it sit
   for a couple of hours and reupload the code.
   
To replace the SD card:

1) If using a new SD card for some reason, make sure to format it to FAT32 (that's a specific encoding I think. Ethernet boards only read 
   FAT16 and FAT32 formats, with FAT32 being the recommended one).

HAPPY TINKERING :)
