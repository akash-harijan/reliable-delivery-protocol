/*
 * server.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: akash
 */

# include "client.cpp"


struct ThreadParam{

	int bufferSize;
	int packetSize;
	int ackDrop;
	int ackCorrupt;
	int fd;
	sockaddr_in addr;
	Digsktra topology;
	string sourceIP;
	string destIP;


};


void * fun(void * arg ){

	int noOfPacketSend=0,noOfPacketRecv=0;

	ThreadParam * threadParam = (ThreadParam * )arg;
	sockaddr_in clientAddr=threadParam->addr;
	int bufferSize=threadParam->bufferSize;
	int packetSize=threadParam->packetSize;
	int ackDrop=threadParam->ackDrop;
	int ackCorrupt=threadParam->ackCorrupt;
	Digsktra topology=threadParam->topology;
	string sourceIP=threadParam->sourceIP;
	string destIP=threadParam->destIP;
	int clientLen=sizeof(clientAddr);
	int source_port;
	int dest_port;
	char tempIP[33];
	tempIP[32]='\0';

	for(int i=0;sourceIP[i]!='\0';i++)
	tempIP[i]=sourceIP[i];


	struct sockaddr_in newServerAddr;

	cout<<"Server port : "<<htons(clientAddr.sin_port)<<endl;
	newServerAddr.sin_family=AF_INET;
	dest_port=clientAddr.sin_port;
	newServerAddr.sin_port=htons(clientAddr.sin_port+50);
	newServerAddr.sin_addr.s_addr=inet_addr(tempIP);
	memset(&(newServerAddr.sin_zero),'\0',8);
	source_port=htons(newServerAddr.sin_port);

	cout<<"source port : "<<source_port<<endl;
	cout<<"After serverport : "<<htons(newServerAddr.sin_port)<<endl;

	sleep(10);


	int newSock=socket(AF_INET,SOCK_DGRAM,0);

	int bind1=bind(newSock,(struct sockaddr *)&newServerAddr, sizeof(struct sockaddr));
	if (bind1==-1){
		cout<<"Error in bind\n";
	}


	//	cout<<"After serverport 123 : "<<htons(newServerAddr.sin_port)<<endl;

	Datagram datagram;

	DatagramHandler handler;
	handler.setSourceIP(datagram,sourceIP);
	handler.setDestinationIP(datagram,destIP);
	handler.updatePorts(datagram,source_port,dest_port);

	int bytecount=sendto(newSock,&datagram,sizeof(Datagram),0,(struct sockaddr *)&clientAddr,(socklen_t) (clientLen));
	if (bytecount==-1){
		cout<<"Error in send bytes to UDP client\n";
	}

	while(true){
		DatagramHandler handler;
		Datagram datagram;
		cout<<"***********************************************************\n";
		cout<<"Waiting for request from client\n";
		cout<<"***************************************************************\n";

		fd_set rfds;

		FD_ZERO(&rfds);
		FD_SET(newSock, &rfds);


		select(newSock+1, &rfds, NULL, NULL, NULL);

		bytecount=recvfrom(newSock,&datagram,sizeof(Datagram),0,(struct sockaddr *) &clientAddr, (socklen_t *)&clientLen);
		if (bytecount==-1){
			cout<<"Error in receiving type\n";
		}
		cout<<"\t\tReceived the Request :)\n";
		sleep(1);
		handler.DisplayAll(datagram);
		sleep(5);

		bool checkFile=true;
		checkFile=handler.setRequestAtServer(datagram);
		if (checkFile==false){
			cout<<"File does not exist";
			continue;
		}
		handler.initBuffer(bufferSize,packetSize);

		bool loop=true;
		while(loop){
			bool dataCheck=true;
			for (int i=0;i<bufferSize;i++){

				cout<<"\n\n\n iteration : "<<i<<endl;

				Datagram dataDatagram;
				dataCheck=!handler.getDataDatagram(dataDatagram);
				cout<<"AT the Server\n";
				cout<<dataCheck<<endl;


				if (dataCheck==false){
					cout<<"File is end\n\n\n";
					sleep(2);
					loop=false;
					break;

				}

				cout<<"\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
				cout<<"Sending  Data "<<dataCheck<<endl;
				cout<<"\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
				sleep(2);

				handler.setSourceIP(dataDatagram,sourceIP);
				handler.setDestinationIP(dataDatagram,destIP);
				handler.updatePorts(dataDatagram,source_port,dest_port);

				handler.DisplayAll(dataDatagram);

				cout<<source_port<<" "<<dest_port<<endl;
				sleep(5);


				sendto(newSock,&dataDatagram,sizeof(Datagram),0,(struct sockaddr *)&clientAddr, (socklen_t)clientLen);

				//sleep(2);
			}
			if (loop==false){
				cout<<"Out of 2nd loop\n";
				sleep(3);
				break;
			}

			Datagram ackDatagram;
			cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
			cout<<"Waiting for acknowledgement\n";
			cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

			timeval tv2;
			tv2.tv_sec=50;
			tv2.tv_usec=0;

			fd_set rfds2;

			FD_ZERO(&rfds2);
			FD_SET(newSock, &rfds2);


			int retval = select(newSock+1, &rfds2, NULL, NULL, &tv2);

			if (retval==-1){
				cout<<"Error in the Select\n";
			}
			else if (retval){
				cout<<"Data is available\n";
				int checkval=recvfrom(newSock,&ackDatagram,sizeof(Datagram),0,(struct sockaddr * )&clientAddr, (socklen_t * )&clientLen);
				cout<<"\tReceived the Acknowledgment :)\n";
				sleep(2);
				handler.DisplayAll(ackDatagram);
				sleep(5);
				handler.ackAtServer(ackDatagram);

			}
			else{
				cout<<"No data in specific time\n";
				cout<<"\n\t\t Timeout  :) \n";
				sleep(2);
				handler.timeout();
				continue;

			}

		}

		cout<<"Closed this request\n";
	}
	cout<<"This is closed\n";

}

