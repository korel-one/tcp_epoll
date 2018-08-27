#include "ClientsData.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>

#define MAX_EVENTS 32

void error(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}

//non-blocking mode
int set_nonblock(int fd) {
    int flags;

    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
	error("ERROR: port is not specified as a command line argument");
    }

    int port = std::atoi(argv[1]);

    int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(master_socket == -1) {
        error("ERROR on creating a socket");
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(master_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
        error("ERROR on binding to the address");
    }

    set_nonblock(master_socket);

    if(-1 == listen(master_socket, SOMAXCONN)) {
	error("ERROR on listening");
    }

    // create epoll descriptors
    int epoll_fd = epoll_create1(0);
    if(-1 == epoll_fd) {
	error("ERROR on creating epoll");
    }

    // register epoll_event in epoll
    struct epoll_event event;
    event.data.fd = master_socket;
    event.events = EPOLLIN;
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master_socket, &event)) {
        close(epoll_fd);
	close(master_socket);
        error("ERROR on registering event in epoll");
    }

    // keeps all connected clients information
    std::shared_ptr<ClientsData> p_clients_data = std::make_shared<ClientsData>();

    std::thread t([c_data = p_clients_data](){
        using namespace std::chrono_literals;

        while(true) {
            c_data->Print();
            std::this_thread::sleep_for(1s);
        }
    });


    while(true) {

        struct epoll_event events[MAX_EVENTS];

        // get N events
        int N = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for(unsigned int i = 0; i < N; ++i) {
            if(events[i].data.fd == master_socket) {
                int slave_socket = accept(master_socket, 0, 0);
                set_nonblock(slave_socket);

                p_clients_data->AddNewClient(slave_socket);

                struct epoll_event _event;
                _event.data.fd = slave_socket;
                _event.events = EPOLLIN;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, slave_socket, &_event);
            }
            else {
                // try to read from socket
                static char buffer[1024];
                int recv_result = recv(events[i].data.fd, buffer, 1024, MSG_NOSIGNAL);

                if(recv_result == 0 && errno != EAGAIN) {

                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);

                    p_clients_data->RemoveClient(events[i].data.fd);
                }
                else if(recv_result > 0) {

                    // TODO: get to know first whether all the data sent by a client has read, then increment a packets_num
                    // simplification: increment a packets_num every read, due to tiny message size
                    p_clients_data->OnPacketReceiveFrom(events[i].data.fd, buffer, recv_result);
                }

            }
        }
    }

    //TODO: refactor to RAII
    t.join();

    close(epoll_fd);
    close(master_socket);

    return 0;
}
