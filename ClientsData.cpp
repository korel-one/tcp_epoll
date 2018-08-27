#include "ClientsData.h"

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

    void parse_str(const std::string& str, int& n1, float& n2, int& n3) {
        std::stringstream token_stream(str);
        std::string s;

        std::getline(token_stream, s, ',');
        n1 = std::stoi(s);

        std::getline(token_stream, s, ',');
        n2 = std::stof(s);

        std::getline(token_stream, s, ',');
        n3 = std::stoi(s);
    }

    std::vector<int> create_vec_indices(size_t N) {
        std::vector<int> v(N);
        std::iota(std::begin(v), std::end(v), 0);

        return v;
    }

    template <typename T>
    bool sort_indices(const std::vector<T>& data, std::vector<int>& o_indices, bool less = true) {
        if (data.size() != o_indices.size()) return false;

        std::function<bool(const int, const int)> f1 = [&data](const int i1, const int i2) { return data[i1] < data[i2]; };
        std::function<bool(const int, const int)> f2 = [&data](const int i1, const int i2) { return data[i1] > data[i2]; };

        auto compare = less ? f1 : f2;

        std::sort(std::begin(o_indices), std::end(o_indices), compare);
        return true;
    }
}

//----------------------------------------------------------------------------------------
// class ClientInfo
//----------------------------------------------------------------------------------------
ClientInfo::ClientInfo(int socket, int i_id)
    : fd(socket)
    , create_time(std::chrono::steady_clock::now())
    , received_packets()
    , ip_addr()
    , id(i_id)
    , file_name("client_" + std::to_string(i_id)) {

    ip_addr = std::move(ip_from_socket(socket));

    v_n1.reserve(1024);
    v_n2.reserve(1024);
    v_n3.reserve(1024);
}

ClientInfo::~ClientInfo() {
    if (p_file) {
        fclose(p_file);
    }

    auto N = v_n1.size();

    // 1. operate with v_n1
    auto indices1 = create_vec_indices(N);
    sort_indices(v_n1, indices1);
    SaveToFile(indices1, file_name + "_n1.csv");

    //2. operate with v_n2
    auto indices2 = create_vec_indices(N);
    sort_indices(v_n2, indices2, false);
    SaveToFile(indices2, file_name + "_n2.csv");

    // 3. operate with v_n3
    auto indices3 = create_vec_indices(N);
    sort_indices(v_n3, indices3);
    SaveToFile(indices3, file_name + "_n3.csv");
}

void ClientInfo::WriteLine(std::string&& line) {
    if (!p_file) {
	if(!(p_file = fopen((file_name + ".csv").c_str(), "a"))) {
            std::cout << "ERROR: file " << file_name << " was not opened\n";
        }
    }
    fprintf(p_file, "%s\n", line.c_str());

    parse_str(line, n1, n2, n3);
    v_n1.push_back(n1);
    v_n2.push_back(n2);
    v_n3.push_back(n3);
}

void ClientInfo::SaveToFile(const std::vector<int>& indices, const std::string& i_file_name) {
    FILE* p_f = fopen(i_file_name.c_str(), "a");
    if (!p_f) {
        std::cout << "ERROR: file " << i_file_name << " was not opened\n";
    }

    char buf[128] = {0};
    for (auto i : indices) {
        sprintf(buf, "%d,%f,%d", v_n1[i], v_n2[i], v_n3[i]);
        fprintf(p_f, "%s\n", buf);
    }

    fclose(p_f);
}

double ClientInfo::GetElapsedTime() const {
    using namespace std::chrono;

    auto now = steady_clock::now();
    duration<double> diff = now - create_time;
    return diff.count();
}

//----------------------------------------------------------------------------------------
// class ClientsData
//----------------------------------------------------------------------------------------
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
    auto res = data.insert(std::pair<int, ClientInfo>(socket, {socket, NextClientId()}));
    if(!res.second) {
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
    if(it != std::end(data)) {
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

        for(auto& key_val: data) {
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
    for(int i = 0; i < n; ++i) {
        printf("\tclient_%d: %s, %fs, %d packets\n", client_id[i], ip_addr[i].c_str(), elapsed_time[i], received_packets[i]);
    }
}
