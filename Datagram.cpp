/*
 * Datagram.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: akash
 */

# include "Segment.cpp"

class Digsktra{

	private:
		 char** ips;//IP of router
		 int ** graphs;//Adjency Matrix;

		 int * sp_distance;//sp_distance[i] contain shortest distance form source to i;
		 bool* sptSet;//sptSet[i] true if vertex i included in shortest path
		int *predecessor;
		 int vertices;
		 int source;
	public:

		 Digsktra(){

			 ips=NULL;
			 sp_distance=NULL;
			 sptSet=NULL;
			 vertices=0;
			 graphs=NULL;
			 source=0;
			 predecessor=NULL;
		 }

		 void intialize(int source){

			 this->source=source;
			 ifstream ifile("input.txt");
			 if(!ifile){
				 cout<<"File does not exit";
			 }
			 else{
				 	 ifile>>vertices;
				 	 cout<<"vertices "<<vertices<<endl;
				 	 sp_distance=new int[vertices];
					 sptSet=new bool[vertices];
					 ips=new char*[vertices];
					 graphs=new int*[vertices];
					predecessor=new int[vertices];
					for(int i=0;i<vertices;i++){
						sptSet[i]=false;
						sp_distance[i]=5000;//infinite distance
						predecessor[i]=5000;
						ips[i]=new char[33];
						ips[32]='\0';
					 }
					 sp_distance[source]=0;
					cout<<"source "<<source<<endl;
					 for(int i=0;i<vertices;i++){
						 graphs[i]=new int[vertices];
						 cout<<"Row\n";
						 for(int j=0;j<vertices;j++){
							ifile>>graphs[i][j];
						 	cout<<graphs[i][j]<<" ";
						 }
						 cout<<endl;
					 }

					 for(int i=0;i<vertices;i++){
						 ifile>>ips[i];
						 cout<<"IP of router "<<i+1<<" "<<ips[i]<<endl;
						 }

					}
			}

		 int minimumPath(int sp_distance[],bool sptSet[]){

			 int min=5000;
			 int minIndex=0;
			 int v=0;
				while(v<vertices){

					if (sp_distance[v]<=min && sptSet[v]==false){
						 min=sp_distance[v];
						 minIndex=v;
					 }
					 v++;

				}
				 return minIndex;
		 }

		 void showShortestDistance(int* sp_distance,int vertices){

			 cout<<"Distance from source \n";
			 for (int i=0;i<vertices;i++){
				 cout<<i<<" "<<sp_distance[i]<<endl;
			 }
			 cout<<"predecessor\n";
			 for (int i=0;i<vertices;i++){
				 cout<<i<<" "<<predecessor[i]<<endl;
			 }
		 }

		void nextPath(){

			for( int i=0;i<vertices;i++){
			sptSet[i]=false;
			predecessor[i]=5000;
			sp_distance[i]=5000;
			}
			sp_distance[source]=0;

			}

		 void calculateSPath(){

			nextPath();
			 for (int i=0;i<vertices;i++){

				 int u=minimumPath(sp_distance,sptSet);
				 sptSet[u]=true;
				 int v=0;
				 while(v<vertices){

					 if (!sptSet[v] && graphs[u][v]>0 && sp_distance[u]!=50000 && sp_distance[u]+graphs[u][v]<sp_distance[v]){
						 sp_distance[v]=sp_distance[u]+graphs[u][v];
					 	 predecessor[v]=u;
					 }
					 v++;
				 }

			 }

			 //showShortestDistance(sp_distance,vertices);
		 }


		void shortestpath(int dindex,int& nextrouter){

			 if (source==dindex){
					cout<<"Next router Index "<<dindex<<endl;
					return;
			 }

			 else if(predecessor[dindex]==5000){
				 cout<<"Path is not calculated\n";
				 return;
			 }
			 else{

			 	 nextrouter=dindex;
			 	 shortestpath(predecessor[nextrouter],nextrouter);
				 	 return;
			 }

}

      int nextRouter(char destIP[]){

			 int dindex=0;
			 while(dindex<vertices){
				 if(strcmp(destIP,ips[dindex])==0){
					 break;
				 }
				 dindex++;
			 }
			 int nextrouter=-5;
			 shortestpath(dindex,nextrouter);
			 return nextrouter;
	}


