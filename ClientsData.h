#pragma once

#include <chrono>
#include <string>
#include <map>
#include <mutex>
#include <vector>

struct ClientInfo {

public:
    ClientInfo(int socket, int i_id);
    ~ClientInfo();

    // returns elapsed time in [s]
    double GetElapsedTime() const;

    const std::string& GetIpAddr() const { return ip_addr; }
    int GetReceivedPacketsNum() const { return received_packets; }
    void ReactPacketArrived() { ++received_packets; }

    void WriteLine(const std::string& line);

    const int GetId() const { return id; }

private:
    void SaveToFile(const std::vector<int>& indices, const std::string& i_file_name);

private:
    const int fd;
    const std::chrono::time_point<std::chrono::steady_clock> create_time;
    int received_packets;

    std::string ip_addr;

    //csv members
    const int id;
    const std::string file_name;
    FILE* p_file = nullptr;

    std::vector<int> v_n1, v_n3;
    std::vector<float> v_n2;

    //aux members
    int n1, n3;
    float n2;
};

class ClientsData {
    public:
        ClientsData();

        void AddNewClient(int socket);
        void RemoveClient(int socket);

        int GetConnectedClientsNum();
        void Print();

        void OnPacketReceiveFrom(int socket);

    private:
        int NextClientId() const { return curr_client_id++; }

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
