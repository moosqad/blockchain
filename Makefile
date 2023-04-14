# g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system client.cpp -o client -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include
build: blockchain.cpp server2.cpp
	g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system server2.cpp -o server -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include

blockchain: blockchain.cpp
	g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system blockchain.cpp -o bl -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include -fsanitize=address -g

validity_check: blockchain.cpp validity_check.cpp
	g++ -std=gnu++17 -lsqlite3 -lcrypto -lboost_system validity_check.cpp -o validity -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include -fsanitize=address -g

clean:
	rm -rf HELLO client.dSYM server.dSYM validity.dSYM bl.dSYM .vscode
	rm test.db ./client ./server a.out  blockchain.db ./bl ./validity .DS_Store
