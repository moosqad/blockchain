build: blockchain.cpp server.cpp client.cpp
	g++ -std=gnu++17 -lsqlite3 -lssl -lcrypto -lssl server.cpp -o server -I /opt/goinfre/photoker/homebrew/Cellar/boost/1.81.0_1/include/
	g++ -std=gnu++17 -lsqlite3 -lssl -lcrypto -lssl client.cpp -o client -I /opt/goinfre/photoker/homebrew/Cellar/boost/1.81.0_1/include/

clean:
	rm -rf HELLO client.dSYM server.dSYM
	rm test.db ./client ./server a.out 
