/*
 * Segment.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: akash
 */

# include "AppMessage.cpp"


struct Segment{
	AppMessage appmessage;

	//headers
	char reservedBits[5];  // 0:SYN, 1:ACK, 2:FIN, 3:FULL
//	char * rwnd;
	char ackNo[33];
	char seqNo[33];
	char sourcePortNo[17];
	char destPortNo[17];


	Segment(){

		for (int i=0;i<4;i++){
			reservedBits[i]='0';
		}

/*
		for (int i=0;i<rwndSize;i++){
			rwnd[i]='0';
		}
*/
		for (int i=0;i<32;i++){
			ackNo[i]='0';
			seqNo[i]='0';
		}
		for (int i=0;i<16;i++){
			sourcePortNo[i]='0';
			destPortNo[i]='0';
		}

		reservedBits[4]='\0';
		ackNo[32]='\0';
		seqNo[32]='\0';
		sourcePortNo[16]='\0';
		destPortNo[16]='\0';
	}


};

class SegmentHandler{

	private:
	public:
		AppHandler appHandler;
		string fileNames;
		Segment * buffer;
		int bufferSize;		//size of buffer
		int bufferIndex;	//bufferIndex sent
		int fillIndex;		//max index has data
		int seqNo;
		bool endOfFile;
		int ackNo;

	public:
		SegmentHandler(){

			bufferIndex=0;
			bufferSize=0;
			fillIndex=0;
			seqNo=0;
			ackNo=0;
			endOfFile=false;
		}
		void updatePorts(Segment& segment,int source, int dest){

			this->setSourcePort(segment,source);
			this->setDestPort(segment,dest);

		}
		void setSourcePort(Segment& segment, int number){
				string str=getBinary(number,16);
				for (int i=0;i<16;i++){
					 segment.sourcePortNo[i]=str[i];
				}
				segment.sourcePortNo[16]='\0';
				return;
		}
		void setDestPort(Segment& segment, int number){
				string str=getBinary(number,16);
				for (int i=0;i<16;i++){
					 segment.destPortNo[i]=str[i];
				}
				segment.destPortNo[16]='\0';
				return;
		}

		int getDestPort(Segment& segment){
			return bin2int(segment.destPortNo);
		}
		int getSourcePort(Segment& segment){
			return bin2int(segment.sourcePortNo);
		}
		void resetSegment(Segment& seg){

			for (int i=0;i<4;i++){
						seg.reservedBits[i]='0';
			}

			for (int i=0;i<32;i++){
				seg.ackNo[i]='0';
				seg.seqNo[i]='0';
			}
			for (int i=0;i<15;i++){
				seg.sourcePortNo[i]='0';
				seg.destPortNo[i]='0';
			}

			seg.reservedBits[4]='\0';
			seg.ackNo[32]='\0';
			seg.seqNo[32]='\0';
			seg.sourcePortNo[16]='\0';
			seg.destPortNo[16]='\0';
		}

		void flush(Segment& segment){
			segment.reservedBits[0]='\0';
//			windSize=0;

		}


		void setSeqNo(Segment& segment, int number){
				string str=getBinary(number,32);
				for (int i=0;i<32;i++){
					 segment.seqNo[i]=str[i];
				}
				segment.seqNo[32]='\0';
				return;
		}
		void setAckNo(Segment& segment, int number){
			string str=getBinary(number,32);
			for (int i=0;i<32;i++){
				segment.ackNo[i]=str[i];
			}
			segment.ackNo[32]='\0';
			return;
		}
		void Display(Segment& segment){
			cout<<"ACKNO : "<<segment.ackNo<<"     "<<bin2int(segment.ackNo)<<endl;
			cout<<"Seq No: "<<segment.seqNo<<"     "<<bin2int(segment.seqNo)<<endl;
			cout<<"Dest Port No:  "<< segment.destPortNo<<"   "<<bin2int(segment.destPortNo)<<endl;
			cout<<"Source Port NO : "<<segment.sourcePortNo<<"    "<<bin2int(segment.sourcePortNo)<<endl;
			cout<<"Reserved Bits : "<<segment.reservedBits<<endl;
		}
		void DisplayAll(Segment& segment){
			cout<<"\n\n------***********----- SEGMENT LEVEL ------********------\n";
			Display(segment);
			cout<<"\n\n------***********----- APPLICATIOIN LEVEL -----********------\n\n";
			appHandler.DisplayAppMsg(segment.appmessage);
		}