void startRouter(char* router_ip,int routerPort,int RNo){

	struct sockaddr_in newRouterAdder;
	newRouterAdder.sin_family=AF_INET;
	newRouterAdder.sin_port=htons(routerPort);
	newRouterAdder.sin_addr.s_addr=inet_addr(router_ip);
	memset(&(newRouterAdder.sin_zero),'\0',8);
	int newSock=socket(AF_INET,SOCK_DGRAM,0);
cout<<router_ip;
cout<<routerPort;


	char serverip[33];
	serverip[32]='\0';
	int port;
	int total=0;
	int tport;
	string tempip;
	fstream ifile("configure.txt");
	if(!ifile){
		cout<<"File does not exit\n";
	}
	else{
		ifile>>total;
		for(int i=0;i<total;i++){
			if(i==total-1){
				ifile>>serverip;
				ifile>>port;
				break;
			}
			else{
				ifile>>tempip;
				ifile>>tport;
			}
		}
	}

	int bind1=bind(newSock,(struct sockaddr *)&newRouterAdder,sizeof(struct sockaddr));

	if(bind1==-1){
		cout<<"Router is not binded\n";
	}
	sockaddr_in clientAddr;
	int clientAddrLen=sizeof(clientAddr);
	Digsktra D;
	D.intialize(RNo);
	D.calculateSPath();

	while(1){
		Datagram datagram;

		int checkVal=recvfrom(newSock,&datagram,sizeof(Datagram),0,(sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen);
					if (checkVal==-1){
						cout<<"Error in receiving ACK\n";
			}
		cout<<datagram.dest_ip<<endl;
		int routerNo=D.nextRouter(datagram.dest_ip);
		char nextip[33];
		nextip[32]='\0';
		int port;
		int total=0;
		int tport;
		string tempip;
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
					ifile>>tempip;
					ifile>>tport;
				}
			}
		}
		if(!strcmp(nextip,serverip)){
			DatagramHandler handler;
			int serverport=handler.getSourcePort(datagram);
			cout<<"fhkjsdajfsadhfjsdhkj"<<serverport<<endl;
			clientAddr.sin_port=htons(serverport);
			clientAddr.sin_addr.s_addr=inet_addr(nextip);
		}
		else{

			clientAddr.sin_port=htons(port);
			clientAddr.sin_addr.s_addr=inet_addr(nextip);
		}

cout<<"IP of next router\n"<<nextip<<endl;
cout<<"Port# of next router\n"<<port<<endl;

	DatagramHandler handler;
	handler.DisplayAll(datagram);
	sleep(5);
	checkVal=sendto(newSock,&datagram,sizeof(Datagram),0,(sockaddr * )& clientAddr, (socklen_t)clientAddrLen);
		if (checkVal==-1){
			cout<<"Error in sending SYN ACK\n";
		}

	}
}


