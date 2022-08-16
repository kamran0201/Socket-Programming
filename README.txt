--Compiling sender.c
gcc sender.c -o sender

--Running sender
sender <SenderPort> <ReceiverPort> <RetransmissionTimer> <NoOfPacketsToBeSent>

--Compiling receiver.c
gcc receiver.c -o receiver

--Running receiver
receiver <ReceiverPort> <SenderPort> <PacketDropProbability>

--Description of sender.c
>> Takes the command line input and stores them in variables
>> Socket creation and binding is done
>> File sender.txt is opened to write over it
>> i-th packet is sent and printed on terminal and txt file
>> Sending time is recorded and retransmission timer is set
>> Packet is retransmitted in case of no ACK received
>> Otherwise, received ACK is stored in buffer
>> ACK is ignored in case it is incorrect
>> Receiving time is recorded and new timer is set
>> i is incremented by 1 in case correct ACK is received
>> Steps 4-8 is repeated until there are no more packets to be sent
>> A final ending message is sent
>> File sender.txt is closed

--Description of receiver.c
>> Takes the command line input and stores them in variables
>> Socket creation and binding is done
>> File receiver.txt is opened to write over it
>> Received packet is stored in buffer
>> Random number is generated and compared with the given probability in case correct packet is received
>> i is incremented by 1 in case random number >= given probability
>> Packet is dropped in case random number < given probability
>> Steps 5-7 does not happen in case incorrect packet is received
>> ACK corresponding to the next expected packet is sent and printed on terminal and txt file
>> Steps 4-9 is repeated until a final ending message is received
>> File receiver.txt is closed
