testcpp: 
	g++ -g -Wall -std=c++20 src/test/cpp/run_tests.cpp src/test/cpp/test.cpp src/main/cpp/order_book.cpp -o test.a
	./test.a
	rm -rf ./test.a*

build: 
	rm -rf ./build
	mkdir build
	cd ./build/; cmake ..; make;

wheel: 
	rm -rf dist/
	python3 setup.py bdist_wheel
	pipenv install dist/*


