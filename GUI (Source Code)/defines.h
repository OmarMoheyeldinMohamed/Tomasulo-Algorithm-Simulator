#ifndef DEFINES_H
#define DEFINES_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;
enum Inst_type { load, store, jump, beq, jal, ret, add, sub, add_imm, nand, mul };
enum Stage {not_started, issue, execute, finished_exec, write, commit};
struct Instruction
{
    Inst_type type;
    int rd, rs1, rs2, imm;
    int num;
};
/*
struct Word
{
    string word;
};
*/
struct Reservation_Station_Entry
{
    bool busy = false;
    Inst_type operation;
    int Vj, Vk;
    int Qj, Qk;
    int dest;
    int address;
    Stage stage;
    int index;
};
struct table_entry
{
    string inst;
    int issued, started_exec, finished_exec, written, commit;
};

struct ROB_entry
{
    int Number, Rd;		//Number = number in ROB
    string Value;
    bool ready = false, mispredicted = false;
    Inst_type Type;
    int branch_store_dest;
    table_entry table_values;
};



#endif
