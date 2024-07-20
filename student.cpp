#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>

#define _BSD_SOURCE
void error(const char* msg) {

	perror(msg);
	exit(0);
}

//0 for file name, 1 for server ip address, 2 for portno
int main(int argc, char *argv[]) {

	int port, n;
	int  ListenSocket, sock1, sock2, sock3;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	char buffer[1024];
	char buffer2[1024];
	int result = 0;

	//check the no of argc, need both ip & port
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(1);
	}

	//get the port
	port = atoi(argv[2]);
	//create sock
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock1 < 0)
		error("Cannot open socket");

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		error("Cannot find the host");
	}

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//transforming bytes from *server to the struct serv_addr, perimeter is the length(h_length)
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sock1, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		error("Connection failed");
	else 
		printf("Connection success\n");
	//code for step 1 & 2
	int sid = 1155144393;
	printf("Sending the student id: %d\n", sid);
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%d", sid);
	int sid_len = (int)strlen(buffer);
	while (sid_len > 0) {
		result = send( sock1, buffer-sid_len+(int)strlen(buffer), sid_len, 0 );
		if (result < 0) {
			printf("send() failed with error\n");
			return 1;
		}
		sid_len -= result;
	}
	printf("Done\n"); 
	//end of code for step 1 & 2
	//code for step 3
	char port2[6];
	memset(port2, 0, sizeof(port2));
	int totalByteReceived = 0;
	int byteToReceive = 5;
	do {
		result = recv(sock1, port2+totalByteReceived, 11-totalByteReceived, 0);
		if ( result > 0 ){
			printf("Bytes received: %d\n", result);
			totalByteReceived += result;
			if (totalByteReceived >= byteToReceive)
				break;
		}
		else if ( result == 0 ){
			printf(" %s %d \n",strerror(errno), errno);
			printf("Connection closed\n");
			return 1;
		}
		else{
			printf(" %s %d \n",strerror(errno), errno);			
			printf("recv failed\n");
			return 1;
		}
	} while( result > 0 );
	printf("portno received: %s\n", port2);
	int portno2 = atoi(port2);
	close(sock1);
	//create a TCP socket 2
	printf("Creating TCP socket for listening and accepting connection...");
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("0.0.0.0");
	service.sin_port = htons(portno2);

	//check bind success or not
	if (bind( ListenSocket, (struct sockaddr *) &service, sizeof(service)) < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("bind() failed.\n");
		close(ListenSocket);
		return 1;
	}
	if (listen( ListenSocket,3) < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error listening on socket.\n");
		close(ListenSocket);
		return 1;
	}
	printf("Done\n");
	printf("\nReady to accept connection on port %d\n", portno2);
	printf("Waiting for connection...\n");
	struct sockaddr_in sin; 
	int size = sizeof(sin);
	memset(&sin, 0, size);

	sock2 = accept( ListenSocket, (struct sockaddr*)&sin, (socklen_t *)&size );
	if (sock2 < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("accept failed\n");
		close(ListenSocket);
		return 1;
	}
	close(ListenSocket);
	printf("\nSever from %s at port %d connected\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
	//end of code for step 3
	//code for step 4
	char UDPport[12];
	memset(UDPport, 0, sizeof(UDPport));
	totalByteReceived = 0;
	byteToReceive = 11;
	int step3Result=0;
	do {
		step3Result = recv(sock2, UDPport+totalByteReceived, 11-totalByteReceived, 0);
		if ( step3Result > 0 ){
			printf("Bytes received: %d\n", step3Result);
			totalByteReceived += step3Result;
			if (totalByteReceived >= byteToReceive)
				break;
		}
		else if ( step3Result == 0 ){
			printf(" %s %d \n",strerror(errno), errno);
			printf("Connection closed\n");
			return 1;
		}
		else{
			printf(" %s %d \n",strerror(errno), errno);			
			printf("recv failed\n");
			return 1;
		}
	} while( step3Result > 0 );
	printf("UDP portno received: %s\n", UDPport);
	close(sock2);
	char port3[6];
	char s3[6];
	memset(port3, 0, sizeof(port3));
	memset(s3, 0, sizeof(s3));
	
	// for (int i = 1; i < 6; i++) {
	// 	s3[i] = UDPport[i];
	// 	port3[i] = UDPport[i+6];
	// 	printf("%s, %s\n",s3, port3);
	// 	printf("Answer %c\n",UDPport[i]);
	// 	printf("Answer %c\n",s3[i]);
	// 	if(i==5){
	// 		break;
	// 	}
	// }
	int i =0;
	while(1){
		*(s3+i)=*(UDPport+i);
		// printf("%s\n",s3);
		// printf("Answer %c\n",*(UDPport+i));
		// printf("Answer %c\n",*(s3+i));	
		i=i+1;	
		if(i==5){
		// 	printf("hey");
		break;
		}
	}
	i =0;
	while(1){
		
		*(port3+i)=*(UDPport+i+6);
		// printf("%s, %s\n",s3, port3);
		// printf("Answer %c\n",*(UDPport+i+6));
		// printf("Answer %c\n",*(port3+i));	
		i=i+1;	
		if(i==5){
			//printf("hey");
		break;
		}
	}

	// for (int i = 0; i <5; i++) {
	// 	port3[i] = UDPport[i+6];
	// }
	// printf("Ready to send to UDP port %s\n",s3);
	// printf("Ready to create socket 3 %s\n", s3);
	
	// printf("Ready to create socket 3 %s\n", port3);
	// printf("Ready to send to UDP portsdasdas %s\n",port3);





	//printf("-1\n");
	int portno3 = atoi(port3);
	int send_s3 = atoi(s3);
	//printf("0\n");
	sock3 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock3 < 0) {
		printf("Error at socket()\n");
		return 1;
	}
	//printf("1\n");
	char* robotIP = inet_ntoa(sin.sin_addr);
	struct sockaddr_in udpAddr;
	udpAddr.sin_family = AF_INET;
	printf("Ready to create socket 3 %s\n", port3);
	printf("port number client is %d\n",send_s3);
	udpAddr.sin_port = htons(portno3);
	udpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock3, (struct sockaddr *) &udpAddr, sizeof(udpAddr));
	//printf("2\n");
	struct sockaddr_in robotAddr;
	robotAddr.sin_family = AF_INET;
	robotAddr.sin_port = htons(send_s3);
	robotAddr.sin_addr.s_addr = inet_addr(robotIP);
	// printf("robot ip is  %c",robotIP);
	//printf("3\n");

	sleep(3);
	time_t t;
	srand((unsigned) time(&t));
	int num = rand()%5 + 5;
	printf("Sending num: %d to ROBOT on port %d\n", num, send_s3);
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%d", num);
    int sendnum = sendto(sock3, buffer, strlen(buffer), 0, (struct sockaddr*) &robotAddr, sizeof(robotAddr));
	printf("student send end\n");
	
	char buffer3[1024];
	memset(buffer3, 0, sizeof(buffer3));
	//end of //code for step 4
	memset(buffer3, 0, sizeof(buffer3));
	recv(sock3, buffer3, sizeof(buffer3), 0);
	sleep(10);
	printf("hey, %s from sever is sent", buffer3);
	int checksame = sendto(sock3, buffer3, strlen(buffer3), 0, (struct sockaddr*) &robotAddr, sizeof(robotAddr));
	sleep(1);

	close(sock3);
	

	return 0;
}