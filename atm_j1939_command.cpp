#include "atm_j1939_command.h"

Atm_j1939_command& Atm_j1939_command::begin(ARD1939& j1939)
{
    const static state_t state_table[] PROGMEM = {
        /*              ON_ENTER     ON_LOOP     ON_EXIT  EVT_TIMER     EVT_RECV       ELSE */
        /* WAITING */         -1,         -1,         -1,        PROCESSING,  -1,        -1,
        /* PROCESSING */      ENT_CHECK,  -1,         -1,        -1,          RECEIVING, WAITING,
        /* RECEIVING */       ENT_RECV,   -1,         EXT_RECV,  -1,          -1,        WAITING,
    };
    // clang-format on
    Machine::begin( state_table, ELSE );
    this->_j1939 = &j1939;
    message.nMsgId = J1939_MSG_NONE;
    message.nJ1939Status = J1939_MSG_NONE;
    timer.set(SYSTEM_TIME);
    return *this;
}


int Atm_j1939_command::event( int id ) {
  switch ( id ) {
    case EVT_TIMER:
        // It's time to check for incoming messages
        return timer.expired(this);
    case EVT_RECV:
        // We got a message.
        return (
            (message.nJ1939Status != J1939_MSG_NONE) && 
            (message.nMsgId == J1939_MSG_APP)
        ) ? 1 : 0;
  }
  return 0;
}

void Atm_j1939_command::action( int id ) {
    switch ( id ) {
        case ENT_CHECK:
            // Check for incoming J1939 message
            message.nJ1939Status = _j1939->Operate(
                &message.nMsgId,
                &message.lPGN,
                &message.pMsg[0],
                &message.nMsgLen,
                &message.nDestAddr,
                &message.nSrcAddr,
                &message.nPriority
            );
            return;
        case ENT_RECV:
            // Handle received J1939 message
            incoming.push();
            return;
        case EXT_RECV:
            // We've handled the received packet
            message.nMsgId = J1939_MSG_NONE;
            message.nJ1939Status = J1939_MSG_NONE;
            message.nMsgLen = 0;
            return;
    }
}

Atm_j1939_command& Atm_j1939_command::onMessage(atm_cb_push_t callback)
{
    incoming.set(callback, 0);
}

Atm_j1939_message Atm_j1939_command::getMessage(void)
{
    return message;
}