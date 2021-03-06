CXX=g++

main:tcpclient.o main.o config.o datastruct.o
	$(CXX) -o main main.o tcpclient.o config.o datastruct.o -lpthread

main.o:tcpclient.h main.cpp datastruct.h
	$(CXX) -c -g main.cpp

tcpclient.o: datastruct.h tcpclient.h tcpclient.cpp
	$(CXX) -c -g tcpclient.cpp

config.o:datastruct.h config.h config.cpp
	$(CXX) -c -g config.cpp

datastruct.o:datastruct.cpp datastruct.h
	$(CXX) -c -g datastruct.cpp

clean:
	rm main.o tcpclient.o config.o main
