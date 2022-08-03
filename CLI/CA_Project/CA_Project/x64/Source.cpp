#include <iostream>
#include "Queue.h"
#include "defines.h"
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

int binary_to_decimal(string val);
vector<Instruction> read_instructions(vector<string>&);
void read_data();
string bin16bit(string);
string tobinary(int word);
string nandfunc(string x, string y);
int instructions_done = 0, branches_encountered = 0, total_cycles = 1, branch_mispridicted = 0, instructions_issued= 0;
map<int, string> Memory;
vector<string> Registers(8, "0000000000000000");
vector<Instruction> Instructions;
vector<string> Instructions_text;
Instruction decode_inst(string inst);
Queue ROB(8);
vector<int> Register_Stat(8, -1);
void issue_instructions();
bool issue_one_instruction();
vector<table_entry> Table;
vector<Reservation_Station_Entry> RS(15);   //[0->1] load, [2->3] store, [4->5] jmp jal ret, [6->7] beq, [8->10] add/sub/addi
                                            //[11->12] nand, [13,14] mul
void display_output();
void commit_instructions();
void commit_one(ROB_entry head);
void check_execution();
void check_write();

bool stop_issue = false;
int index = 0;
int main() {
    //Memory[15] = "0000000000000110";
    read_data();

    cout << "Please enter the starting address: " << endl;
    cin >> index;
    cout << endl << endl;
    Instructions = read_instructions(Instructions_text);
    for (int i = 0; i < 15; i++)
    {
        RS[i].Qj = -1;
        RS[i].Qk = -1;
    }
    while (index < Instructions.size() || !ROB.isEmpty())
    {
        if (!ROB.isEmpty())
        {
            commit_instructions();
        }
        if (index< Instructions.size())
            issue_instructions();
        check_execution();
        check_write();
        total_cycles++;
    }
    /*
    while (!ROB.isEmpty())
    {
        commit_instructions();
        check_execution();
        check_write();
        total_cycles++;
    }*/
    
    display_output();
    
    
    system("pause");
    return 0;
}

vector<Instruction> read_instructions(vector<string> &Instructions_text)
{
    ifstream in;
    in.open("code.txt");
    vector<Instruction> Instructions;
    while (!in.eof())
    {
        Instruction current_inst;
        string inst;
        getline(in, inst, '\n');
        current_inst = decode_inst(inst);
        Instructions_text.push_back(inst);
        Instructions.push_back(current_inst);
    }
    in.close();
    return Instructions;
}

void read_data()
{
    ifstream in;
    in.open("data.txt");
    int num;
    in >> num;
    int address;
    string memory;
    for (int i = 0; i < num; i++)
    {
        in >> address >> memory;
        if (address >= 0 && address <= 65536)
            Memory[address] = memory;
        else
        {
            cout << "Invalid memory address in data file... Exiting program." << endl;
            exit(0);
        }
    }
}

string bin16bit(string binary)
{
    string word = binary; char ad;
    if (word[0] == '0')
        ad = '0';
    else ad = '1';
    if (word.size() <= 16)
    {
        while (word.size() != 16)
        {
            string temp = "";
            temp += ad;
            temp += word;
            word = temp;
        }
    }
    else
    {
        string temp = "";
        for (int i = 0; i < 16; i++)
        {
            temp += word[15 - i];
        }
        word = temp;
    }
    
    return word;
}

