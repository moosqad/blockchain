CPP = g++
EFLAGS = -Wall -Wextra -Werror -std=gnu++17 -lsqlite3 -lcrypto -lboost_system
SSL = -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include

build: blockchain.cpp server.cpp
	$(CPP) $(EFLAGS) server.cpp -o server $(SSL)

blockchain: blockchain.cpp
	$(CPP) $(EFLAGS) blockchain.cpp -o blockchain $(SSL)

clean:
	rm -rf HELLO client.dSYM server.dSYM validity.dSYM bl.dSYM .vscode
	rm test.db ./client ./server a.out  blockchain.db ./bl ./validity .DS_Store
