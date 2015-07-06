This program simulates an answering machine. There is a client-side program which simulates a caller and a server-side program which is the answering machine.

You control only the caller directly.

There are 6 states:

IDLE: The program start here. Upon answering "y" to calling, the call begins. Transition to CALL state.
CALL: The program keeps on sending "RING" to the server until it receives an acknowledging "AHOY". Then, the caller sends "HELLO" to complete this three-way handshake. Transition to TALK state.
TALK: You may input a message to send to the answering machine with "DATA {Message}". Or, if you would like to hear the last message recorded on the answering machine, send "#". Transition to CHECK state. If you have no data to send and you don't want to hear the message, input "n" to initiate an end to the call. Transition to HANGUP state.
CHECK: You must input a one-digit code to access the recorded message. The code is 7. You'll then transition to the TRY state. 
TRY: If the server sends a "MSG", you'll get the message. Transition back to TALK state. If the server sends a "FAIL", you'll automatically transition to HANGUP state.
HANGUP: Once the server sends a "LATER", the client will finish the 2nd three-way handshake by sending a "BYE" to the server. Transition to IDLE.

To compile, use "make". Then, run the ansmach-server on one terminal. Run the ansmach-client on another terminal.