Instruction decode_inst(string inst)
{
    Instruction current_inst;
    int i = 0;
    string type = "";
    while (inst[i] != ' ' && i<inst.size())
    {
        type = type + inst[i];
        i++;
    }
    string temp = type;
    type = "";
    for (int k = 0; k < temp.size(); k++)
    {
        type += toupper(temp[k]);
    }
    while (inst[i] == ' ')
    {
        i++;
    }
    if (type == "LOAD")
    {

        string RD = "";
        while (inst[i] != ',')
        {
            RD = RD + inst[i];
            i++;
        }
        i++;
        temp = RD;
        RD = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RD += temp[k + 1];
        }
        int rd = stoi(RD);

        while (inst[i] == ' ')
        {
            i++;
        }
        string IMM;
        while (inst[i] != '(')
        {
            IMM = IMM + inst[i];
            i++;
        }
        int imm = stoi(IMM);
        i++;
        string RS1;
        while (inst[i] != ')')
        {
            RS1 = RS1 + inst[i];
            i++;
        }
        temp = RS1;
        RS1 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS1 += temp[k + 1];
        }
        int rs1 = stoi(RS1);
        current_inst.type = load;
        current_inst.rs1 = rs1;
        current_inst.rs2 = -1;
        current_inst.rd = rd;
        current_inst.imm = imm;
    }
    else if (type == "STORE")
    {
        string RS2 = "";
        while (inst[i] != ',')
        {
            RS2 = RS2 + inst[i];
            i++;
        }
        i++;
        temp = RS2;
        RS2 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS2 += temp[k + 1];
        }
        int rs2 = stoi(RS2);

        while (inst[i] == ' ')
        {
            i++;
        }
        string IMM;
        while (inst[i] != '(')
        {
            IMM = IMM + inst[i];
            i++;
        }
        int imm = stoi(IMM);
        i++;
        string RS1;
        while (inst[i] != ')')
        {
            RS1 = RS1 + inst[i];
            i++;
        }
        temp = RS1;
        RS1 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS1 += temp[k + 1];
        }
        int rs1 = stoi(RS1);
        current_inst.type = store;
        current_inst.rs1 = rs1;
        current_inst.rs2 = rs2;
        current_inst.rd = -1;
        current_inst.imm = imm;
    }
    else if (type == "JMP" || type == "JAL")
    {
        string IMM = "";
        while (i<inst.size() && inst[i]!=' ')
        {
            IMM = IMM + inst[i];
            i++;
        }
        int imm = stoi(IMM);
        if (type == "JMP")
            current_inst.type = jump;
        else
            current_inst.type = jal;
        current_inst.rs1 = -1;
        current_inst.rs2 = -1;
        current_inst.rd = -1;
        current_inst.imm = imm;
    }
    else if (type == "BEQ")
    {
        string RS1 = "";
        while (inst[i] != ',')
        {
            RS1 = RS1 + inst[i];
            i++;
        }
        i++;
        temp = RS1;
        RS1 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS1 += temp[k + 1];
        }
        int rs1 = stoi(RS1);
        while (inst[i] == ' ')
        {
            i++;
        }
        string RS2 = "";
        while (inst[i] != ',')
        {
            RS2 = RS2 + inst[i];
            i++;
        }
        i++;
        temp = RS2;
        RS2 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS2 += temp[k + 1];
        }
        int rs2 = stoi(RS2);
        while (i < inst.size() && inst[i] != ' ')
        {
            i++;
        }

        string IMM = "";
        while (i<inst.size())
        {
            IMM = IMM + inst[i];
            i++;
        }
        int imm = stoi(IMM);
        current_inst.type = beq;
        current_inst.rs1 = rs1;
        current_inst.rs2 = rs2;
        current_inst.rd = -1;
        current_inst.imm = imm;
    }

    else if (type == "RET")
    {
        current_inst.type = ret;
        current_inst.rs1 = -1;
        current_inst.rs2 = -1;
        current_inst.rd = -1;
        current_inst.imm = -1;
    }
    else if (type == "ADDI")
    {
        string RD = "";
        while (inst[i] != ',')
        {
            RD = RD + inst[i];
            i++;
        }
        i++;
        temp = RD;
        RD = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RD += temp[k + 1];
        }
        int rd = stoi(RD);
        while (inst[i] == ' ')
        {
            i++;
        }
        string RS1 = "";
        while (inst[i] != ',')
        {
            RS1 = RS1 + inst[i];
            i++;
        }
        i++;
        temp = RS1;
        RS1 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS1 += temp[k + 1];
        }
        int rs1 = stoi(RS1);
        while (inst[i] == ' ')
        {
            i++;
        }

        string IMM = "";
        while (i < inst.size() && inst[i] != ' ')
        {
            IMM = IMM + inst[i];
            i++;
        }
        int imm = stoi(IMM);
        current_inst.type = add_imm;
        current_inst.rs1 = rs1;
        current_inst.rs2 = -1;
        current_inst.rd = rd;
        current_inst.imm = imm;
    }
    else
    {
        string RD = "";
        while (inst[i] != ',')
        {
            RD = RD + inst[i];
            i++;
        }
        i++;
        temp = RD;
        RD = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RD += temp[k + 1];
        }
        int rd = stoi(RD);
        while (inst[i] == ' ')
        {
            i++;
        }
        string RS1 = "";
        while (inst[i] != ',')
        {
            RS1 = RS1 + inst[i];
            i++;
        }
        i++;
        temp = RS1;
        RS1 = "";
        for (int k = 0; k < temp.size() - 1; k++)
        {
            RS1 += temp[k + 1];
        }
        int rs1 = stoi(RS1);
        while (inst[i] == ' ')
        {
            i++;
        }

        string RS2 = "";
        while (i < inst.size() && inst[i] != ' ')
        {
            RS2 = RS2 + inst[i];
            i++;
        }
        RS2 = RS2[1];
        int rs2 = stoi(RS2);
        if (type == "ADD")
            current_inst.type = add;
        else if (type == "SUB")
            current_inst.type = sub;
        else if (type == "NAND")
            current_inst.type = nand;
        else
            current_inst.type = mul;
        current_inst.rs1 = rs1;
        current_inst.rs2 = rs2;
        current_inst.rd = rd;
        current_inst.imm = -1;
    }
    return current_inst;
}

