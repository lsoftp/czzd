CXX=g++

main:tcpclient.o main.o
	$(CXX) -o main main.o tcpclient.o

main.o:tcpclient.h main.cpp
	$(CXX) -c main.cpp

tcpclient.o: datastruct.h tcpclient.h tcpclient.cpp
	$(CXX) -c tcpclient.cpp

clean:
	rm main.o tcpclient.o main
