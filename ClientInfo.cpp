#include "ClientInfo.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <utility>

namespace {
    std::string ip_from_socket(int socket) {
        struct sockaddr_in addr;
        socklen_t addr_size = sizeof(addr);

        if(-1 == getpeername(socket, (struct sockaddr*)&addr, &addr_size)) {
            //todo: print some error message
            return std::string();
        }
        return inet_ntoa(addr.sin_addr);
    }
}

//----------------------------------------------------------------------------------------
// class ClientInfo
//----------------------------------------------------------------------------------------
ClientInfo::ClientInfo(int socket, int i_id)
    : fd(socket)
    , create_time(std::chrono::steady_clock::now())
    , received_packets()
    , csv_writer(i_id) {

    ip_addr = std::move(ip_from_socket(socket));
}

double ClientInfo::GetElapsedTime() const {
    using namespace std::chrono;

    auto now = steady_clock::now();
    duration<double> diff = now - create_time;
    return diff.count();
}
