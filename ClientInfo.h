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

Members:
          - fd: socket file desctiptor
	  - create_time: timepoint when a ClientInfo object is instantiated
	  - ip_addr: tcp client ip address in a string format
          - csv_writer: saves received from tcp client data into *.csv files
------------------------------------------------------------------------------ */
struct ClientInfo {

public:

    /*
    Args: socket - socket file descriptor;
          i_id - a number that is used when save to csv file
    */
    ClientInfo(int socket, int i_id);

    // returns elapsed time in [s]
    double GetElapsedTime() const;

    // getters
    const std::string& GetIpAddr() const { return ip_addr; }
    int GetReceivedPacketsNum() const { return received_packets; }
    void ReactPacketArrived() { ++received_packets; }

    /*
       wrapper method, that asks csv writer to append 'line'
       to the end of corresponding csv file

       Args: line - comma separated n1,n2,n3
    */
    void WriteLine(std::string&& line) {
        csv_writer.WriteLine(std::forward<std::string>(line));
    }

    // wrapper id getter
    const int GetId() const { return csv_writer.GetId(); }

private:
    const int fd;
    const std::chrono::time_point<std::chrono::steady_clock> create_time;
    int received_packets;

    std::string ip_addr;

    CSVWriter csv_writer;
};
