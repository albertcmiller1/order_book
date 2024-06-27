test: 
	g++ -g -Wall -std=c++20 src/test/cpp/run_tests.cpp src/test/cpp/test.cpp src/main/cpp/order_book.cpp -o test.a
	./test.a
	rm -rf ./test.a*

