test: 
	g++ -g -Wall -std=c++20 src/tests/run_tests.cpp src/tests/test.cpp src/book/order_book.cpp -o test.a
	./test.a
	rm -rf ./test.a*

