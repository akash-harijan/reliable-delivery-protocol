/*
 * client.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: akash
 */

# include "Datagram.cpp"



void client(int serverPort,char IPaddr[],int bufferSize, int packetSize, int ackDrop,int ackCorrupt){

	int noOfPacket=0;
	string source_ip;
	string dest_ip;
	int source_port;
	int dest_port;
	cout<<"IT is client\n";

	int noOfPacketSend=0;


	if (ackDrop!=0){
		ackDrop=ackDrop/packetSize;
		ackDrop++;
	}




	int clientSock=socket(AF_INET,SOCK_DGRAM,0);
	if (clientSock==-1){
		cout<<"Error in client Socket\n";
		exit(0);
	}
	else{

		//path.

		DatagramHandler conHandler;
		Datagram conDatagram;
		Datagram get=conHandler.getConDatagram(conDatagram);

		cout<<"\nSending Packet SYN \n\n";
		conHandler.Display(get);
		string iptemp;
		string ptemp;
		ifstream cfile("configure.txt");
		if(!cfile){
			cout<<"File does not exit\n";
		}
		else{
			int total;
			cfile>>total;
			cfile>>source_ip;
			cfile>>dest_port;
			for(int i=1;i<total;i++){
				if (i==total-1){
					cfile>>dest_ip;
					cfile>>source_port;
					break;
				}
				else{
					cfile>>iptemp;
					cfile>>ptemp;
				}
			}
		}
		cout<<"Source IP address "<<source_ip<<endl;
		cout<<"Destination IP address "<<dest_ip<<endl;
		conHandler.setSourceIP(get,source_ip);
		conHandler.setDestinationIP(get,dest_ip);
		conHandler.updatePorts(get,source_port,dest_port);

		Digsktra path;
		path.intialize(0);
		path.calculateSPath();
		int routerNo=path.nextRouter(get.dest_ip);
		char* nextip=new char[33];
		nextip[32]='\0';
		int port;
		int total=0;
		int tport;
		string tip;
		fstream ifile("configure.txt");
		if(!ifile){
			cout<<"File does not exit\n";
		}
		else{
			ifile>>total;
			for(int i=0;i<total;i++){
				if(i==routerNo){
					ifile>>nextip;
					ifile>>port;
					break;
				}
				else{
					ifile>>tip;
					ifile>>tport;
				}
			}
		}
cout<<"IP"<<nextip<<endl;
cout<<"port"<<port<<endl;
		struct sockaddr_in serverAddr;
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_port=htons(port);
		serverAddr.sin_addr.s_addr=inet_addr(nextip);
		memset(&(serverAddr.sin_zero),'\0',8);
		int servLen=sizeof(serverAddr);

		char clientIP[33];
		clientIP[32]='\0';
		int clientPort;
		fstream gfile("configure.txt");
		if(!gfile){
			cout<<"File does not exit\n";
		}
		else{
			gfile>>total;
			for(int i=0;i<total;i++){
				if(i==0){
					gfile>>clientIP;
					gfile>>clientPort;
					break;
				}
				else{
					gfile>>tip;
					gfile>>tport;
				}
			}
		}



		struct sockaddr_in servAddr;
		servAddr.sin_family=AF_INET;
		servAddr.sin_port=htons(clientPort);
		servAddr.sin_addr.s_addr=inet_addr(clientIP);
		memset(&(servAddr.sin_zero),'\0',8);


		int bind1=bind(clientSock,(struct sockaddr *)&servAddr,sizeof(struct sockaddr));

		cout<<"Sending the ACK\n";
		conHandler.Display(get);

		int checkVal=sendto(clientSock,&get,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t)servLen);
		if (checkVal==-1){
			cout<<"Sending error in the connection request\n";
		}



		Datagram datagram;
		checkVal=recvfrom(clientSock,&datagram,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t *)&servLen);
		if (checkVal==-1){
			cout<<"Error in receivng SYN ACK\n";
		}
		cout<<"\nReceiving Packet SYN ACK \n\n";
		conHandler.Display(datagram);

		Datagram datagram2=conHandler.getConDatagram(datagram);
		cout<<"\nSending packet  ACK  \n\n";
		conHandler.Display(datagram2);
		conHandler.setSourceIP(datagram2,source_ip);
		conHandler.setDestinationIP(datagram2,dest_ip);
		conHandler.updatePorts(datagram2,source_port,dest_port);

		checkVal=sendto(clientSock,&datagram2,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t)servLen);
		if (checkVal==-1){

		}

		Datagram datagram1;

		checkVal=recvfrom(clientSock,&datagram1,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t *)&servLen);
		if (checkVal==-1){
				cout<<"Error in receivng SYN ACK\n";
		}

		DatagramHandler handler;

		source_port=handler.getSourcePort(datagram1);
	//	source_port+=1;
		cout<<"Port at client"<<source_port<<endl;

		int outerLoop=1;
		while(outerLoop){

			cout<<"Press 1 to show the files at server\n";
			cout<<"Press 2 to download file\n";
			cout<<"Press 0 for terminating\n";

			cin>>outerLoop;

			switch(outerLoop){
				case 1:
				{
					Datagram datagram;
					handler.getClientShow(datagram);

					cout<<"***********************************************************\n";
					cout<<"Sending Request for the Show the Files\n";
					cout<<"***************************************************************\n";
					handler.setSourceIP(datagram,source_ip);
					handler.setDestinationIP(datagram,dest_ip);
					handler.updatePorts(datagram,source_port,dest_port);

					handler.DisplayAll(datagram);
					cout<<"Port at client"<<source_port+1<<endl;

					sleep(5);


					sendto(clientSock,&datagram,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t )servLen);

					bool loop=true;
					while(loop){
						Datagram dataDatagram;

						for (int i=0;i<bufferSize;i++){
							cout<<"Waiting to receive Packet\n";
							recvfrom(clientSock,&dataDatagram,sizeof(Datagram),0,(struct sockaddr * )&serverAddr,(socklen_t *)&servLen);
							cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
							cout<<"Packet Received "<<endl;
							cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
							sleep(1);
							handler.DisplayAll(dataDatagram);
							sleep(5);
							bool check=handler.setDataDatagram(dataDatagram);
							if (check==true){
								loop=false;
								cout<<"End of File\n";
								break;
							}
						}

						if (loop==false)
							break;
						Datagram ackDatagram;
						handler.setClientAck(ackDatagram);

						cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
						cout<<"SENDING ACKNOWLEDGMENT\n";
						cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
						sleep(2);
						handler.DisplayAll(ackDatagram);


						cout<<"NO of Packets Send : "<<++noOfPacketSend<<endl;
						if (noOfPacketSend== ackCorrupt || noOfPacketSend==ackDrop){
							cout<<"Packet is Lost || corrupt\n";
							sleep(5);
							continue;
						}

						handler.setSourceIP(ackDatagram,source_ip);
						handler.setDestinationIP(ackDatagram,dest_ip);
						handler.updatePorts(ackDatagram,source_port,dest_port);

						sendto(clientSock,&ackDatagram,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t)servLen);
						cout<<errno<<endl;
						cout<<"Packet Send\n";
						sleep(2);
					}

					break;
				}
				case 2:
				{
					string fileName;
					cout<<"Enter the file : ";
					cin>>fileName;

					Datagram datagram;
					handler.getClientDownload(datagram,fileName);
					handler.DisplayAll(datagram);
					handler.setSourceIP(datagram,source_ip);
					handler.setDestinationIP(datagram,dest_ip);
					handler.updatePorts(datagram,source_port,dest_port);

					sendto(clientSock,&datagram,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t )servLen);


					bool loop=true;
					while(loop){
						Datagram dataDatagram;

						for (int i=0;i<bufferSize;i++){
							recvfrom(clientSock,&dataDatagram,sizeof(Datagram),0,(struct sockaddr * )&serverAddr,(socklen_t *)&servLen);
							cout<<errno<<endl;
							cout<<"\nReceiving\n\n";
							handler.DisplayAll(dataDatagram);
							bool check=handler.setDataDatagram2(dataDatagram);
							if (check==true){
								loop=false;
								cout<<"Mehtab\n";
								break;
							}
						}

						if (loop==false)
							break;
						Datagram ackDatagram;
						handler.setClientAck(ackDatagram);
						cout<<"Sending ack \n\n\n\n";
						handler.DisplayAll(ackDatagram);

						sleep(10);
						handler.setSourceIP(ackDatagram,source_ip);
						handler.setDestinationIP(ackDatagram,dest_ip);
						handler.updatePorts(ackDatagram,source_port,dest_port);

						cout<<"NO of Packets Send : "<<++noOfPacketSend<<endl;
						/*if (noOfPacketSend== ackCorrupt || noOfPacketSend==ackDrop){
							cout<<"Packet is Lost || corrupt\n";
							sleep(5);
							continue;
						}*/


						sendto(clientSock,&ackDatagram,sizeof(Datagram),0,(struct sockaddr *)&serverAddr,(socklen_t)servLen);
						cout<<errno<<endl;
						cout<<"Packet Send\n";
						sleep(2);

					}
					break;
				}
				case 0:{
					cout<<"terminating\n";
					break;
				}
				default:
				{
					cout<<"Invalid \n";
				}
			}
		}

	}

}
