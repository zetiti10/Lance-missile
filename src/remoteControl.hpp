#ifndef REMOTE_CONTROL_DEFINITIONS
#define REMOTE_CONTROL_DEFINITIONS

void processMessage(String receivedMessage, UartClass *UART);
void receivedData(UartClass *UART);
void checkMessages();

#endif