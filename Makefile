CPP = g++
EFLAGS = -Wall -Wextra -Werror
SFLAGS =  -std=gnu++17 -lsqlite3 -lcrypto -lboost_system
SSL = -L/usr/local/Cellar/openssl@1.1/1.1.1t/lib -I/usr/local/Cellar/openssl@1.1/1.1.1t/include

build: blockchain.cpp server1.cpp server2.cpp
	$(CPP) $(EFLAGS) $(SFLAGS) server1.cpp -o server1 $(SSL)
	$(CPP) $(EFLAGS) $(SFLAGS) server2.cpp -o server2 $(SSL)

charity: charity.cpp
	$(CPP) $(EFLAGS) $(SFLAGS) charity.cpp -o charity $(SSL)

blockchain: blockchain.cpp
	$(CPP) $(EFLAGS) $(SFLAGS) blockchain.cpp -o blockchain $(SSL)

registration: registration.cpp
	$(CPP) $(EFLAGS) -lsqlite3 registration.cpp -o registration

clean:
	rm -rf HELLO client.dSYM server.dSYM validity.dSYM bl.dSYM .vscode
	rm ./server1 ./server2 charity.db-journal blockchain.db ./blockchain ./validity ./web_client/.DS_Store ./registration users.db charity.db ./charity
