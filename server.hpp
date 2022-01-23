#ifndef SERVER_HPP
#define SERVER_HPP

#include "netbase.hpp"

class server : public netbase
{
private:
	string       codeport;
	size_t       backlog;
	string       logfilename;
	sockaddr_in  code_address;
	int          code_socketfd;

	function<uint16_t(uint16_t)> encrypt_function;

public:
	server(string const& ipa, 
		   string const& dataprt, 
		   string const& codeprt, 
		   size_t 		 bklog, 
		   string const& logfilen, 
		   function<uint16_t(uint16_t)> const& encrypt_func);
	~server();
private:
	void set_listen(sockaddr_in& adrs, int fd);
private:
	int accept(int fd);
	void listen_codeport();
public:
	int execute();
};

#endif