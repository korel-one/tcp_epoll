#pragma once

#include "CSVWriter.h"

#include <chrono>
#include <string>
#include <map>
#include <mutex>
#include <vector>

/* ------------------------------------------------------------------------------
struct ClientInfo is a data keeper class that stores all the information received 
from a tcp_client provess.

It gets updated every time a tcp_server receives a packet from a tcp_client.
------------------------------------------------------------------------------ */
struct ClientInfo {

public:
    ClientInfo(int socket, int i_id);

    // returns elapsed time in [s]
    double GetElapsedTime() const;

    const std::string& GetIpAddr() const { return ip_addr; }
    int GetReceivedPacketsNum() const { return received_packets; }
    void ReactPacketArrived() { ++received_packets; }

    void WriteLine(std::string&& line) {
        csv_writer.WriteLine(std::forward<std::string>(line));
    }

    const int GetId() const { return csv_writer.GetId(); }

private:
    const int fd;
    const std::chrono::time_point<std::chrono::steady_clock> create_time;
    int received_packets;

    std::string ip_addr;

    CSVWriter csv_writer;
};