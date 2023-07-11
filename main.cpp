#include "main.hpp"
#include "test.cpp"
// #include "test.hpp"

using namespace std; 

int main(int argc, char** argv){
    Testing tests = Testing();
    if (
            // tests.test_1() &&       // prove the orderbook can add a buy  order 
            // tests.test_2() &&       // prove the orderbook can add a sell order
            
            // tests.test_3() &&       // prove a buy  order can create a match with a sell order
            // tests.test_4() &&       // prove a sell order can create a match with a buy  order
            
            // tests.test_5() &&       // prove a buy  order can change limit nodes to fill orders
            // tests.test_6() &&       // prove a sell order can change limit nodes to fill orders
            
            // tests.test_7() &&       // prove a buy  order can partially fill a sell order 
            // tests.test_8() &&       // prove a sell order can partially fill a buy  order 

            // tests.test_9() &&       // prove a buy  order can be filled by traversing a DLL of orders of a single limit node 
            // tests.test_10() &&      // prove a sell order can be filled by traversing a DLL of orders of a single limit node 

            // tests.test_11() &&      // prove a buy  order can be filled when it crosses the spread at a new, very high limit price
            // tests.test_12()         // prove a sell order can be filled when it crosses the spread at a new, very low  limit price

            tests.test_11()

        ){ cout << "<--all tests passed-->" << endl;} 
    else {cout << "failure." << endl;}

    return 0;
}