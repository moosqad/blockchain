build: blockchain.cpp server.cpp client.cpp
	g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system server.cpp -o server -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include
	g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system client.cpp -o client -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include

clean:
	rm -rf HELLO client.dSYM server.dSYM
	rm test.db ./client ./server a.out 
