#include "ClientsData.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

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
ClientInfo::ClientInfo(int socket)
    : fd(socket)
    , create_time(std::chrono::steady_clock::now())
    , received_packets() {

    ip_addr = std::move(ip_from_socket(socket));
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
ClientsData::ClientsData() {
    //TODO: change to some reasonable constant

    ip_addr.reserve(1000);
	elapsed_time.reserve(1000);
	received_packets.reserve(1000);
}

void ClientsData::AddNewClient(int socket) {
    //don't check the key exists at the moment

	std::lock_guard<std::mutex> lock(m);
	auto res = data.insert(std::pair<int, ClientInfo>(socket, socket));
	if(!res.second) {
		//TODO: handle element hasn't been inserted
	}

	//data[socket] = ClientInfo(socket);
}

void ClientsData::RemoveClient(int socket) {
	std::lock_guard<std::mutex> lock(m);
	data.erase(socket);
}

void ClientsData::OnPacketReceiveFrom(int socket) {
	std::lock_guard<std::mutex> lock(m);
	auto it = data.find(socket);
	if(it != std::end(data)) {
		it->second.ReactPacketArrived();
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

	int connected_clients_num = 0;
	{
		std::lock_guard<std::mutex> lock(m);
		connected_clients_num = data.size();

		for(auto& key_val: data) {
			const auto& client_info = key_val.second;

			ip_addr.push_back(client_info.GetIpAddr());
			elapsed_time.push_back(client_info.GetElapsedTime());
			received_packets.push_back(client_info.GetReceivedPacketsNum());
		}
	}

	printf("connected clients # = %d:\n", connected_clients_num);
	int n = ip_addr.size();
	for(int i = 0; i < n; ++i) {
		printf("\tclient_%d: %s, %fs, %d packets\n", i, ip_addr[i].c_str(), elapsed_time[i], received_packets[i]);
	}
}
