#include "main.hpp"
#include "test.cpp"
// #include "test.hpp"

using namespace std; 

int main(int argc, char** argv){
    Testing tests = Testing();
    if (
            // tests.test_1() &&
            // tests.test_2() &&
            // tests.test_3() &&
            // tests.test_4() &&
            // tests.test_5() && 
            tests.test_6()
        ){ cout << "<--all tests passed-->" << endl;} else {cout << "failure." << endl;}

    return 0;
}