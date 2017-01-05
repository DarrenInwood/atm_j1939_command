// First set up the ARD1939 library
#include <stdlib.h>
#include <inttypes.h>
#include <SPI.h>
#include <ARD1939.h>
ARD1939 j1939;

// Include the Automaton libraries
#include <Automaton.h>
#include <atm_j1939_command.hpp>

// Create our Automaton machines
Atm_j1939_command j1939Command;
Atm_led led;

// Application settings
#define J1939_ADDRESS 131
#define LED_PIN 5

// Turn on/off debug messages
#define DEBUG_J1939 1

void setup() {

#if DEBUG_J1939 == 1
    Serial.begin(115200);
#endif

    // Initialise our status LED
    led.begin(LED_PIN).blink(40, 100);

    // Initialise ARD1939 - note that most configuration happens in ARD1939.h
    if (j1939.Init(SYSTEM_TIME) == 0) {
#if DEBUG_J1939 == 1
        Serial.print("CAN Controller Init OK.\n\r\n\r");
    } else {
        Serial.print("CAN Controller Init Failed.\n\r");
#endif
    }
      
    // Set the preferred address and address range
    // Note, here would be the place to add filters also
    j1939.SetPreferredAddress(J1939_ADDRESS);
    j1939.SetAddressRange(ADDRESSRANGEBOTTOM, ADDRESSRANGETOP);
   
    // Set the NAME
    j1939.SetNAME(NAME_IDENTITY_NUMBER,
        NAME_MANUFACTURER_CODE,
        NAME_FUNCTION_INSTANCE,
        NAME_ECU_INSTANCE,
        NAME_FUNCTION,
        NAME_VEHICLE_SYSTEM,
        NAME_VEHICLE_SYSTEM_INSTANCE,
        NAME_INDUSTRY_GROUP,
        NAME_ARBITRARY_ADDRESS_CAPABLE
    );

    // Start the command machine
    j1939Command
    .begin(j1939)
    // Only messages that either have Destination Address matching this node
    // or are addressed to the Broadcast address will trigger this callback.
    .onMessage( [] (int idx, int v, int up) {
        // Make a copy of the incoming J1939 message
        Atm_j1939_message message = j1939Command.getMessage();
        // Do something with it depending on the PGN
        // Note that PGNs should be in hex - doesn't work with int or long for some reason
        switch (message.lPGN) {
            case 0x400:
                if (message.pMsg[0] == 0) {
                    led.trigger( led.EVT_OFF );
                } else {
                    led.trigger( led.EVT_ON );
                }
#if DEBUG_J1939 == 1
                Serial.print("Received ");
                Serial.println(message.pMsg[0]);
                break;
            default:
                Serial.print("Got another PGN: ");
                Serial.println(message.lPGN);
#endif
                break;
        }
    });
}

void loop() {
    automaton.run();
}
