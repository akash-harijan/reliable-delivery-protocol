# UDP-based Client-Server File Transfer with Reliable Delivery Protocol (RDP)

This project implements a UDP-based client-server architecture for simple file transfer. The goal is to manage the transfer of file data from the client to the server using only a UDP channel with unreliable delivery. The project aims to enhance understanding of common transport layer protocol mechanisms, such as fragmentation, error detection and/or correction, and retransmission. This is achieved by constructing a Reliable Delivery Protocol (RDP) on top of UDP.

Key Features:

- Reliable transfer of file data over a flaky UDP channel
- Fragmentation and reassembly of data packets
- Error detection and correction mechanisms
- Concurrent operation with multiple clients using POSIX threads
- Two-layer stack comprising an RDP layer and an application layer

To ensure efficient management of concurrent accesses to data structures, it is recommended to use separate UDP sockets for handling client requests and to support each RDP stream in the server. The implementation follows a two-layer stack architecture, with the RDP layer handling reliable delivery and the application layer managing the file transfer.

## Table of Contents

- [Requirements](#requirements)
- [Structure](#structure)
- [Usage](#usage)
- [Acknowledgements](#acknowledgements)


## Requirements

- C++

## Structure

This project is an implementation of Reliable Delivery Protocol, so OOP concepts are used accordingly to immitate the two-layer stack of RDP model.

 - Application Layer: AppMessage.cpp shows how application layer works and it encodes the data insides the message.
 - Transport Layer: segment.cpp shows how application layer message is encoded at Transport Layer in a segment.
 - Network Layer: Datagram.cpp shows how transport layer segment is encoded at Network Layer in a datagram.
 - Client : client.cpp is file that has all the code related to client handling.
 - Server : server.cpp is file that has all the code related to Server.
 - Main : main.cpp is file that runs this whole project.

While running main.cpp, it gives three options:

- Server
- Client
- Router

This code can be run on one machine, or more than one machines in same network to send a message from client to Server, and get message from server to client back. There can be router in between the client and server acting as hops as it happens in real life. 

There are two files that defines the ips and ports of client, Server and routers.

- configure.txt
- input.txt

Confiure.txt defines how many nodes are involved including client and server, also defining their ip addresses and ports. Input.txt defines a graph in which these nodes are connected to each other, later a Dijkstra's algorithm is implemented in the code to find shorted path in these nodes to communicate between server and client.



## Usage

As current configure.txt has 4 nodes in it, so we have to run the main.cpp 4 times, one time for server, 2 times for routers and 1 time for client.

```
$ g++ main.cpp -o exe
$ ./exe
```

Then it will start doing the sync between client and server using the shorted path between client and server, calcuated by Dijkstra algorithm.


## Acknowledgement

This code was written by me in 2016 as a project of Computer Network course in National University of Computer and Emerging Sciences.




