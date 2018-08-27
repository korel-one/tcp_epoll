#pragma once

#include "ClientInfo.h"

#include <map>

class ClientsData {
public:
    ClientsData();

    void AddNewClient(int socket);
    void RemoveClient(int socket);

    int GetConnectedClientsNum();
    void Print();

    void OnPacketReceiveFrom(int socket, const char* buf, size_t buf_size);

private:
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
