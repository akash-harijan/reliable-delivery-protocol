
# include <iostream>
# include <fstream>
# include "server.cpp"

using namespace std;


int main(){


/*
	ofstream ofile("info.txt");
	if (!ofile){
		cout<<"Error in info.txt file opening\n";
	}
	else{
*/

	int bufferSize=3;
	int pktSize=5;
	int ackDrop=1;
	int ackCorrupt=0;
	char check;
	char ** ips;
	int *ports;
	int pcs;




		 	 ifstream ifile("configure.txt");
		 	 if (!ifile){
		 		 cout<<"File does not exit\n";
		 	 }
		 	 else{
		 		ifile>>pcs;
		 		ips=new char*[pcs];
		 		ports=new int[pcs];
		 		for (int i=0;i<pcs;i++){
		 			ips[i]=new char[33];
		 			ips[i][32]='\0';
		 			ifile>>ips[i];
		 			ifile>>ports[i];
		 			cout<<ips[i]<<endl;
		 		}
		 	 }
				cout<<"Enter s for server \n Enter c for Client\n Enter r for router\n";
				cin>>check;
				if (check=='s'){
					server(ports[pcs-1],ips[pcs-1],bufferSize,pktSize,ackDrop,ackCorrupt);

				}
				else if(check=='r'){
					cout<<"Enter sequence# of router\n";
					int seqNo=0;
					cin>>seqNo;
					Router(ports[seqNo],ips[seqNo],seqNo);

				}
				else if (check=='c'){

					client(ports[0],ips[0],bufferSize,pktSize,ackDrop,ackCorrupt);
				}

/*

	}

	ofile.close();
*/

}
