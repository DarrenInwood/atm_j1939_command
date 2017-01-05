#ifndef ATM_J1939_COMMAND_H
#define ATM_J1939_COMMAND_H

#include <Arduino.h>
#include <Automaton.h>

#include <stdlib.h>
#include <inttypes.h>
#include <SPI.h>

#include <ARD1939.h>

/**
 * An Automaton machine that enables J1939 connectivity for an application.
 *
 * The machine reacts to incoming J1939 packets, and enables emitting EVT_RECV events
 * in response to particular PGNs via a pub/sub mechanism.
 */

struct Atm_j1939_message {
	byte nMsgId;
	byte nDestAddr;
	byte nSrcAddr;
	byte nPriority;
	byte nJ1939Status;
	int nMsgLen;
	long lPGN;
	byte pMsg[J1939_MSGLEN];
};


class Atm_j1939_command : public Machine
{
	public:
		enum { WAITING, PROCESSING, RECEIVING }; // STATES
		enum { EVT_TIMER, EVT_RECV, ELSE }; // EVENTS

		// Constructor
		Atm_j1939_command(void) : Machine(){};
		// Start
		Atm_j1939_command& begin(ARD1939& j1939);
		// Set event handler callback
		Atm_j1939_command& onMessage(atm_cb_push_t callback);

		// Timer to fire J1939::Operate every 1ms
		atm_timer_millis timer;

		// Get the current J1939 message
		Atm_j1939_message getMessage(void);

	private:
		enum { ENT_CHECK, ENT_RECV, EXT_RECV }; // ACTIONS
		atm_connector incoming; // CONNECTORS

		ARD1939* _j1939;

		// Current J1939 message data
		Atm_j1939_message message;

		int event( int id );
		void action( int id );
};

#endif
