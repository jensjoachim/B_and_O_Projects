# B_and_O_Projects
Different hw projects for B&amp;O amps. Tested on Beomaster 4500
The projects are Arduino based

There is two projects and a support folder:
1. ir_read
  This Arduino projects is used to received B&O ir-codes. The data pin of a TSOP7000 or a B&O ir-reciver to pin 2 or 3 of the Arduino. These pins are the external interrupt pins.
2. sound_detect
  This project is connected between the arduio link interface of the Beomaster4500 and a AUX audio source. The device tests the audio input to detect if any. Then it sends serial commands to the BM4500 to either turn on or into standby. The on commands sets the BM into "Tape 2" source input. After being on without any music coming from the external audio source in approx one minute, the device will send the standby command.
3. support
  This folder contains a logic analyzer to record the audio link interface serial commands and a command generator which can generate the commands. The commands are sent in bytes and a bit it sent every 3126us. The logic analyzer encodes the commands in a format where 0x00xx means a data byte and 0x1xxx means the number of wait time between the bytes.
  