	int** getGraphs() const {
		return graphs;
	}

	void setGraphs(int** graphs) {
		this->graphs = graphs;
	}

	char** getIps() const {
		return ips;
	}

	void setIps(char** ips) {
		this->ips = ips;
	}

	int getSource() const {
		return source;
	}

	void setSource(int source) {
		this->source = source;
	}

	int* getSpDistance() const {
		return sp_distance;
	}

	void setSpDistance(int* spDistance) {
		sp_distance = spDistance;
	}

	bool* getSptSet() const {
		return sptSet;
	}

	void setSptSet(bool* sptSet) {
		this->sptSet = sptSet;
	}

	int getVertices() const {
		return vertices;
	}

	void setVertices(int vertices) {
		this->vertices = vertices;
	}
};





struct Datagram{

	Segment segment;
	char source_ip[33];
	char dest_ip[33];

	Datagram(){
		for(int i=0;i<32;i++){
			source_ip[i]='0';
			dest_ip[i]='0';
		}

		source_ip[33]='\0';
		dest_ip[33]='\0';
	}

};

class DatagramHandler{
	//private:
public:
		SegmentHandler segmentHandler;
	public:

	void updatePorts(Datagram& datagram,int source, int dest){
		segmentHandler.updatePorts(datagram.segment,source,dest);
	}

	int getSourcePort(Datagram& datagram){
		return segmentHandler.getSourcePort(datagram.segment);
	}
	int getDestPort(Datagram& datagram){
		return segmentHandler.getDestPort(datagram.segment);
	}



	void Display(Datagram& datagram){
		segmentHandler.Display(datagram.segment);
	}
	void DisplayAll(Datagram& datagram){


		cout<<"IP source address : "<<datagram.source_ip<<endl;
		cout<<"IP dest address : "<<datagram.dest_ip<<endl;
		segmentHandler.DisplayAll(datagram.segment);
	}
	bool setRequestAtServer(Datagram& datagram){
		return segmentHandler.setRequestAtServer(datagram.segment);
	}
	bool getDataDatagram(Datagram& datagram){
		return segmentHandler.getDataSegment(datagram.segment);
	}
	bool setDataDatagram2(Datagram& datagram){
		return segmentHandler.setDataDatagram2(datagram.segment);
	}
	bool setDataDatagram(Datagram& datagram){
		return segmentHandler.setDataDatagram(datagram.segment);
	}
	void initBuffer(int buffLen,int pktSize){
		segmentHandler.initBuffer(buffLen,pktSize);
	}
	void setClientAck(Datagram& datagram){
		segmentHandler.setClientAck(datagram.segment);
	}
	void setFin(Datagram& datagram){
		segmentHandler.setFin(datagram.segment);
	}
	void timeout(){
		segmentHandler.timeout();
	}
	void ackAtServer(Datagram& datagram){
		segmentHandler.ackAtServer(datagram.segment);
	}
	Datagram getConDatagram(Datagram& datagram){
		Datagram retDatagram;
		retDatagram.segment=segmentHandler.getConSegment(datagram.segment);
		cout<<"At datagarm level : \n";
//		segmentHandler.Display(retDatagram.segment);
		cout<<endl;
		return retDatagram;
	}
	Datagram getServerConDatagram(Datagram& datagram){
		Datagram retDatagram;
		retDatagram.segment=segmentHandler.getServerConSegment(datagram.segment);
//		segmentHandler.Display(retDatagram.segment);
		cout<<endl;
		return retDatagram;
	}

	void getClientShow(Datagram& datagram){
		segmentHandler.getClientShow(datagram.segment);
	}
	void getClientDownload(Datagram& datagram,string fileName){
		segmentHandler. getClientDownload(datagram.segment,fileName);
	}

	void setSourceIP(Datagram& datagram,string IP){

		for (int i=0;i<32;i++){
			datagram.source_ip[i]=IP[i];
		}
		datagram.source_ip[33]='\0';
	}
	char* getSourceIP(Datagram& datagram){

		return datagram.source_ip;
	}
	void setDestinationIP(Datagram& datagram,string IP){
		for(int i=0;i<32;i++){
			datagram.dest_ip[i]=IP[i];
		}
		datagram.dest_ip[33]='\0';
	}
	char* getDestIP(Datagram& datagram){


		return datagram.dest_ip;
	}

};

