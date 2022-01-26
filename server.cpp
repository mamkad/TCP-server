#include "server.hpp"
#include "temptologfile.hpp"

#include <thread>
#include <exception>
#include <fstream>

using std::thread;
using std::exception;
using std::ofstream;

server::server(string const& ipa, string const& dataprt, string const& codeprt, size_t bklog, string const& logfilen, function<uint16_t(uint16_t)> const& encrypt_func)
try : netbase(ipa, dataprt), codeport(codeprt), backlog(bklog), logfilename(logfilen), encrypt_function(encrypt_func)
{
	set_address(code_address, codeprt);
	set_socket(code_socketfd);
	set_listen(main_address, main_socketfd);
	set_listen(code_address, code_socketfd);

	cout << endl << "server is created" << endl;
	cout << "ip: " << ip << endl;
	cout << "data port: " << port << endl;
	cout << "code port: " << codeport << endl << endl;
}
catch(...) 
{
	cout << "server is not created"<< endl;
	throw;
}

server::~server() {}

void server::set_listen(sockaddr_in& adrs, int fd)
{
	if (bind(fd, (struct sockaddr*)&adrs, sizeof(adrs)) < 0) {
    	cout << "error of bind function" << endl;
   	 	throw; 
  	}
 
  	if (listen(fd, backlog) < 0) {
    	cout  << "error of listen function" << endl;
    	throw; 
  	}
}

int server::accept_connection(int fd)
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
	int connection_fd;
	uint16_t id, code;

	for(;;) {
		try {
			cout << "wait connections to code port...." << endl;
			
			connection_fd = accept_connection(code_socketfd);

			cout << endl << "got connection to code port" << endl;

			// read id from user
			recv_data(connection_fd, (char*)&id, DEFAULTCODEIDSIZE); 

			// getting code
			code = encrypt_function(id);

			// send code to user
			send_data(connection_fd, (char*)&code, DEFAULTCODEIDSIZE);

			cout << id << ' ' << code << endl;

		} catch(exception const &e) {
			cout << "listen_codeport error" << endl;
			cout << "error of send/recv data" << endl;
			cout << e.what() << endl;
		}
		close(connection_fd);
	}
}

void server::listen_dataport()
{
	int connection_fd;
	uint16_t id, code, status;
	uint64_t buffsize;

	for(;;) {
		try {
			cout << "wait connections to data port...." << endl;

			connection_fd = accept_connection(main_socketfd);

			cout << endl << "got connection to data port" << endl;

			// read id
			recv_data(connection_fd, (char*)&id,   DEFAULTCODEIDSIZE);

			// read code
			recv_data(connection_fd, (char*)&code, DEFAULTCODEIDSIZE);

			// check correct of code
			status = 0; 

			if (encrypt_function(id) != code ) {
				cout << "wrong code" << endl;
				cout << "close connection" << endl;
				status = 1;
			} else							   {
				// read size
				recv_data(connection_fd, (char*)&buffsize, SIZE);

				// read buffer
				shared_ptr<char[]> buff = make_shared<char[]>(buffsize);
				recv_data(connection_fd, buff.get(), buffsize);
				// save to log
				savetolog(id, code, buff.get(), buffsize);
			}

			// send result of connection
			send_data(connection_fd, (char*)&status, DEFAULTCODEIDSIZE);

		} catch(exception const &e) {
			cout << "listen dataport error" << endl;
			cout << "error of send/recv data" << endl;
			cout << e.what() << endl;
		}
		close(connection_fd);
	}
}

void server::savetolog(uint16_t id, uint16_t code, char* buff, uint64_t bsize)
{
	ofstream fout(logfilename, std::ios_base::binary | std::ios_base::app);

	if (!fout) {
		cout << "file " << logfilename << " is not open"<< endl;
		throw;
  	}

  	fout << LINE << endl;
  	fout << SPC << "connection " << endl       << endl;
  	fout << SPC << "user id:   " << id         << endl;
  	fout << SPC << "user code: " << code       << endl;
  	fout << SPC << "time:      " << get_date() << endl << endl;
  	fout.write(buff, bsize);
  	fout << LINE << endl;

	fout.close();
}

int server::execute()
{
	thread codeport_thread(&server::listen_codeport, this);
	thread dataport_thread(&server::listen_dataport, this);

	codeport_thread.join();
	dataport_thread.join();

	return 0;
}