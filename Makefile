CXX=g++

main:tcpclient.o main.o
	$(CXX) -o main main.o tcpclient.o -lpthread

main.o:tcpclient.h main.cpp datastruct.h
	$(CXX) -c -g main.cpp

tcpclient.o: datastruct.h tcpclient.h tcpclient.cpp
	$(CXX) -c -g tcpclient.cpp

clean:
	rm main.o tcpclient.o main