void issue_instructions()
{
    if (issue_one_instruction())
    {
        if (index < Instructions.size())
            issue_one_instruction();
    }
}

bool issue_one_instruction()
{
    if (!stop_issue)
    {
        if (!ROB.isFull())
        {
            bool issue = false;
            int station = -1;
            int rob = ROB.next_rob();
            ROB_entry rob_entry;
            rob_entry.table_values.inst = Instructions_text[index];
            Inst_type type = Instructions[index].type;
            if (type == load)
            {
                if (RS[0].busy == false)
                {
                    station = 0;
                    issue = true;

                }
                else if (RS[1].busy == false)
                {
                    station = 1;
                    issue = true;
                }
                if (issue == true)
                {
                    RS[station].address = Instructions[index].imm;
                    if (Register_Stat[Instructions[index].rd] == -1)
                        Register_Stat[Instructions[index].rd] = rob;
                    rob_entry.Rd = Instructions[index].rd;
                }
            }
            else if (type == store)
            {
                if (RS[2].busy == false)
                {
                    station = 2;
                    issue = true;

                }
                else if (RS[3].busy == false)
                {
                    station = 3;
                    issue = true;
                }
                if (issue == true)
                {
                    RS[station].address = Instructions[index].imm;
                    if (Register_Stat[Instructions[index].rs2] != -1 && Register_Stat[Instructions[index].rs2] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs2];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vk = binary_to_decimal(ROB.get_entry(waiting).Value);
                            RS[station].Qk = -1;

                        }
                        else
                        {
                            RS[station].Qk = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vk = binary_to_decimal(Registers[Instructions[index].rs2]);
                        RS[station].Qk = -1;

                    }
                }
                
            }
            else if (type == beq)
            {
                if (RS[6].busy == false)
                {
                    station = 6;
                    issue = true;

                }
                else if (RS[7].busy == false)
                {
                    station = 7;
                    issue = true;
                }
                if (issue == true)
                {
                    RS[station].address = Instructions[index].imm;
                    RS[station].index = index;
                    
                    if (Register_Stat[Instructions[index].rs2] != -1 && Register_Stat[Instructions[index].rs2] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs2];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vk = binary_to_decimal(ROB.get_entry(waiting).Value); 
                            RS[station].Qk = -1;

                        }
                        else
                        {
                            RS[station].Qk = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vk = binary_to_decimal(Registers[Instructions[index].rs2]); 
                        RS[station].Qk = -1;
                    }

                }
                
            }
            else if (type == add || type == sub)
            {
                if (RS[8].busy == false)
                {
                    station = 8;
                    issue = true;

                }
                else if (RS[9].busy == false)
                {
                    station = 9;
                    issue = true;
                }
                else if (RS[10].busy == false)
                {
                    station = 10;
                    issue = true;
                }
                if (issue == true)
                {
                    if (Register_Stat[Instructions[index].rs2] != -1 && Register_Stat[Instructions[index].rs2] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs2];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vk = binary_to_decimal(ROB.get_entry(waiting).Value);
                            RS[station].Qk = -1;

                        }
                        else
                        {
                            RS[station].Qk = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vk = binary_to_decimal(Registers[Instructions[index].rs2]);
                        RS[station].Qk = -1;
                        
                    }
                    if (Register_Stat[Instructions[index].rd] == -1)
                        Register_Stat[Instructions[index].rd] = rob;
                    rob_entry.Rd = Instructions[index].rd;
                }
            }
            else if (type == add_imm)
            {
                if (RS[8].busy == false)
                {
                    station = 8;
                    issue = true;

                }
                else if (RS[9].busy == false)
                {
                    station = 9;
                    issue = true;
                }
                else if (RS[10].busy == false)
                {
                    station = 10;
                    issue = true;
                }
                if (issue == true)
                {
                    RS[station].Vk = Instructions[index].imm;
                    RS[station].Qk = -1;
                    if (Register_Stat[Instructions[index].rd] == -1)
                        Register_Stat[Instructions[index].rd] = rob;
                    rob_entry.Rd = Instructions[index].rd;
                }
                
            }
            else if (type == nand)
            {
                if (RS[11].busy == false)
                {
                    station = 11;
                    issue = true;

                }
                else if (RS[12].busy == false)
                {
                    station = 12;
                    issue = true;
                }
                if (issue == true)
                {
                    if (Register_Stat[Instructions[index].rs2] != -1 && Register_Stat[Instructions[index].rs2] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs2];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vk = binary_to_decimal(ROB.get_entry(waiting).Value);
                            RS[station].Qk = -1;

                        }
                        else
                        {
                            RS[station].Qk = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vk = binary_to_decimal(Registers[Instructions[index].rs2]);
                        RS[station].Qk = -1;

                    }
                    if (Register_Stat[Instructions[index].rd] == -1)
                        Register_Stat[Instructions[index].rd] = rob;
                    rob_entry.Rd = Instructions[index].rd;
                }
            }
            else if (type == mul)
            {
                if (RS[13].busy == false)
                {
                    station = 13;
                    issue = true;

                }
                else if (RS[14].busy == false)
                {
                    station = 14;
                    issue = true;
                }
                if (issue == true)
                {
                    if (Register_Stat[Instructions[index].rs2] != -1 && Register_Stat[Instructions[index].rs2] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs2];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vk = binary_to_decimal(ROB.get_entry(waiting).Value);
                            RS[station].Qk = -1;

                        }
                        else
                        {
                            RS[station].Qk = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vk = binary_to_decimal(Registers[Instructions[index].rs2]);
                        RS[station].Qk = -1;

                    }
                    if (Register_Stat[Instructions[index].rd] == -1)
                        Register_Stat[Instructions[index].rd] = rob;
                    rob_entry.Rd = Instructions[index].rd;
                }
            }
            else if (type == ret)
            {
                if (RS[4].busy == false)
                {
                    station = 4;
                    issue = true;

                }
                else if (RS[5].busy == false)
                {
                    station = 5;
                    issue = true;
                }
                stop_issue = true;
                if (Register_Stat[1] != -1)
                {
                    int waiting = Register_Stat[1];
                    if (ROB.get_entry(waiting).ready == true)
                    {
                        RS[station].Vj = binary_to_decimal(ROB.get_entry(waiting).Value);
                        RS[station].Qj = -1;

                    }
                    else
                    {
                        RS[station].Qj = waiting;
                    }
                }
                else
                {
                    RS[station].Vj = binary_to_decimal(Registers[1]);
                    RS[station].Qj = -1;

                }
                
            }
            else
            {
                if (RS[4].busy == false)
                {
                    station = 4;
                    issue = true;

                }
                else if (RS[5].busy == false)
                {
                    station = 5;
                    issue = true;
                }
                stop_issue = true;
                RS[station].index = index;
                if (type != ret)
                {
                    RS[station].address = Instructions[index].imm;
                }
                if (type == jal)
                {
                    if (Register_Stat[1] == -1)
                        Register_Stat[1] = rob;
                    rob_entry.Rd = 1;
                }
            }

            if (issue == true)
            {
                if (type != jump && type != jal && type != ret) 
                {
                    if (Register_Stat[Instructions[index].rs1] != -1 && Register_Stat[Instructions[index].rs1] != rob)
                    {
                        int waiting = Register_Stat[Instructions[index].rs1];
                        if (ROB.get_entry(waiting).ready == true)
                        {
                            RS[station].Vj = binary_to_decimal(ROB.get_entry(waiting).Value);
                            RS[station].Qj = -1;

                        }
                        else
                        {
                            RS[station].Qj = waiting;
                        }
                    }
                    else
                    {
                        RS[station].Vj = binary_to_decimal(Registers[Instructions[index].rs1]);
                        RS[station].Qj = -1;

                    }
                }
                if (type == beq)
                {
                    if (Instructions[index].imm < 0)
                    {
                        index = index + Instructions[index].imm;
                    }
                }
                RS[station].busy = true;
                RS[station].dest = rob;
                rob_entry.Type = type;
                rob_entry.ready = false;
                rob_entry.table_values.issued = total_cycles;
                ROB.enQueue(rob_entry);
                RS[station].stage = Stage::issue;
                RS[station].operation = type;

                index++;
            }
            
            return issue;
        }
    }
    
}

