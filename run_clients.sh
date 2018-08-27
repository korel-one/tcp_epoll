#!/bin/bash

# mandatory arguments
host_addr_port=$1
num_clients=$2
max_value=$3

if [ $# -eq 0 ] || [ -z $host_addr_port ] || [ -z $num_clients ] || [ -z $max_value ]
then
    echo "ERROR! Run as: $0 [host_address:port] [number_of_clients] [value_upper_bound] [max_packets_number {by default = 3}]"
    exit 1
fi

max_num_packets=$4
for((i=0;i < $num_clients;i++));
do
  if [ -z $max_num_packets ]
  then
      ./tcp_client $host_addr_port $max_value &
  else
      ./tcp_client $host_addr_port $max_value $max_num_packets &
  fi
done
