#include <iostream>

// Node class to represent a node of the linked list.
class Node {
public:
    int data;
    Node* next;
  
    // Default constructor
    Node()
    {
        data = 0;
        next = nullptr;
    }
  
    // Parameterised Constructor
    Node(int data)
    {
        this->data = data;
        this->next = nullptr;
    }
};



// Linked list class to implement a linked list.
class Linkedlist {
    Node* head;
  
public:
    // Default constructor
    Linkedlist() { head = nullptr; }
  
    // Function to insert a node at the end of the linked list.
    void insertNode(int);
  
    // Function to print the linked list.
    void printList();
  
    // Function to delete the node at given position
    void deleteNode(int);
};