#include "mainwindow.h"
#include <string>
using namespace std;




MainWindow::MainWindow():ROB(8)
{
    QPixmap pixmap("cpu.jpg");
    QIcon icon(pixmap);
    setWindowIcon(icon);

    index = 0;
    Registers = vector<string> (8, "0000000000000000");
    ROB = Queue(8);
     Register_Stat = vector<int>(8, -1);
     RS = vector<Reservation_Station_Entry>(15);
    current = 1;

    scene = new QGraphicsScene;

    setBackgroundBrush(QBrush(Qt::black));
    setScene(scene);
    setMinimumSize(1500, 900);
    setFixedWidth(1500);
    scene->setSceneRect(0, 0, 1500, 10000);
    setWindowTitle("Project");
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsTextItem *Title= new QGraphicsTextItem(QString("Project"));
    QFont titlefont("comic sans", 50);
    Title->setFont(titlefont);
    Title->setPos(width()/2-Title->boundingRect().width()/2, 25);
    Title->setDefaultTextColor(Qt::yellow);
    scene->addItem(Title);

    QGraphicsTextItem *StartingAddressText = new QGraphicsTextItem(QString("Starting Address:"));
    font = QFont("comic sans", 17);
    StartingAddressText->setFont(font);
    StartingAddressText->setPos(200, 150);
    StartingAddressText->setDefaultTextColor(Qt::white);
    scene->addItem(StartingAddressText);

    StartingAddressLineEdit = new QLineEdit;
    scene->addWidget(StartingAddressLineEdit);
    StartingAddressLineEdit->resize(350,45);
    StartingAddressLineEdit->move(200,200);
    StartingAddressLineEdit->setFont(font);

    Button *Start = new Button("Start");
    Start->setPos(500, 700);
    connect(Start, SIGNAL(clicked()), this, SLOT(startsimulation()));
    scene->addItem(Start);
}

void MainWindow::startsimulation()
{
    starting_index = StartingAddressLineEdit->text().toInt();







    read_data();
    Instructions = read_instructions(Instructions_text);



    print_values();
}

void MainWindow::doprev()
{
    if (current !=1)
        current--;
    print_values();

}

void MainWindow::donext()
{
    current++;
    print_values();
}

void MainWindow::doend()
{
    current = INT_MAX;
    print_values();
}





vector<Instruction> MainWindow::read_instructions(vector<string> &Instructions_text)
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

void MainWindow::read_data()
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

string MainWindow::bin16bit(string binary)
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

Instruction MainWindow::decode_inst(string inst)
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
        QString RD1 = QString::fromStdString(RD);
        int rd = RD1.toInt();

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
        QString IMM1 = QString::fromStdString(IMM);
        int imm = IMM1.toInt();
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
        QString RS11 = QString::fromStdString(RS1);
        int rs1 = RS11.toInt();
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
        QString RS22 = QString::fromStdString(RS2);
        int rs2 = RS22.toInt();

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
        QString IMM1 = QString::fromStdString(IMM);
        int imm = IMM1.toInt();
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
        QString RS11 = QString::fromStdString(RS1);
        int rs1 = RS11.toInt();
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
        QString IMM1 = QString::fromStdString(IMM);
        int imm = IMM1.toInt();
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
        QString RS11 = QString::fromStdString(RS1);
        int rs1 = RS11.toInt();
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
        QString RS22 = QString::fromStdString(RS2);
        int rs2 = RS22.toInt();
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
        QString IMM1 = QString::fromStdString(IMM);
                int imm = IMM1.toInt();
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
        QString RD1 = QString::fromStdString(RD);
                int rd = RD1.toInt();
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
        QString RS11 = QString::fromStdString(RS1);
                int rs1 = RS11.toInt();
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
        QString IMM1 = QString::fromStdString(IMM);
                int imm = IMM1.toInt();
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
        QString RD1 = QString::fromStdString(RD);
                int rd = RD1.toInt();
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
        QString RS11 = QString::fromStdString(RS1);
                int rs1 = RS11.toInt();
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
        QString RS22 = QString::fromStdString(RS2);
                int rs2 = RS22.toInt();
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

void MainWindow::issue_instructions()
{
    if (issue_one_instruction())
    {
        if (index < Instructions.size())
            issue_one_instruction();
    }
}

