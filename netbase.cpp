#include "netbase.hpp"

#include <chrono>      
#include <sstream>     
#include <iomanip>

netbase::netbase(string const& ipa, string const& prt)
try : ip(ipa), port(prt)
{
	set_address(main_address, port);
	set_socket(main_socketfd);
}
catch(exception const& e) 
{
	cout << e.what() << '\n';
	throw logic_error("class netbase is not created");
}

netbase::~netbase() 
{
	cout << "netbase is destroyed" << '\n';
}

void netbase::set_address(sockaddr_in& adrs, string const& prt)
{
  	memset(&adrs, 0, sizeof(adrs)); 
  	adrs.sin_family = AF_INET;         //(IPv4)

  	if (inet_pton(AF_INET, ip.c_str(), &adrs.sin_addr) <= 0) 
  		throw logic_error("inet_pton error function");
  
  	int intport = std::stoi(prt);

  	if (!intport) 
  		throw logic_error("port must be a numeric value greater than zero");
    
    adrs.sin_port = htons(intport);
}

void netbase::set_socket(int& sockfd)
{
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    	throw logic_error("error of create socket function");
}

void netbase::send_data(int fd, char* buff, size_t size)
{
	if (send(fd, buff, size, 0) < 0)
		throw logic_error("error of send function");
}

void netbase::recv_data(int fd, char* buff, size_t size)
{
	if (recv(fd, buff, size, 0) == -1) 
		throw logic_error("error of recv function");
}

string netbase::get_date() const
{
	auto now = std::chrono::system_clock::now();
  	auto in_time_t = std::chrono::system_clock::to_time_t(now);

  	std::stringstream ss;
  	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
  	return ss.str();
}
