#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <memory>

struct Limit {
    std::string name;
    int age;
    Limit(std::string n, int a) : name(std::move(n)), age(std::move(a)) {}
};

struct CompareLimitMaxHeap {
    bool operator()(const std::unique_ptr<Limit>& p1, const std::unique_ptr<Limit>& p2) const {
        return p1->age < p2->age;
    }
};

struct CompareLimitMinHeap {
    bool operator()(const std::unique_ptr<Limit>& p1, const std::unique_ptr<Limit>& p2) const {
        return p1->age > p2->age;
    }
};