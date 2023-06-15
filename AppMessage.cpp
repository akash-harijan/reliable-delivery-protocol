/*
 * AppMessage.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: akash
 */

#include<iostream>
#include<cstring>
#include<string>
# include<cstring>
# include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
# include <cmath>

#include "config.h"
#include "unp.h"


using namespace std;

int bin2int(char * );
string getBinary(int num,int len);
int char2int(char * );



struct AppMessage{

	char data[51];
	char size[7];	// size show the no of bytes in the data array
	char msgtype;// this will show the msg Type i.e. 0:Request(client) or 1:Response(Server)
	char rqstType[3];//this will tell further about the request message i.e. 00:Show files
							// 01:download file, 10: upload file
	char key[6];	// this is the key used for encryption and decryption
	char status[3];//this will show the status of file i.e. 00: End of file, 01: Error in file
				// 10:FileNotfound, 11:File continuing


	AppMessage(){
		memset(data,'\0',51);
		memset(size,'\0',6);
		msgtype='\0';
		memset(rqstType,'\0',3);
		memset(key,'\0',6);
		memset(status,'\0',3);


	}
	bool setRqstType(int number){
		string str=getBinary(number,2);
			for (int i=0;i<2;i++){
				rqstType[i]=str[i];
			}
			size[2]='\0';
			return true;

	}
	void setMsgType(int number){
		if (number==0)
			msgtype='0';
		else
			msgtype='1';
	}
	bool setSize(int number){
		string str=getBinary(number,6);
		for (int i=0;i<6;i++){
			size[i]=str[i];
		}
		size[6]='\0';
		return true;
	}

	void setStatus(int number){
		string str=getBinary(number,2);
		for (int i=0;i<2;i++){
			status[i]=str[i];
		}
		status[2]='\0';

	}
	void setKey(int number){
		string str=getBinary(number,5);
		for (int i=0;i<5;i++){
			key[i]=str[i];
		}
		key[5]='\0';
	}


};

class AppHandler{
	private:
	public:
		string fileData;
		char * fileName;
		int fileSize;
		int sentByte;
		int tempSentByte;
		int msgType;
		int packetSize;
	public:
		AppHandler(){
			//flush
			sentByte=0;
			tempSentByte=0;
			fileSize=0;
			msgType=-1;
			packetSize=0;
		}
		void flush(AppMessage& appmessage){

			memset(appmessage.data,'\0',51);
			memset(appmessage.size,'\0',6);
			appmessage.msgtype='\0';
			memset(appmessage.rqstType,'\0',3);
			memset(appmessage.key,'\0',6);
			memset(appmessage.status,'\0',3);


		}
		void getClientShow(AppMessage& appmessage){

			flush(appmessage);
			appmessage.msgtype='0';
			appmessage.rqstType[0]='0';
			appmessage.rqstType[1]='0';
			appmessage.rqstType[2]='\0';
		}
		void getClientDownload(AppMessage& appMessage,string fileName){

			flush(appMessage);
			appMessage.msgtype='0';
			appMessage.rqstType[0]='0';
			appMessage.rqstType[1]='1';
			appMessage.rqstType[2]='\0';

			int i=0;
			for (i=0;i<fileName.length() ;i++){
				appMessage.data[i]=fileName[i];
			}
			appMessage.data[i]='\0';
		}

		void DisplayAppMsg(AppMessage& appmessage){

			cout<<"Data : "<< appmessage.data<<endl;
			cout<<"Key : "<<appmessage.key<<endl;
			cout<<"Message Type : "<<appmessage.msgtype<<endl;
			cout<<"Message Rqst Type : "<<appmessage.rqstType<<endl;
			cout<<"Size : "<<appmessage.size<<endl;
			cout<<"Status : "<<appmessage.status<<endl;
		}
		bool setRequestAtServer(AppMessage& appmessage){

			if ( !strcmp(appmessage.rqstType,"00")  ){
				msgType=1;
				serverShowFiles();
				return true;
			}
			else if ( !strcmp(appmessage.rqstType,"01") ){
				msgType=1;
				cout<<"File Name to read : "<<appmessage.data<<endl;
				sleep(2);
				this->fileName=new char [strlen(appmessage.data)+1];
				for (int i=0;i<strlen(appmessage.data);i++){
					this->fileName[i]=appmessage.data[i];
				}
				this->fileName[strlen(appmessage.data)]='\0';
				return readFile(appmessage.data);

			}
			else if (! strcmp(appmessage.rqstType,"10")){
				msgType=1;
			}
			return false;
		}

		void setPacketSize(int pktSize){
			packetSize=pktSize;
		}