bool MainWindow::issue_one_instruction()
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
                if (type == jal)
                {
                    Register_Stat[1] = rob;
                }
                if (type == load || type == add || type == sub || type == add_imm || type == nand || type == mul)
                {
                    Register_Stat[Instructions[index].rd] = rob;
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

void MainWindow::display_output()
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

void MainWindow::commit_instructions() //comits 2 instructions
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

void MainWindow::commit_one(ROB_entry head)
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

    if (head.Rd >=0 && head.Rd < 8 && Register_Stat[head.Rd] == head.Number)
        Register_Stat[head.Rd] = -1;
    head.table_values.commit = total_cycles;
    Table.push_back(head.table_values);

}

void MainWindow::check_execution()
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

void MainWindow::check_write()
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

void MainWindow::print_values()
{
    for (int i = 0; i<scene->items().size(); i++)
            delete scene->items()[i];
        scene->clear();


        Button *Prev = new Button("Prev", 100, 50);
        Prev->setPos(50, 50);
        connect(Prev, SIGNAL(clicked()), this, SLOT(doprev()));
        scene->addItem(Prev);

        Button *Next = new Button("Next", 100, 50);
        Next->setPos(160, 50);
        connect(Next, SIGNAL(clicked()), this, SLOT(donext()));
        scene->addItem(Next);

        Button *End = new Button("End", 100, 50);
        End->setPos(270, 50);
        connect(End, SIGNAL(clicked()), this, SLOT(doend()));
        scene->addItem(End);

    QGraphicsTextItem *CurrentValues = new QGraphicsTextItem(QString("Current Cycle:" + QString::number(current) +
                            ", Instructions done: " + QString::number(instructions_done) + ", Branches encountered: " +
                            QString::number(branches_encountered) + ", Branches mispredicted: " + QString::number(branch_mispridicted)));
    CurrentValues->setFont(font);
    CurrentValues->setDefaultTextColor(Qt::white);
    CurrentValues->setPos(50, 150);
    scene->addItem(CurrentValues);

    Registers = vector<string> (8, "0000000000000000");
    ROB = Queue(8);
     Register_Stat = vector<int>(8, -1);
     RS = vector<Reservation_Station_Entry>(15);

    for (int i = 0; i < 15; i++)
        {
            RS[i].Qj = -1;
            RS[i].Qk = -1;
        }

        index = starting_index;
        stop_issue = false;
    instructions_done = 0, branches_encountered = 0, total_cycles = 1, branch_mispridicted = 0, instructions_issued= 0;
    Table = vector<table_entry>();
        while ((index < Instructions.size() || !ROB.isEmpty()) && total_cycles <= current)
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
        total_cycles--;

        int place_end;

        QGraphicsTextItem *ROB_Title = new QGraphicsTextItem(QString("ROB:"));
        ROB_Title->setFont(font);
        ROB_Title->setDefaultTextColor(Qt::red);
        ROB_Title->setPos(50, 100);
        scene->addItem(ROB_Title);
    for (int i = 0; i< 8; i++)
    {

        ROB_entry entry = ROB.get_entry(i);
        QString type_text = "";
        Inst_type type = entry.Type;
        if (type == load)
            type_text = "Load";
        else if (type == store)
            type_text = "Store";
        else if (type == jump)
            type_text = "Jump";
        else if (type == beq)
            type_text = "Beq";
        else if (type == jal)
            type_text = "Jal";
        else if (type == ret)
            type_text = "Ret";
        else if (type == add)
            type_text = "Add";
        else if (type == sub)
            type_text = "Sub";
        else if (type == add_imm)
            type_text = "Addi";
        else if (type == nand)
            type_text = "Nand";
        else if (type == mul)
            type_text = "Mul";
        QString ready_text = "";
        if (entry.ready == true)
            ready_text = "Y";
        else
            ready_text = "N";



        if (i == 7)
            place_end = 250 +i*75+150;
        QColor color;
        if (entry.table_values.issued != 0)
            color = Qt::green;
        else
            color = Qt::white;

        QGraphicsTextItem *ROB_Text = new QGraphicsTextItem(QString("#" + QString::number(i )+ "  Type: " + type_text
                + "  Dest : " + QString::number(entry.Rd) + "  Value: " + QString::fromStdString(entry.Value) + "  Ready: " +  ready_text));
        ROB_Text->setFont(font);
        ROB_Text->setDefaultTextColor(color);
        ROB_Text->setPos(50, 250+i*75);

        scene->addItem(ROB_Text);


    }
    QGraphicsTextItem *RS_Title = new QGraphicsTextItem(QString("Reservation Stations:"));
    RS_Title->setFont(font);
    RS_Title->setDefaultTextColor(Qt::red);
    RS_Title->setPos(50, place_end);
    scene->addItem(RS_Title);
    place_end += 100;
    for (int i = 0; i< 15; i++)
    {
        QString type_text = "";
        Inst_type type = RS[i].operation;
        if (type == load)
            type_text = "Load";
        else if (type == store)
            type_text = "Store";
        else if (type == jump)
            type_text = "Jump";
        else if (type == beq)
            type_text = "Beq";
        else if (type == jal)
            type_text = "Jal";
        else if (type == ret)
            type_text = "Ret";
        else if (type == add)
            type_text = "Add";
        else if (type == sub)
            type_text = "Sub";
        else if (type == add_imm)
            type_text = "Addi";
        else if (type == nand)
            type_text = "Nand";
        else if (type == mul)
            type_text = "Mul";
        QString busy_text = "";
        QColor color;
        if (RS[i].busy == true)
        {
            busy_text = "Y";
            color = Qt::green;
        }
        else
        {
            busy_text = "N";
            color = Qt::white;
        }

        QGraphicsTextItem *RS_Text = new QGraphicsTextItem(QString( type_text + "  Busy: " + busy_text + "  Vj:"
                + QString::number(RS[i].Vj) + "  Vk: "+  QString::number(RS[i].Vk) + "  Qj: " + QString::number(RS[i].Qj)
                +"  Qk: " + QString::number(RS[i].Qk) +"  ROB dest: " +  QString::number(RS[i].dest) +
                                                                    "  Address: " + QString::number(RS[i].address)));
        RS_Text->setFont(font);
        RS_Text->setDefaultTextColor(color);
        RS_Text->setPos(50, place_end);
        scene->addItem(RS_Text);
        place_end += 75;
    }
    if ((index < Instructions.size() || !ROB.isEmpty()) == false)
    {
        print_table(place_end);
    }


}

