#ifndef REMOTE_CONTROL_DEFINITIONS
#define REMOTE_CONTROL_DEFINITIONS

#define END_LINE_CHAR 'e'

void receivedData(UartClass *UART);
void checkMessages();

#endif