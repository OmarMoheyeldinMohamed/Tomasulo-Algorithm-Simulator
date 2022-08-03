#include "Queue.h"

Queue::Queue (int size)
{
    front = -1;
    rear = -1;
    SIZE = size;

    items = new vector<ROB_entry>(SIZE);
}

bool Queue::isFull()
{
    if (front == 0 && rear == SIZE - 1) {
        return true;
    }
    if (front == rear + 1) {
        return true;
    }
    return false;
}

bool Queue::isEmpty()
{
    if (front == -1)
        return true;
    else
        return false;
}

int Queue::enQueue(ROB_entry element) {
    if (isFull()) {
        cout << "Queue is full";
    }
    else {
        if (front == -1) front = 0;
        rear = (rear + 1) % SIZE;
        items->at(rear).branch_store_dest = element.branch_store_dest;
        items->at(rear).mispredicted = element.mispredicted;
        items->at(rear).Number = rear;
        items->at(rear).Rd = element.Rd;
        items->at(rear).ready = element.ready;
        items->at(rear).table_values = element.table_values;
        items->at(rear).Type = element.Type;
        items->at(rear).Value = element.Value;
        return rear;
    }
}

ROB_entry Queue::deQueue() {
    ROB_entry element;
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return (ROB_entry());
    }
    else {
        element = items->at(front);
        items->at(front).table_values.issued = 0;
        if (front == rear) {
            front = -1;
            rear = -1;
        }
        // Q has only one element,
        // so we reset the queue after deleting it.
        else {
            front = (front + 1) % SIZE;
        }
        return (element);
    }
}
/*
void Queue::display() {
    // Function to display status of Circular Queue
    int i;
    if (isEmpty()) {
        cout << endl
            << "Empty Queue" << endl;
    }
    else {
        cout << "Front -> " << front;
        cout << endl
            << "Items -> ";
        for (i = front; i != rear; i = (i + 1) % SIZE)
            cout << items[i];
        cout << items[i];
        cout << endl
            << "Rear -> " << rear;
    }
}*/

ROB_entry Queue::view_front()
{
    if (!isEmpty())
        return items->at(front);
    else
        return ROB_entry();
}

void Queue::Edit_value(string value, int index)
{
    items->at(index).Value = value;
}

void Queue::reset()
{
    delete items;
    front = -1;
    rear = -1;
    items = new vector<ROB_entry>(SIZE);
}

int Queue::next_rob()
{
    return (rear + 1) % SIZE;;
}

ROB_entry Queue::get_entry(int i)
{
    return items->at(i);
}

void Queue::set_exec_start(int x, int y)
{
    items->at(x).table_values.started_exec = y;
}

int Queue::get_exec_start(int x)
{
    return items->at(x).table_values.started_exec;
}

bool Queue::store_at_head(int i)
{
    if (i == front)
        return true;
    else
        return false;
}

bool Queue::no_earlier_stores(int i)
{
    if (i == front)
        return true;
    int x = i;
    while (x != front)
    {
        x = x - 1;
        if (x == -1)
            x = SIZE - 1;
        if (items->at(x).Type == store)
            return false;

    }
    if (items->at(x).Type == store)
        return false;
    return true;

}

void Queue::set_exec_end(int x, int y)
{
    items->at(x).table_values.finished_exec = y;
}

void Queue::set_issued(int x, int y)
{
    items->at(x).table_values.issued = y;
}

int Queue::get_issue(int x)
{
    return items->at(x).table_values.issued;
}

void Queue::set_ready(int i)
{
    items->at(i).ready = true;
}

void Queue::set_written(int x, int y)
{
    items->at(x).table_values.written = y;
}

void Queue::set_address(int x, int y)
{
    items->at(x).branch_store_dest = y;
}

void Queue::set_mispredicted(int x, bool y)
{
    items->at(x).mispredicted = y;
}
