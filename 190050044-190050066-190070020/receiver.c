#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>

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
	int recvport=atoi(argv[1]);
	int sendport=atoi(argv[2]);
	double pdp=atof(argv[3]);

	// socket creation and binding taken from geeksforgeeks
	int listenfd, len;
	struct sockaddr_in servaddr, cliaddr;
	bzero(&servaddr, sizeof(servaddr));

	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(recvport);
	servaddr.sin_family = AF_INET;

	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	len = sizeof(cliaddr);

	// file opening
	FILE *f = fopen("receiver.txt", "w");
	if (f == NULL){
		printf("Error opening receiver.txt!\n");
		exit(1);
	}

	int i=1;
	srand(time(0));
	while(1){
		// received message is stored in buffer
		char buffer[32];
		int n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
		// checks if final ending message is sent
		if(strlen(buffer)==3){
			break;
		}

		// packet dropping based on user given probability
		int recvbuff=convert_slice(buffer,7,strlen(buffer));
		if(recvbuff==i){
			double rng = (double)rand()/(double)((unsigned)RAND_MAX + 1);
			// printf("%f\n", rng);
			if(rng<pdp){
				printf("Packet:%d Dropped\n", i);
				fprintf(f, "Packet:%d Dropped\n", i);
				continue;
			}
			i++;
		}
		
		// concatenating int to string
		char is[32];
		sprintf(is,"%d",i);
		char message[32] = "Acknowledgment:";
		strcat(message,is);

		// message is sent and printed on terminal and txt file
		sendto(listenfd, message, 100, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		printf("%s\n", message);
		fprintf(f, "%s\n", message);
	}
	fclose(f);
} 