void display_output()
{
    cout.setf(ios::left);
    cout << setw(20) << "Instruction" << setw(10) << "Issued" << setw(15) << "Exec Start" << 
        setw(10) << "Exec End" << setw(10) << "Written" << setw(10)<<"Commit"<< endl;
    for (int i = 0; i < Table.size(); i++)
    {
        cout << setw(20) << Table[i].inst << setw(10) << Table[i].issued << setw(15) << 
            Table[i].started_exec << setw(10) << Table[i].finished_exec << setw(10) << Table[i].written <<
            setw(10) << Table[i].commit<<endl;

    }
    cout << endl<<endl<<endl;
    cout << "Total execution time in cycles: " << total_cycles << endl;
    if (total_cycles !=0)
        cout << "The IPC: " << static_cast<double>(instructions_done) / total_cycles<<endl;
    if (branches_encountered != 0)
        cout << "The branch misprediction percentage = " << (branch_mispridicted / branches_encountered) * 100<< '%' << endl;
    else
        cout << "The branch misprediction percentage = " << 0<<endl;
}

void commit_instructions() //comits 2 instructions
{
    if (!ROB.isEmpty())
    {
        ROB_entry head = ROB.view_front();
        if (head.ready == true)
        {
            commit_one(head);
            instructions_done++;
            if (!ROB.isEmpty())
            {
                head = ROB.view_front();
                if (head.ready == true)
                {
                    commit_one(head);
                    instructions_done++;
                }
            }
        }
    }
}