void MainWindow::print_table(int place_end)
{
    QGraphicsTextItem *Table_Title = new QGraphicsTextItem(QString("Final Values:"));
    Table_Title->setFont(font);
    Table_Title->setDefaultTextColor(Qt::red);
    Table_Title->setPos(50, place_end);
    scene->addItem(Table_Title);
    place_end += 100;
    for (int i = 0; i < Table.size(); i++)
        {
            QGraphicsTextItem *Table_Text = new QGraphicsTextItem("Instruction: "+ QString::fromStdString(Table[i].inst) +
        "  Issued: " + QString::number(Table[i].issued) + "  Exec Start: " + QString::number(Table[i].started_exec) +
        "  Exec End: " + QString::number(Table[i].finished_exec) + "  Written: "+ QString::number(Table[i].written) + "  Commit: "
            + QString::number(Table[i].commit));
            Table_Text->setDefaultTextColor(Qt::white);
            Table_Text->setPos(50, place_end);
            Table_Text->setFont(font);
            scene->addItem(Table_Text);
            place_end += 75;

        }
    QString branch_text;
    if (branches_encountered != 0)
        branch_text = "The branch misprediction percentage = " +QString::number(static_cast<double>(branch_mispridicted) / branches_encountered * 100) + "%";
    else
        branch_text = "The branch misprediction percentage = " + QString::number(0);

        QGraphicsTextItem *Table_Text = new QGraphicsTextItem("Total execution time in cycles: " + QString::number(total_cycles) + "\n"+"The IPC: " +
        QString::number( static_cast<double>(instructions_done) / total_cycles) + "\n"+branch_text);
        Table_Text->setDefaultTextColor(Qt::white);
        Table_Text->setPos(50, place_end);
        scene->addItem(Table_Text);
        Table_Text->setFont(font);
        place_end += 75;

}

int MainWindow::binary_to_decimal(string val)
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


string MainWindow::tobinary(int word)
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

string MainWindow::nandfunc(string x, string y)
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



