// First set up the ARD1939 library
#include <stdlib.h>
#include <inttypes.h>
#include <SPI.h>
#include <ARD1939.h>
ARD1939 j1939;

// Include the Automaton libraries
#include <Automaton.h>
#include <atm_j1939_command.h>

// Create our Automaton machines
Atm_j1939_command j1939Command;
Atm_digital inputSwitch;

// Application settings
#define J1939_ADDRESS 130
#define SWITCH_PIN 5

// Turn on/off debug messages
#define DEBUG_J1939 1

void setup() {

#if DEBUG_J1939 == 1
    Serial.begin(115200);
#endif

    // Initialise ARD1939 - note that most configuration happens in ARD1939.h
    if (j1939.Init(SYSTEM_TIME) == 0) {
#if DEBUG_J1939 == 1
        Serial.print("CAN Controller Init OK.\n\r\n\r");
    } else {
        Serial.print("CAN Controller Init Failed.\n\r");
#endif
    }
      
    // Set the node address and address range
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

    // Start the j1939 command machine - this is needed to periodically call
    // J1939->Operate(), which performs the startup address claim procedure
    // and handles other protocol-specific stuff
    j1939Command.begin(j1939);

    // Set up our switch to send J1939 packets on state change
    inputSwitch
    .onChange( [] (int idx, int v, int up) {
        // Send a J1939 message
        // Note that PGNs should be in hex - doesn't work with int or long for some reason
        byte message[] = {v, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        j1939.Transmit(
            6, // priority = 0-7
            0x400, // PGN
            j1939.GetSourceAddress(), // source address
            255, // dest address - 255 = broadcast
            message, // data
            8 // data length
        );

#if DEBUG_J1939 == 1
        Serial.print("Sent ");
        Serial.println(v);
#endif

    })
    .begin(SWITCH_PIN, 20, true, true);
}

void loop() {
    automaton.run();
}
