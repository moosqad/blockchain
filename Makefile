CPP = g++
EFLAGS = -Wall -Wextra -Werror
SFLAGS =  -std=gnu++17 -lsqlite3 -lcrypto -lboost_system
SSL = -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include

build: blockchain.cpp server.cpp
	$(CPP) $(EFLAGS) $(SFLAGS) server.cpp -o server $(SSL)

blockchain: blockchain.cpp
	$(CPP) $(EFLAGS) $(SFLAGS) blockchain.cpp -o blockchain $(SSL)

registration: registration.cpp
	$(CPP) $(EFLAGS) -lsqlite3 registration.cpp -o registration

clean:
	rm -rf HELLO client.dSYM server.dSYM validity.dSYM bl.dSYM .vscode 
	rm test.db ./client ./server a.out  blockchain.db ./blockchain ./validity .DS_Store ./registration users.db
