build: blockchain.cpp
	g++ -std=gnu++17 -lsqlite3 blockchain.cpp && ./a.out

clean:
	rm -rf HELLO
	rm test.db a.out
