#ifndef NETBASE_HPP
#define NETBASE_HPP

#include <iostream>
#include <functional>
#include <memory>

using std::cin;
using std::cout
using std::endl;

using std::function;
using std::shared_ptr;
using std::make_shared;

class netbase
{
protected:
	string      ip;
	string      port;
	int         main_socketfd;
	sockaddr_in main_address;
public:
	netbase(string const& ipa, string const& prt);
	~netbase();
public:
	netbase(netbase&)  = delete;
	netbase(netbase&&) = delete;
	netbase& operator = (netbase&)  = delete;
	netbase& operator = (netbase&&) = delete;
protected:
	void set_address(sockaddr_in& adrs, string const& prt);
	void set_socket(int& sockfd);
protected:
	void send_data(int fd, shared_ptr<char*>& buff, ize_t size);
	void recv_data(int fd, shared_ptr<char*>& buff, size_t size);
protected:
	void alloc_buff(make_shared<char*>& buff, size_t size);
};

#endif