void commit_one(ROB_entry head)
{
    if (head.Type == beq)
    {
        if (head.mispredicted == true)
        {
            index = head.branch_store_dest;
            ROB.reset();
            stop_issue = false;
            for (int i = 0; i < Register_Stat.size(); i++)
            {
                Register_Stat[i] = -1;
            }
            for (int i = 0; i < RS.size(); i++)
            {
                RS[i].busy = false;
                RS[i].address = 0;
                RS[i].dest = -1;
                RS[i].index = 0;
                RS[i].Qj = -1;
                RS[i].Qk = -1;
                RS[i].stage = not_started;
                RS[i].Vj = 0;
                RS[i].Vk = 0;
            }
            branch_mispridicted++;

        }
        else
        {
            ROB.deQueue();
        }
        branches_encountered++;
    }
    else if (head.Type == jump || head.Type == jal || head.Type == ret)
    {
        index = head.branch_store_dest;
        ROB.reset();
        for (int i = 0; i < Register_Stat.size(); i++)
        {
            Register_Stat[i] = -1;
        }
        stop_issue = false;
        if (jal == head.Type)
        {
            Registers[1] = head.Value;
        }
        for (int i = 0; i < RS.size(); i++)
        {
            RS[i].busy = false;
            RS[i].address = 0;
            RS[i].dest = -1;
            RS[i].index = 0;
            RS[i].Qj = -1;
            RS[i].Qk = -1;
            RS[i].stage = not_started;
            RS[i].Vj = 0;
            RS[i].Vk = 0;
        }

    }
    else if (head.Type == store)
    {
        Memory[head.branch_store_dest] = head.Value;
        ROB.deQueue();
    }
    else
    {
        if (head.Rd != 0)
            Registers[head.Rd] = head.Value;
        ROB.deQueue();
    }
    
    if (head.Rd >= 0 && head.Rd < 8 && Register_Stat[head.Rd] == head.Number)
    {
        bool keep = false;
        for (int k = 0; k < RS.size(); k++)
        {
            if (RS[k].busy == true && ROB.get_entry(RS[k].dest).Rd == head.Rd)
                keep = true;
        }
        if (!keep)
            Register_Stat[head.Rd] = -1;
    }
        
    head.table_values.commit = total_cycles;
    Table.push_back(head.table_values);
    
}

