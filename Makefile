testcpp: 
	g++ -g -Wall -std=c++20 src/test/cpp/run_tests.cpp src/test/cpp/test.cpp src/main/cpp/order_book.cpp -o test.a
	./test.a
	rm -rf ./test.a*

testcpp2: 
	g++ -g -Wall -std=c++20 src/test/cpp/run_tests2.cpp src/test/cpp/test2.cpp src/main/cpp/order_book2.cpp -o test2.a
	./test2.a
	rm -rf ./test.a*

compile: 
	rm -rf ./build
	mkdir build
	cd ./build/; cmake ..; make;

wheel: 
	pipenv uninstall book 
	rm -rf dist/
	python3 setup.py bdist_wheel
	pipenv install dist/*

run: 
	python3 src/main/py/main.py   

check: 
	ls -l ~/.local/share/virtualenvs/order_book*/lib/python3.12/site-packages | grep book

book-version: 
	pip list | grep book | awk '{print $$2}'

test2: 
	g++ -std=c++20 src/main/cpp/order_book2.cpp && ./a.out

lint: 
	pipenv run pylint --verbose src/