#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QFont>
#include <QLineEdit>
#include "button.h"
#include <vector>
#include <string>

#include <iostream>
#include "queue.h"
#include "defines.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <limits>
using namespace std;
class MainWindow : public QGraphicsView
{
    Q_OBJECT

public:
    MainWindow();
    QGraphicsScene *scene;
    QFont font;
    QLineEdit *StartingAddressLineEdit;

    bool started = false;
    int starting_index;
    int index = 0;

    int binary_to_decimal(string val);
    vector<Instruction> read_instructions(vector<string>&);
    void read_data();
    string bin16bit(string);
    string tobinary(int word);
    string nandfunc(string x, string y);
    int instructions_done = 0, branches_encountered = 0, total_cycles = 1, branch_mispridicted = 0, instructions_issued= 0;
    map<int, string> Memory;
    vector<Instruction> Instructions;
    vector<string> Instructions_text;
    Instruction decode_inst(string inst);

    void issue_instructions();
    bool issue_one_instruction();
    vector<table_entry> Table;
    void display_output();
    void commit_instructions();
    void commit_one(ROB_entry head);
    void check_execution();
    void check_write();

    bool stop_issue = false;
    vector<string> Registers;
    Queue ROB;
    vector<int> Register_Stat;
    vector<Reservation_Station_Entry> RS;   //[0->1] load, [2->3] store, [4->5] jmp jal ret, [6->7] beq, [8->10] add/sub/addi
                                                //[11->12] nand, [13,14] mul

    int current;
    void print_values();
    void print_table(int place_end);
    public slots:
    void startsimulation();
    void doprev();
    void donext();
    void doend();
};
#endif // MAINWINDOW_H
