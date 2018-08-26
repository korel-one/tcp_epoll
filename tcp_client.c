#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

void error(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

//assume lower is 0
void generate_random(const int upper_bound, char* out, const size_t out_sz, const char delim) {
	int n1 = rand()%(upper_bound + 1);
	int n3 = rand()%(upper_bound + 1);
	float n2 = ((float)rand()/RAND_MAX)*upper_bound;

	memset(out, 0, out_sz);

	//don't check a possible failure
	sprintf(out, "%d%c%f%c%d", n1, delim, n2, delim, n3);
}

#define DEFAULT_PACKETS_NUM 3

int main(int argc, char* argv[]) {

	// handle command line

	char* host_addr = strtok(argv[1], ":");
	char* port_str = strtok(NULL, ":");
	int port = atoi(port_str);

	int max_val = atoi(argv[2]);
	int packets_num = (argc == 4) ? atoi(argv[3]) : DEFAULT_PACKETS_NUM; 

	// connect to TCP server

	int sock;
	struct sockaddr_in server;

	// create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		error("ERROR on creating a client socket");
	}

	server.sin_addr.s_addr = inet_addr(host_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	// connect to the remote server
	if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		error("ERROR on connection to the server");
	}

	//seed
	srand(time(0));
	char buf[256];
	char reply[256];

	for(int i = 0; i < packets_num; ++i) {
		generate_random(max_val, buf, sizeof(buf), ',');

		if(send(sock, buf, strlen(buf), 0) < 0) {
			error("ERROR on sending");
		}
		else {
			printf("sent_%d: %s\n", i, buf);
		}

		/*if(recv(sock, reply, sizeof(reply), 0) < 0) {
			close(sock);
			error("ERROR on receiving");
		}*/
		usleep(500*1000);
	}

	close(sock);
	return 0;
}
