# Client in C and Server in C++

## Internals
- Makefile
- run_clients.sh
- source files ( *.cpp/ *.c, *.h)

## Environment Setup:

- Check if package index is up to date: **sudo apt-get update**
- Install build-essential: **sudo apt-get install build-essential**

## **Before launch:**
- make
- mkdir csv


## **Launch**
* tcp server - simply run ./tcp_server [port] 
	* ex:     **./tcp_server 8888**
  
* tcp clients - use *run_clients.sh*
	* to see required arguments list **./run_clients.sh**
	* ex: **./run_clients.sh 127.0.0.1:8888 2 100 5**
		* ***127.0.0.1:8888*** server address and port
		* ***2*** number of tcp clients to run in parallel
		* ***100*** is an apper bound for random numbers (0 assumed as  a lower bound)
		* ***5*** is a number of packets each client will send to the server (default value = 3)
    
## NOTE:
  Since all the produced csv files are stored into **./csv** directory, its more convenient to clean-up the directory before
  next launch of **run_clients.sh**.
