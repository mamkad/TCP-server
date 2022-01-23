#include "netbase.hpp"

server::server(string const& ipa, string const& dataprt, string const& codeprt, size_t bklog, string const& logfilen, function<uint16_t(uint16_t)> const& encrypt_func)
try : netbase(ipa, dataprt), codeport(codeprt), backlog(bklog), logfilename(logfilen), encrypt_function(encrypt_func)
{
	set_address(code_address, codeprt);
	set_socket(codesocketfd);
	set_listen(main_address, main_socketfd);
	set_listen(codeport, code_socketfd);
}
catch(...) 
{
	cout << "server is not created"<< endl;
	throw;
}

server::~server() {}

void server::set_listen(sockaddr_in& adrs, int fd)
{
	if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    	cout << "error of bind function" << endl;
   	 	throw; 
  	}
 
  	if (listen(fd, backlog) < 0) {
    	cout  << "error of listen function" << endl;
    	throw; 
  	}
}

int server::accept(int fd)
{
	int newfd;
	if ( (newfd = accept(fd, (struct sockaddr*)NULL, NULL)) < 0 ) {
		cout  << "error of accept function" << endl;
		throw;
	}
	return newfd;
}

void server::listen_codeport()
{
	for(;;) {
		int connection_fd;

		try        { connection_fd = accept(code_socketfd); } 
		catch(...) {
			cout << "stop works with this connection" << endl;
			continue;
		}

		cout << "got connection" << endl;


		if (!fork()) {
			try {
				close(code_socketfd);

				shared_ptr<char*> buff;
				alloc_buff(buff, sizeof(uint16_t));

				recv_data(connection_fd, buff, sizeof(uint16_t));

				uint16_t id;    
				memcpy(&id, *buff, sizeof(uint16_t));

				uint16_t code = encrypt_function(id);

				shared_ptr<char*> nbuff;	
				alloc_buff(nbuff, sizeof(uint16_t));

				memcpy(*nbuff, &code, sizeof(uint16_t));

				send_data(connection_fd, nbuff, sizeof(uint16_t));

				close(connection_fd);
				exit(0);
			} catch(...) {
				cout << "error of send/recv data" << endl;
				exit(1);
			}
		}
		close(connection_fd);
	}
}

int server::execute()
{
	thread codeport_thread(&server::listen_codeport, this);
	codeport_thread.join();
	return 0;
}