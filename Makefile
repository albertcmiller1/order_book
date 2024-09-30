testcpp: 
	g++ -g -Wall -std=c++20 src/test/cpp/run_tests.cpp src/test/cpp/test.cpp src/main/cpp/order_book.cpp src/main/cpp/money.cpp -o test.a
	./test.a
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
	python3 src/test/features/steps/common.py

check: 
	ls -l ~/.local/share/virtualenvs/order_book*/lib/python3*/site-packages | grep book

book-version: 
	pip list | grep book | awk '{print $$2}'

lint: 
	pipenv run pylint --verbose src/