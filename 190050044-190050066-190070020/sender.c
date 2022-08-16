#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <poll.h>

// function to convert part of string to int
int convert_slice(const char *s, size_t a, size_t b){
	int val = 0;
	while(a<b){
		val = val*10+s[a++]-'0';
	}
	return val;
}

int main(int argc, char *argv[]){
	// input parameters
	int sendport=atoi(argv[1]);
	int recvport=atoi(argv[2]);
	int retranstimer=atoi(argv[3]);
	int packets=atoi(argv[4]);

	// socket creation and binding taken from geeksforgeeks
	int sockfd;
	struct sockaddr_in servaddr;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(recvport);
	servaddr.sin_family = AF_INET;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// file opening
	FILE *f = fopen("sender.txt", "w");
	if (f == NULL){
		printf("Error opening sender.txt!\n");
    	exit(1);
	}

	int i=1;
	while(i<=packets){
		// concatenating int to string
		char is[32];
		sprintf(is,"%d",i);
		char message[32] = "Packet:";
		strcat(message,is);

		// message is sent and printed on terminal and txt file
		sendto(sockfd, message, 100, 0, (struct sockaddr*)NULL, sizeof(servaddr));
		printf("%s\n", message);
		fprintf(f, "%s\n", message);
		// sending time is recorded
		struct timeval before;
		gettimeofday(&before,NULL);
		// printf("%ld\n", before.tv_sec);

		// retransmission timer is set
		struct pollfd fd;
		fd.fd = sockfd;
		fd.events = POLLIN;
		int res;
		res = poll(&fd, 1, 1000*retranstimer);
		// printf("%d\n", res);
		// packet is retransmitted in case of no ACK received
		if(res == 0){
			printf("Packet:%d Timer Expired\n", i);
			fprintf(f, "Packet:%d Timer Expired\n", i);
			continue;
		}

		// received ACK is stored in buffer
		char buffer[32];
		int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		int recvbuff=convert_slice(buffer,15,strlen(buffer));
		// i is incremented by 1 in case correct ACK is received
		if(recvbuff==i+1){
			i++;
		}
		else{
			// ACK is ignored in case it is incorrect
			// receiving time is recorded and new timer is set
			struct timeval after;
			gettimeofday(&after,NULL);
			// printf("%ld\n", after.tv_sec);
			int difference = after.tv_sec - before.tv_sec;
			// printf("%d\n", difference);
			int newtimer = retranstimer - difference;
			// printf("%d\n", newtimer);
			res = poll(&fd, 1, 1000*newtimer);
			if(res == 0){
				printf("Packet:%d Timer Expired\n", i);
				fprintf(f, "Packet:%d Timer Expired\n", i);
				continue;
			}
		}
	}
	//  final ending message is sent
	sendto(sockfd, "end", 100, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	fclose(f);
	close(sockfd); 
} 