		void initBuffer(int buffLen,int pktSize){

			bufferSize=buffLen;
			buffer= new Segment[bufferSize];
			appHandler.setPacketSize(pktSize);
		}
		void setClientAck(Segment& segment){
			this->resetSegment(segment);
			appHandler.flush(segment.appmessage);
			segment.reservedBits[1]='1';
			setAckNo(segment,appHandler.fileSize);
			sleep(5);
		}
		void setFin(Segment& seg){
			this->resetSegment(seg);
			seg.reservedBits[2]='1';
		}
		void ackAtServer(Segment& segment){
			if (segment.reservedBits[1]=='1' && segment.reservedBits[0]=='0' && segment.reservedBits[2]=='0'){
				delete buffer;
				buffer= new Segment[bufferSize];
				fillIndex=0;
				bufferIndex=0;


				cout<<"SEnt & temp : "<<appHandler.sentByte<< " "<<appHandler.tempSentByte<<endl;
				appHandler.sentByte+=appHandler.tempSentByte;
				cout<<"SEnt & temp : "<<appHandler.sentByte<< " "<<appHandler.tempSentByte<<endl;

				sleep(1);


			}
			else{
				cout<<"Error in ack received\n";
			}
		}

		Segment getConSegment(Segment& segment){

			Segment seg;
			resetSegment(seg);

			if (segment.reservedBits[0]!='1' && segment.reservedBits[1]!='1'){
				seg.reservedBits[0]='1';
			}
			else if (segment.reservedBits[0]=='1' && segment.reservedBits[1]=='1' ){
							seg.reservedBits[0]='0';
							seg.reservedBits[1]='1';
						}
			return seg;
		}

		Segment getServerConSegment(Segment& segment){

			Segment seg;
			resetSegment(seg);
			if (segment.reservedBits[0]=='1'){
				seg.reservedBits[0]='1';	//SYN
				seg.reservedBits[1]='1';	//ACK
			}


			return seg;
		}


		void getClientShow(Segment& segment){
			resetSegment(segment);
			appHandler.getClientShow(segment.appmessage);

		}

		void getClientDownload(Segment& segment,string fileName){
			resetSegment(segment);
			appHandler.getClientDownload(segment.appmessage,fileName);
		}
		bool setRequestAtServer(Segment& segment){
			return appHandler.setRequestAtServer(segment.appmessage);
		}
		bool setDataDatagram(Segment& segment){

				return appHandler.setDataDatagram(segment.appmessage,ackNo,segment.seqNo);
		}
		bool setDataDatagram2(Segment& segment){

			return appHandler.setDataDatagram2(segment.appmessage,ackNo,segment.seqNo);
		}

		bool getDataSegment(Segment& segment){

			if (fillIndex==0){
				cout<<"Bufer called\n";
				cout<<endOfFile<<endl;
				if (endOfFile==true){
					cout<<"Retruned\n";
					return endOfFile;
				}
				endOfFile=fullBuffer(0);


			}

			if (bufferIndex < fillIndex){
				segment=buffer[bufferIndex++];
			}
			else{
				return true;
			}


			return false;//endOfFile;
		}


		bool fullBuffer(int j){
			fillIndex=0;
			bufferIndex=0;
			this->appHandler.tempSentByte=0;

			bool endOfFile=false;
			for (int i=0;i<bufferSize && !endOfFile;i++){
				Segment seg;
				setSeqNo(seg,seqNo+appHandler.sentByte+appHandler.tempSentByte);
				endOfFile=appHandler.setAppMessage(seg.appmessage);
				appHandler.tempSentByte+=bin2int(seg.appmessage.size);
				buffer[fillIndex++]=seg;
				cout<<i<<endl;
			//	sleep(2);
			}

			cout<<"out\n";
			cout<<"fil : "<<fillIndex<<endl;
			//sleep(5);

			return endOfFile;
		}
		void timeout(){
			//this->appHandler.sentByte-=this->appHandler.tempSentByte;
			bufferIndex=0;
			//this->appHandler.tempSentByte=0;

			cout<<"Sent Byte : "<<appHandler.sentByte<<endl;
			cout<<"TempSent Byte : "<<appHandler.tempSentByte<<endl;

			bufferIndex=0;

			cout<<"After\n";
			cout<<"Sent Byte : "<<appHandler.sentByte<<endl;
			cout<<"TempSent Byte : "<<appHandler.tempSentByte<<endl;
			sleep(3);

		}
};
