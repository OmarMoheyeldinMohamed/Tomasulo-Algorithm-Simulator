#ifndef QUEUE_H
#define QUEUE_H
//Code for circular queue adapted from Programiz
// Circular Queue implementation in C++

#include <iostream>
#include "defines.h"
#include <vector>
using namespace std;




class Queue {
private:
    int  front, rear, SIZE;
    vector<ROB_entry> *items;

public:
    Queue(int size);
    // Check if the queue is full
    bool isFull();
    // Check if the queue is empty
    bool isEmpty();
    // Adding an element
    int enQueue(ROB_entry element);
    // Removing an element
    ROB_entry deQueue();

    //void display();
    ROB_entry view_front();
    void Edit_value(string value, int index);
    void reset();
    int next_rob();
    ROB_entry get_entry(int i);
    void set_exec_start(int x, int y);
    int get_exec_start(int x);
    bool store_at_head(int);
    bool no_earlier_stores(int);
    void set_exec_end(int x, int y);
    void set_issued(int x, int y);
    int get_issue(int x);
    void set_ready(int i);
    void set_written(int x, int y);
    void set_address(int x, int y);
    void set_mispredicted(int x, bool y);


};


#endif