void check_execution()
{
    for (int i = 0; i < 15; i++)
    {
        if (RS[i].stage == issue)
        {
            if (ROB.get_issue(RS[i].dest) != total_cycles) 
            {
                if (RS[i].operation == add || RS[i].operation == sub || RS[i].operation == nand
                    || RS[i].operation == mul || RS[i].operation == add_imm)
                {
                    if (RS[i].Qj == -1 && RS[i].Qk == -1)
                    {
                        RS[i].stage = execute;
                        ROB.set_exec_start(RS[i].dest, total_cycles);
                        Register_Stat[ROB.get_entry(RS[i].dest).Rd] = RS[i].dest;
                    }
                }
                else if (RS[i].operation == store)
                {

                    if (RS[i].Qj == -1 && ROB.store_at_head(RS[i].dest))
                    {
                        RS[i].stage = execute;
                        ROB.set_exec_start(RS[i].dest, total_cycles);
                        RS[i].address = RS[i].address + RS[i].Vj;
                    }
                }
                else if (RS[i].operation == load)
                {
                    if (RS[i].Qj == -1 && ROB.no_earlier_stores(RS[i].dest))
                    {
                        RS[i].stage = execute;
                        ROB.set_exec_start(RS[i].dest, total_cycles);
                        RS[i].address = RS[i].address + RS[i].Vj;
                        Register_Stat[ROB.get_entry(RS[i].dest).Rd] = RS[i].dest;
                    }
                }
                else if (RS[i].operation == beq)
                {
                    if (RS[i].Qj == -1 && RS[i].Qk == -1)
                    {
                        RS[i].stage = execute;
                        ROB.set_exec_start(RS[i].dest, total_cycles);
                    }
                }
                else if (RS[i].operation == jal || RS[i].operation == jump)
                {
                    RS[i].stage = execute;
                    ROB.set_exec_start(RS[i].dest, total_cycles);
                    if (RS[i].operation == jal)
                        Register_Stat[1] = RS[i].dest;
                }
                else if (RS[i].operation == ret)
                {
                    if (RS[i].Qj == -1)
                    {
                        RS[i].stage = execute;
                        ROB.set_exec_start(RS[i].dest, total_cycles);
                    }
                }
            }
            
        }
        else if (RS[i].stage == execute)
        {
            if (i == 0 || i == 1 || i== 2 || i ==3)
            {
                if (total_cycles - ROB.get_exec_start(RS[i].dest) == 3)
                {
                    RS[i].stage = finished_exec;

                }
            }
            else if (i == 4 || i == 5 || i == 6 || i == 7 || i == 11 || i ==12)
            {
                if (total_cycles - ROB.get_exec_start(RS[i].dest) == 1)
                {
                    RS[i].stage = finished_exec;

                }
            }
            else if (i == 8 || i == 9 || i == 10 )
            {
                if (total_cycles - ROB.get_exec_start(RS[i].dest) == 2)
                {
                    RS[i].stage = finished_exec;

                }
            }
            else if (i == 13 || i == 14)
            {
                if (total_cycles - ROB.get_exec_start(RS[i].dest) == 10)
                {
                    RS[i].stage = finished_exec;

                }
            }
            if (RS[i].stage == finished_exec)
            {
                ROB.set_exec_end(RS[i].dest, total_cycles-1);
            }
        }
        
        
    }
}

