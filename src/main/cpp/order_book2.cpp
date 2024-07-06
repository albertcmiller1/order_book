#include "order_book2.hpp"

int main() {
    std::priority_queue<std::unique_ptr<Limit>, std::vector<std::unique_ptr<Limit>>, CompareLimitMaxHeap> maxHeap;
    std::priority_queue<std::unique_ptr<Limit>, std::vector<std::unique_ptr<Limit>>, CompareLimitMinHeap> minHeap;

    maxHeap.push(std::make_unique<Limit>("Alice", 30));
    maxHeap.push(std::make_unique<Limit>("Bob", 25));
    maxHeap.push(std::make_unique<Limit>("Charlie", 35));

    minHeap.push(std::make_unique<Limit>("Alice", 30));
    minHeap.push(std::make_unique<Limit>("Bob", 25));
    minHeap.push(std::make_unique<Limit>("Charlie", 35));

    while (!maxHeap.empty()) {
        const auto& p = maxHeap.top();
        std::cout << "Name: " << p->name << ", Age: " << p->age << std::endl;
        maxHeap.pop(); 
    }

    while (!minHeap.empty()) {
        const auto& p = minHeap.top();
        std::cout << "Name: " << p->name << ", Age: " << p->age << std::endl;
        minHeap.pop();
    }

    return 0;
}