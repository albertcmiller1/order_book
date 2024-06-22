#include "mymath.hpp"

int add(int i, int j) {
    return i + j;
}

std::string return_str(std::string var){
    return var;
}


bool val_in_set(){
    std::set<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    return set.count(3);
}

const std::string Pet::getName(){
    return this->name;
}

void Pet::setName(const std::string &name_) {
    this->name = name_;
}