void check_write()
{
    int write2 = 0;
    for (int i = 0; i < 15; i++)
    {
        if (RS[i].stage == finished_exec)
        {
            if (write2 != 2)
            {
                int res;
                int rob = RS[i].dest;
                string result;
                Inst_type type = RS[i].operation;
                
                if (type == load)
                {
                    result = Memory[RS[i].address+RS[i].Vj];
                    res = binary_to_decimal(result);
                    ROB.Edit_value(result, rob);
                }
                else if (type == store)
                {
                    res = RS[i].Vk;
                    result = tobinary(res);
                    result = bin16bit(result);

                }
                else if (type == jump)
                {
                    res = RS[i].index + 1 + RS[i].address;
                    ROB.set_address(rob, res);
                    
                    //result = tobinary(res);
                    //result = bin16bit(result);
                }
                else if (type == beq)
                {
                    res = RS[i].index + 1;
                    if (RS[i].Vj == RS[i].Vk)
                    {
                        res += RS[i].address;
                    }
                    ROB.set_address(rob, res);
                    if (RS[i].address < 0 && RS[i].Vj != RS[i].Vk)
                        ROB.set_mispredicted(rob, true);
                    else if (RS[i].address >=0 && RS[i].Vj == RS[i].Vk)
                        ROB.set_mispredicted(rob, true);
                    else
                        ROB.set_mispredicted(rob, false);
                    
                    //result = tobinary(res);
                    //result = bin16bit(result);
                }
                else if (type == jal)
                {
                    res = RS[i].index + 1 + RS[i].address;//problem
                    ROB.set_address(rob, res);
                    result = tobinary(RS[i].index + 1);
                    result = bin16bit(result);
                    ROB.Edit_value(result, rob);
                    //result = tobinary(res);
                    //result = bin16bit(result);
                }
                else if (type == ret)
                {
                    res = RS[i].Vj;
                    ROB.set_address(rob, res);
                    //result = tobinary(res);
                    //result = bin16bit(result);
                }
                else if (type == add)
                {
                    res = RS[i].Vj + RS[i].Vk;
                    result = tobinary(res);
                    result = bin16bit(result);
                    ROB.Edit_value(result, rob);
                }
                else if (type == sub)
                {
                    res = RS[i].Vj - RS[i].Vk;
                    result = tobinary(res);
                    result = bin16bit(result);
                    ROB.Edit_value(result, rob);
                }
                else if (type == add_imm)
                {
                    res = RS[i].Vj + RS[i].Vk;
                    result = tobinary(res);
                    result = bin16bit(result);
                    ROB.Edit_value(result, rob);
                }
                else if (type == mul)
                {
                    res = RS[i].Vj * RS[i].Vk;
                    result = tobinary(res);
                    result = bin16bit(result);
                    ROB.Edit_value(result, rob);

                }
                else if (type == nand)
                {
                    string x = tobinary(RS[i].Vj), y = tobinary(RS[i].Vk);
                    x = bin16bit(x); y = bin16bit(y);
                    result = nandfunc(x, y);
                    res = binary_to_decimal(result);
                    ROB.Edit_value(result, rob);
                }

                
                if (RS[i].operation != store || (RS[i].operation == store && RS[i].Qk == -1))
                {
                    write2++;
                    if (type == store)
                    {
                        ROB.set_address(rob, RS[i].address);
                        ROB.Edit_value(result, rob);
                    }
                    
                    ROB.set_ready(rob);
                    ROB.set_written(rob, total_cycles);
                    if (ROB.get_entry(rob).Rd != 0)
                    for (int i = 0; i < 15; i++)
                    {
                        if (RS[i].Qj == rob)
                        {
                            RS[i].Vj = res;
                            RS[i].Qj = -1;
                        }
                        if (RS[i].Qk == rob)
                        {
                            RS[i].Vk = res;
                            RS[i].Qk = -1;
                        }
                        
                    }
                    RS[i].busy = false;
                    RS[i].address = 0;
                    RS[i].dest = -1;
                    RS[i].index = 0;
                    RS[i].Qj = -1;
                    RS[i].Qk = -1;
                    RS[i].stage = not_started;
                    RS[i].Vj = 0;
                    RS[i].Vk = 0;
                }
                
                
            }
        }
    }
}

