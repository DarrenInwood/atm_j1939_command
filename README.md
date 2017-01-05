# Automaton J1939 command machine

A state machine that processes incoming J1939 protocol packets via the CAN bus, 
and fires a handler callback for packets intended for the current node.

## Requirements

* Arduino (https://www.arduino.cc/)
* Automaton (https://github.com/tinkerspy/Automaton)
* The ARD1939 library (http://copperhilltech.com/ard1939-sae-j1939-protocol-stack-for-aduino/)

The ARD1939 library this uses unfortunately only works on Arduino Uno and Arduno Mega 2560, 
so this can only be used with those processors.

## Installation

Make sure you have installed both the Automaton and ARD1939 libraries into your Arduino
`libraries` folder, or installed them from zipfiles via the Arduino IDE.

Save the atm_j1939_command folder into Arduino/libraries folder, or install from zip.

## Usage

Even if your application is only sending J1939 packets, you should use this machine to 
handle the address claim procedure and other protocol-related incoming packets.

Examples of how to send and receive J1939 PGN messages can be found in the `examples` folder.

## Caveats

You may wish to edit ARD1939.h in the ARD1939 folder, and change the value of CS_PIN to 
match your hardware.

Also consider whether you need message lengths more than 8 bytes.  If you don't, change 
ARD1939VERSION to 0 in ARD1939.h to save ~4k program memory and ~850 bytes of SRAM. 
(Hint: you probably should do this unless you specifically know otherwise)

The ARD1939 library doesn't seem to handle auto-negotiation of addresses too well, so
you should ensure your nodes all have unique preferred addresses.  This may be to do with
how the SetAddressRange and SetPreferredAddress methods work together.

You should use hexadecimal PGN values in your code.  The ARD1939 Transmit function seems to 
send incorrect PGN values when using decimal PGNs.





