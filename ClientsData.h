#pragma once

#include "ClientInfo.h"

#include <map>

/*-----------------------------------------------------------------------------
class CllientsData is responsible for adding  newly connected tcp clients and
for removing disconnected ones. It contains all the information received from
the currently connected clients. It forces received data to be saved and allows
to display tcp server statistics.

Members:
	  - curr_client_id: newly connected client id
	  - data: keeps all the information received from active clients
	  - m: mutex
	  - aux containers: are used when pring tcp server information
------------------------------------------------------------------------------*/

class ClientsData {
public:
    ClientsData();

    // Registers ClientInfo object w.r.t. newly connected client
    void AddNewClient(int socket);

    // Unregister ClientInfo object when corresponsing client disconnects
    void RemoveClient(int socket);

    // Returns total number of currently connected clients
    int GetConnectedClientsNum();

    // Displays tcp server information
    void Print();

    /*
       Handles the data when new packet is received w.r.t. tcp client
       Args: socket - socket file descriptor
             buf - received data
	     buf_size - received data size
    */
    void OnPacketReceiveFrom(int socket, const char* buf, size_t buf_size);

private:
    // Returns new client id
    int NextClientId() { return curr_client_id++; }

private:
    int curr_client_id;
    std::map<int, ClientInfo> data;

    std::mutex m;

    // aux containers
    std::vector<std::string> ip_addr;
    std::vector<double> elapsed_time;
    std::vector<int> received_packets;
    std::vector<int> client_id;
};