		void serverShowFiles(){

				char dirArray[256];

				struct dirent *dirp;
				DIR * filedesc;
				getcwd(dirArray, 600);
				string output;
				if ((filedesc = opendir(dirArray)) == NULL) {
					cout<<"Files can't be open\n";
					return ;
				}
					string s1="";
					string s2=".txt";
					string s3=".txt~";
				for ( ;(dirp = readdir(filedesc)) != NULL; ) {
					s1=dirp->d_name;
					if(!strcmp(dirp->d_name,".") )
						continue;
					else if (!strcmp(dirp->d_name,".."))
						continue;
					else if(strstr(s1.c_str(),s3.c_str()))
						continue;
					else if(strstr(s1.c_str(),s2.c_str())){
						output+=dirp->d_name;
						//output+=".txt";
						output+="$";
					}
					s1.clear();
				}
				cout<<output<<endl;
				fileData=output;
				fileSize=output.length();

		}

		bool readFile(char * fileName){
			fileData.clear();
			fileSize=0;
			ifstream ifile(fileName);
			if (!ifile){
				cout<<"File Does not exist\n";
				return false;
			}
			else{
				string line;
				while(ifile){
					getline(ifile,line);
					fileData+=line;
				}
				fileSize=fileData.length();
			}
			ifile.close();
			return true;
		}

		bool setAppMessage(AppMessage& appMessage){


			cout<<"Sent bye : "<<sentByte<<endl;
			cout<<"fliseize : "<<fileSize<<endl;
			cout<<"TempSentByte : "<<tempSentByte<<endl;
			sleep(5);


			for (int i=0;i<packetSize;i++){
				if (sentByte+tempSentByte+i<fileSize){
					appMessage.data[i]=fileData[sentByte+tempSentByte+i];
					cout<<"Data at index : "<<i<<" is "<<appMessage.data[i]<<endl;
					sleep(1);

				}
				else{
					cout<<"nothing to send :(\n";
					sleep(2);
					appMessage.setStatus(0);
					appMessage.setSize(i);
				//	tempSentByte+=i;
				//	sentByte+=i;
					return true;
				}
			}
			appMessage.setStatus(3);
			appMessage.setSize(strlen(appMessage.data));
			//tempSentByte+=packetSize;
			//sentByte+=packetSize;
			return false;
		}

		bool setDataDatagram(AppMessage& appMessage,int & ackNo, char * seqNo){
			if (!strcmp(appMessage.status,"00")){

				fileData+=appMessage.data;
				fileSize=fileData.length();
				char * data=new char[fileData.length()+1];
				for (int i=0;i<fileData.length();i++){
					data[i]=fileData[i];
				}
				data[fileData.length()]='\0';

				char * arr=strtok(data,"$");

				cout<<"***********************************************************";
				cout<<"\nFile Names \n\n";
				cout<<"***********************************************************\n";
				while(arr){
					cout<<arr<<endl;
					arr=strtok(NULL,"$");
				}

				sleep(2);
				fileData.clear();
				fileSize=0;
				cout<<"End of File\n";
				return true;

			}
			if (bin2int(seqNo) >= fileSize){
				fileData+=appMessage.data;
				fileSize=fileData.length();
				ackNo=char2int(appMessage.size);
			}
			cout<<"Seq No : "<<bin2int(seqNo)<<endl;
			cout<<" file Size : "<<fileSize<<endl;
			cout<<"Data : "<<fileData<<endl;
			sleep(5);

			return false;
		}

		bool setDataDatagram2(AppMessage& appMessage,int & ackNo,char * seqNo){
			if (!strcmp(appMessage.status,"00")){


				fileData+=appMessage.data;
				fileSize=fileData.length();

				ofstream ofile("output.txt");
				if (!ofile){
					cout<<"File can't create\n";
				}
				else{

						ofile<<fileData;

				}

				ofile.close();
				sleep(2);
				fileData.clear();
				fileSize=0;
				cout<<"End of File\n";
				return true;
			}
			if ( bin2int(seqNo) >=fileSize ){
				fileData+=appMessage.data;
				fileSize=fileData.length();
				ackNo=char2int(appMessage.size);
			}
			cout<<"Seq No : "<<bin2int(seqNo)<<endl;
			cout<<" file Size : "<<fileSize<<endl;
			cout<<"Data : "<<fileData<<endl;
			sleep(5);
			return false;
		}

};


string getBinary(int num, int len){

	char * arr= new char [len+1];
	for (int i=0;i<len;i++){
		if (num>0){
			if (num%2==0)
				arr[len-i-1]='0';
			else
				arr[len-i-1]='1';
			num=num/2;
		}
		else{
			arr[len-i-1]='0';
		}

	}
	arr[len]='\0';
	return arr;
}



int char2int(char * arr){

	int length=strlen(arr);
	int retVal=0;
	for (int i=1;i<=length;i++){
		retVal*=10;
		retVal+=static_cast<int>(arr[length-i]%48);
	}

	return retVal;
}

int bin2int(char * arr){

	int length=strlen(arr);
	int retVal=0;
	for (int i=1;i<=length;i++){
		retVal+=(static_cast<int>(arr[length-i]%48))*pow(2,i-1);

	}
	return retVal;
}
