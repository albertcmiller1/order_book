#include "main.hpp"
#include "test.cpp"
// #include "test.hpp"

using namespace std; 

int main(int argc, char** argv){
    OrderBook *book = new OrderBook;
    Testing tests = Testing();
    if (
            tests.test_1(book) &&
            tests.test_2(book) &&
            tests.test_3(book) 
        ){ cout << "<--all tests passed-->" << endl;} else {cout << "failure." << endl;}

    return 0;
}