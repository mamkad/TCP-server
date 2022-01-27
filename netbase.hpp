#ifndef NETBASE_HPP
#define NETBASE_HPP

#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <exception>

using std::cin;
using std::cout;
using std::endl;

using std::function;
using std::shared_ptr;
using std::make_shared;
using std::string;

using std::logic_error;
using std::exception;

#include <unistd.h>     // read/write
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // inet_pton
#include <cstring>

class netbase
{
protected:
	string      ip;
	string      port;
	int         main_socketfd;
	sockaddr_in main_address;
protected:
	enum 
	{
		DEFAULTCODEIDSIZE = sizeof(uint16_t),	// size of id and code
		SIZE = sizeof(uint64_t)					// std length of buffer 
	};	
public:
	netbase(string const& ipa, string const& prt);
	~netbase();
protected:
	void set_address(sockaddr_in& adrs, string const& prt);
	void set_socket(int& sockfd);
protected:
	void send_data(int fd, /*shared_ptr<char[]>&*/ char* buff, size_t size);
	void recv_data(int fd, /*shared_ptr<char[]>&*/ char* buff, size_t size);
protected:
	string get_date() const;
};

#endif
