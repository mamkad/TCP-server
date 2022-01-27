CMP = g++
VER = c++11

server: main.o server.o netbase.o
	$(CMP) -o server main.o server.o netbase.o -std=$(VER) -pthread

main.o: main.cpp server.hpp
	$(CMP) -c main.cpp

server.o: server.cpp server.hpp temptologfile.hpp
	$(CMP) -c server.cpp

netbase.o: netbase.cpp netbase.hpp
	$(CMP) -c netbase.cpp