void Router(int routerPort, char * routerIP,int RNo){

		startRouter(routerIP,routerPort,RNo);

}
void server(int serverPort, char * serverIP,int bufferSize, int pktSize,int ackDrop, int ackCorrupt){

	Digsktra topology;
	topology.intialize(3);
	topology.calculateSPath();
	string source_ip;
	string dest_ip;
	int source_port;
	int dest_port;
	int ptemp;
	string iptemp;
	ifstream cfile("configure.txt");
	if(!cfile){
		cout<<"File does not exit\n";
	}
	else{
		int total;
		cfile>>total;
		cfile>>dest_ip;
		cfile>>dest_port;
		for(int i=1;i<total;i++){
			if (i==total-1){
				cfile>>source_ip;
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




	while(true){

		int serverSock=socket(AF_INET,SOCK_DGRAM,0);

		if (serverSock==-1){
			cout<<"Error in welcome socket\n";
		}
		else{

				cout<<"Welcome socket created successfully\n";
				struct sockaddr_in serverAddr;

				serverAddr.sin_family=AF_INET;
				serverAddr.sin_port=htons(serverPort);
				serverAddr.sin_addr.s_addr=inet_addr(serverIP);
				memset(&(serverAddr.sin_zero),'\0',8);


			int serverBind=bind(serverSock,(struct sockaddr *)& serverAddr, sizeof (struct sockaddr));




			if (serverBind==-1){
				cout<<"Error in binding the wellcome socket\n";
				exit(0);
			}
			else{
				cout<<"Wellcome socket binded successfully\n";


				while(1){
					struct sockaddr_in clientAddr;
					int clientAddrLen= sizeof (clientAddr);


					DatagramHandler conHandler;
					Datagram conDatagram;


					int checkVal=recvfrom(serverSock,&conDatagram,sizeof(Datagram),0,(struct sockaddr*)&clientAddr,(socklen_t *)&(clientAddrLen));
					if (checkVal==-1){
						cout<<"Error in recv funtion\n";
					}

					cout<<"\nReceived Packet  'SYN' : \n\n";
					conHandler.Display(conDatagram);

					Datagram datagram=conHandler.getServerConDatagram(conDatagram);

					cout<<"\nSending Packet ACK SYN \n\n ";
					conHandler.Display(datagram);
					conHandler.setSourceIP(datagram,source_ip);
					conHandler.setDestinationIP(datagram,dest_ip);
					conHandler.updatePorts(datagram,source_port,dest_port);

					cout << "\nIP address : "<<inet_ntoa(clientAddr.sin_addr)<<endl;
					cout<<" Port no : "<<htons(clientAddr.sin_port)<<endl;


					checkVal=sendto(serverSock,&datagram,sizeof(Datagram),0,(sockaddr * )& clientAddr, (socklen_t)clientAddrLen);
					if (checkVal==-1){
						cout<<"Error in sending SYN ACK\n";
					}

					Datagram datagram2;

/*
					struct timeval t;
					t.tv_sec=5;
					t.tv_usec=0;

					setsockopt(serverSock,SOL_SOCKET,SO_RCVTIMEO,&t,sizeof(t));
*/
					checkVal=recvfrom(serverSock,&datagram2,sizeof(Datagram),0,(sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen);
					if (checkVal==-1){
						cout<<"Error in receiving ACK\n";
					}
/*
					if (checkVal==0 || errno==11){
						cout<<"timeout\n";
						cout<<"Resending the packet\n";
					}
*/
					cout<<"\nReceiving Packet ACK\n\n";
					conHandler.Display(datagram2);

					ThreadParam threadParam;
					threadParam.fd=serverSock;
					threadParam.addr=clientAddr;
					threadParam.bufferSize=bufferSize;
					threadParam.packetSize=pktSize;
					threadParam.ackCorrupt=ackCorrupt;
					threadParam.ackDrop=ackDrop;
					threadParam.topology=topology;
					threadParam.sourceIP=source_ip;
					threadParam.destIP=dest_ip;
					pthread_t tid;
					checkVal=pthread_create(&tid,NULL,fun,&threadParam);
					if (checkVal==-1){
						cout<<"Error in the creating thread\n";
					}

				}



			}
		}
	}
}