int binary_to_decimal(string val)
{
    int res = 0, i = 0;
    if (val[0] == '0')
    {
        for (auto x : val)
        {
            res += pow(2, 15 - i) * (x - '0');

            i++;
        }
    }
    else
    {
        string temp = "";
        for (auto x : val)
        {
            if (x == '0')
            {
                temp.append(1, '1');

            }
            else
            {
                temp.append(1, '0');
            }
        }
        for (auto x : temp)
        {
            res += pow(2, 15 - i) * (x - '0');

            i++;
        }
        res += 1;
        res = 0 - res;

    }
    return res;
}


string tobinary(int word)
{
    string bin = "";
    vector<int> vec;
    int x = word;
    if (x == 0)
        return "0";
    if (x > 0)
    {
        while (x > 0)
        {
            vec.push_back(x % 2);
            x /= 2;
        }
        vec.push_back(0);
        reverse(vec.begin(), vec.end());

        for (auto y : vec)
        {
            bin += to_string(y);
        }
    }
    else
    {
        x = 0 - x;
        x = x - 1;

        while (x > 0)
        {
            vec.push_back(x % 2);
            x /= 2;
        }
        for (int i = 0; i < vec.size(); i++)
        {
            if (vec[i] == 0)
                vec[i] = 1;
            else
                vec[i] = 0;
        }
        vec.push_back(1);
        reverse(vec.begin(), vec.end());

        for (auto y : vec)
        {
            bin += to_string(y);
        }

    }



    return bin;
}

string nandfunc(string x, string y)
{
    string res = "";
    for (int i = 0; i < x.size(); i++)
    {
        char one;
        if (x[i] == '0' || y[i] == '0')
            one = '1';
        else
            one = '0';
        res += one;
    }
    return res;
}
