#pragma once

#include <chrono>
#include <string>
#include <map>
#include <mutex>
#include <vector>

struct ClientInfo {
	explicit ClientInfo(int socket);
	//ClientInfo& operator=(const ClientInfo&) = default;

	// returns elapsed time in [s]
	double GetElapsedTime() const;

	const std::string& GetIpAddr() const { return ip_addr; }
	int GetReceivedPacketsNum() const { return received_packets; }
	void ReactPacketArrived() { ++received_packets; }

private:
	const int fd;
	const std::chrono::time_point<std::chrono::steady_clock> create_time;
	int received_packets;

	std::string ip_addr;
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
		std::map<int, ClientInfo> data;
		std::mutex m;

	// aux containers
		std::vector<std::string> ip_addr;
		std::vector<double> elapsed_time;
		std::vector<int> received_packets;
};
