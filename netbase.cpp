#include "netbase.hpp"

netbase::netbase(string const& ipa, string const& prt)
try : ip(ipa), port(prt)
{
	set_address(main_address, port);
	set_socket(main_socketfd);
}
catch(...) 
{
	cout << "class netbase is not created"<< endl;
	throw;
}

netbase::~netbase() {}

void netbase::set_address(sockaddr_in& adrs, string const& prt)
{
  	memset(&adrs, 0, sizeof(adrs)); 
  	adrs.sin_family = AF_INET;         //(IPv4)

  	if (inet_pton(AF_INET, ip.c_str(), &adrs.sin_addr) <= 0) {
  		cout << "inet_pton error function" << endl;
  		throw;
  	}

  	intport = std::stoi(prt);

  	if (!intport) {
  		cout  << "port must be a numeric value greater than zero" << endl;
      	throw;
    }

    adrs.sin_port = htons(intport);
}

void netbase::set_socket(int& sockfd)
{
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	cout  << "error of create socket function" << endl;
    	throw;
  	}
}

void netbase::send_data(int fd, shared_ptr<char*>& buff, size_t size)
{
	if (send(fd, buff, size, 0) < 0) {
		cout  << "error of send function" << endl;
		throw;
	}

	return buff;
		
}

void netbase::recv_data(int fd, shared_ptr<char*>& buff, size_t size)
{

	if (recv(fd, buff, size, 0) == -1) {
		cout  << "error of recv function" << endl;
		throw;
	}

	return buff;
}

void netbase::alloc_buff(make_shared<char*>& buff, size_t size)
{
	try { 
		buff = make_shared<char*>(size); 
	} catch(exceptions const &e) {
		cout << e.whaw() << endl;
		throw;
	}
}