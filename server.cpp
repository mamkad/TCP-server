#include "server.hpp"
#include "temptologfile.hpp"

#include <thread>
#include <fstream>

using std::thread;
using std::ofstream;

server::server(string const& ipa, string const& dataprt, string const& codeprt, size_t bklog, string const& logfilen, function<uint16_t(uint16_t)> const& encrypt_func)
try : netbase(ipa, dataprt), codeport(codeprt), backlog(bklog), logfilename(logfilen), encrypt_function(encrypt_func)
{
	set_address(code_address, codeprt);
	set_socket(code_socketfd);
	set_listen(main_address, main_socketfd);
	set_listen(code_address, code_socketfd);

	cout << '\n' << "server is created" << '\n';
	cout << "ip: " << ip << '\n';
	cout << "data port: " << port << '\n';
	cout << "code port: " << codeport << '\n' << '\n';
}
catch(exception const& e) 
{
	cout << e.what() << '\n';
	throw logic_error("server is not created");
}

server::~server() 
{
	cout << "server is destroyed" << '\n';
}

void server::set_listen(sockaddr_in& adrs, int fd)
{
	if (bind(fd, (struct sockaddr*)&adrs, sizeof(adrs)) < 0)
   	 	throw logic_error("error of bind function"); 

  	if (listen(fd, backlog) < 0)
    	throw logic_error("error of listen function"); 
}

int server::accept_connection(int fd)
{
	int newfd;
	if ( (newfd = accept(fd, (struct sockaddr*)NULL, NULL)) < 0 ) 
		throw logic_error("error of accept function");

	return newfd;
}

void server::close_connection(int fd)
{
	if (close(fd) < 0)
		throw logic_error("error of close connection");
}

void server::listen_codeport()
{
	uint16_t id, code;
	int connection_fd;

	for(;;) {
		cout << "wait connections to code port...." << '\n';

		try {
			connection_fd = accept_connection(code_socketfd);

		} catch (exception const &e) {
			cout << e.what() << '\n';
			continue;
		}

		cout << '\n' << "got connection to code port" << '\n';

		try {
			// read id from user
			recv_data(connection_fd, (char*)&id, DEFAULTCODEIDSIZE); 

			// getting code
			code = encrypt_function(id);

			// send code to user
			send_data(connection_fd, (char*)&code, DEFAULTCODEIDSIZE);

			cout << id << ' ' << code << '\n';
			close_connection(connection_fd);

		} catch(exception const &e) {
			cout << e.what() << '\n';
			close_connection(connection_fd);
		}
	}
}

void server::listen_dataport()
{
	int connection_fd;
	uint16_t id, code, status;
	uint64_t buffsize;

	for(;;) {
		cout << "wait connections to data port...." << '\n';

		try {
			connection_fd = accept_connection(code_socketfd);

		} catch (exception const &e) {
			cout << e.what() << '\n';
			continue;
		}

		cout << '\n' << "got connection to data port" << '\n';

		try {
			// read id
			recv_data(connection_fd, (char*)&id,   DEFAULTCODEIDSIZE);

			// read code
			recv_data(connection_fd, (char*)&code, DEFAULTCODEIDSIZE);

			// check correct of code
			check_code(id, code);
			   
			// read size
			recv_data(connection_fd, (char*)&buffsize, SIZE);

			// make buffer
			char * buff = new char[buffsize];

			// read buffer
			recv_data(connection_fd, buff, buffsize);

			// save to log
			savetolog(id, code, buff, buffsize);

			delete[] buff;
			
			// send result of connection
			send_data(connection_fd, (char*)&status, DEFAULTCODEIDSIZE);

			close_connection(connection_fd);

		} catch(exception const &e) {
			cout << e.what() << endl;
			close_connection(connection_fd);
		}
	}
}

void server::savetolog(uint16_t id, uint16_t code, char* buff, uint64_t bsize)
{
	ofstream fout(logfilename, std::ios_base::binary | std::ios_base::app);

	if (!fout)
		throw logic_error("file " + logfilename + " is not open");

  	fout << LINE << '\n';
  	fout << SPC << "connection " << '\n'       << '\n';
  	fout << SPC << "user id:   " << id         << '\n';
  	fout << SPC << "user code: " << code       << '\n';
  	fout << SPC << "time:      " << get_date() << '\n' << '\n';
  	fout.write(buff, bsize);
  	fout << LINE << '\n';

	fout.close();
}

void server::check_code(uint16_t id, uint16_t code)
{
	if (encrypt_function(id) != code ) 
		throw logic_error("wrong code");
}

int server::execute()
{
	thread codeport_thread(&server::listen_codeport, this);
	thread dataport_thread(&server::listen_dataport, this);

	codeport_thread.join();
	dataport_thread.join();

	return 0;
}