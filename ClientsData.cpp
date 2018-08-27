#include "ClientsData.h"

#include <stdio.h>

ClientsData::ClientsData()
    : client_id() {

    //TODO: change to some reasonable constant

    ip_addr.reserve(1024);
    elapsed_time.reserve(1024);
    received_packets.reserve(1024);
    client_id.reserve(1024);
}

void ClientsData::AddNewClient(int socket) {
    //don't check the key exists at the moment

    std::lock_guard<std::mutex> lock(m);
    auto res = data.insert(std::pair<int, ClientInfo>(socket, { socket, NextClientId() }));
    if (!res.second) {
        //TODO: handle element hasn't been inserted
    }
}

void ClientsData::RemoveClient(int socket) {
    std::lock_guard<std::mutex> lock(m);
    data.erase(socket);
}

void ClientsData::OnPacketReceiveFrom(int socket, const char* buf, size_t buf_size) {
    std::lock_guard<std::mutex> lock(m);
    auto it = data.find(socket);
    if (it != std::end(data)) {
        it->second.ReactPacketArrived();
        it->second.WriteLine(std::string(buf, buf_size));
    };
}

int ClientsData::GetConnectedClientsNum() {
    int n = 0;
    {
        std::lock_guard<std::mutex> lock(m);
        n = data.size();
    }
    return n;
}

void ClientsData::Print() {

    ip_addr.clear();
    elapsed_time.clear();
    received_packets.clear();
    client_id.clear();

    int connected_clients_num = 0;
    {
        std::lock_guard<std::mutex> lock(m);
        connected_clients_num = data.size();

        for (auto& key_val : data) {
            const auto& client_info = key_val.second;

            client_id.push_back(client_info.GetId());
            ip_addr.push_back(client_info.GetIpAddr());
            elapsed_time.push_back(client_info.GetElapsedTime());
            received_packets.push_back(client_info.GetReceivedPacketsNum());
        }
    }

    if (0 == connected_clients_num) return;

    printf("connected clients # = %d:\n", connected_clients_num);
    int n = ip_addr.size();
    for (int i = 0; i < n; ++i) {
        printf("\tclient_%d: %s, %fs, %d packets\n", client_id[i], ip_addr[i].c_str(), elapsed_time[i], received_packets[i]);
    }
}
