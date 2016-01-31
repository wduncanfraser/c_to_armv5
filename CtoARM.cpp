//
// Created by W. Duncan Fraser on 4/28/15.
//

#include "CtoARM.h"
#include "CtoARMtokens.h"

std::string data = "\t.data\n";		            //contains the data section of the ARM assembly
std::string text = "\t.text\n\tb main\nmain:\n";     //contains the text section of the ARM assembly
Registers SysRegs;
SymTab SymbolTable;
int IfCount, WhileCount = 0;

void writeFile(std::string filename)
{
    std::ofstream file;
    file.open(filename);
    file << data << "\n" << text << "\n";
    file.close();
}


std::string Registers::GetReg(char t)
{
    std::string retregister;
    switch(t)
    {
        case 'r':
            for(int i = 0; i < rRegCount; i++)
            {
                if(!rRegisters[i])
                {
                    rRegisters[i] = 1;
                    retregister = "r"+std::to_string(i);
                    break;
                }
            }
            break;
        case 's':
            for(int i = 0; i < sRegCount; i++)
            {
                if(!sRegisters[i])
                {
                    sRegisters[i] = 1;
                    retregister = "s"+std::to_string(i);
                    break;
                }
            }
            break;
    }
    return retregister;
}

void Registers::RelReg(std::string reg)
{
    char t = reg[0];
    int i = std::stoi(reg.substr(1));
    switch(t)
    {
        case 'r':
            rRegisters[i] = 0;
            break;
        case 's':
            sRegisters[i] = 0;
            break;
    }
}

void Registers::RelAll()
{
    rRegisters.fill(0);
    sRegisters.fill(0);
}

std::string loadIntReg(int i)
{
    std::string reg = SysRegs.GetReg('r');
    text += "\tmov " + reg + ", #" + std::to_string(i) + "\n";
    return reg;
}

bool SymTab::AddSymbol(std::string name, int type)
{
    if(table.count(name) == 0)
    {
        table.insert(std::pair<std::string, int>(name, type));
        return 0;
    }
    else
    {
        return 1;
        //Multiple declarations, throw error
    }
}

std::string NStatementBlock::CodeGen()
{
    for (unsigned int i = 0; i < stmtlist.size(); i++)
    {
        stmtlist[i]->CodeGen();
    }

    return "";
}

std::string NCondition::CodeGen()
{
    std::string rlhs = lhs.CodeGen();
    std::string rrhs = rhs.CodeGen();

    switch(oper)
    {
        case CMPEQ:
            text += "\tteq " + rlhs + ", " + rrhs + "\n";
            break;
    }

    SysRegs.RelReg(rlhs);
    SysRegs.RelReg(rrhs);
    //Nothing to return, comparison in ARM5 sets the zero flag, not a register
    return "";
}

std::string NFlowControl::CodeGen()
{
    switch(type)
    {
        case IF:
            cond.CodeGen();
            text += "\tbeq if_" + std::to_string(IfCount) + "\n";
            text += "\tb if_" + std::to_string(IfCount) + "_end\n";
            text += "\tif_" + std::to_string(IfCount) + ":\n";
            stmts.CodeGen();
            text += "\tif_" + std::to_string(IfCount) + "_end:\n";
            //Increment If Count
            IfCount++;
            break;
        case WHILE:
            text += "\twhile_" + std::to_string(WhileCount) + "_check:\n";
            cond.CodeGen();
            text += "\tbeq while_" + std::to_string(WhileCount) + "\n";
            text += "\tb while_" + std::to_string(WhileCount) + "_end\n";
            text += "\twhile_" + std::to_string(WhileCount) + ":\n";
            stmts.CodeGen();
            text += "\tb while_" + std::to_string(WhileCount) + "_check\n";
            text += "\twhile_" + std::to_string(WhileCount) + "_end:\n";
            //Increment While Count
            WhileCount++;
            break;
    }

    return "";
}

std::string NBinaryOperator::CodeGen()
{
    std::string reg = SysRegs.GetReg('r');

    std::string rlhs = lhs.CodeGen();
    std::string rrhs = rhs.CodeGen();
    //This is literally how limited the ARMv5 is for hardware operations.
    //Division isn't implemented, going to attempt to implement via substraction loops
    switch(oper)
    {
        case PLUS:
            text += "\tadd " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case MINUS:
            text += "\tsub " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case MUL:
            text += "\tmul " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case DIV:
            text += "\tdiv " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case AND:
            text += "\tand " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case OR:
            text += "\torr " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
        case XOR:
            text += "\teor " + reg + ", " + rlhs + ", " + rrhs + "\n";
            break;
    }

    SysRegs.RelReg(rlhs);
    SysRegs.RelReg(rrhs);
    return reg;
}

std::string NIncDecOperator::CodeGen()
{
    std::string rrhs = rhs.CodeGen();

    switch(oper)
    {
        case INC:
            text += "\tadd " + rrhs + ", " + rrhs + ", #1\n";
            break;
        case MINUS:
            text += "\tsub " + rrhs + ", " + rrhs + ", #1\n";
            break;
    }
    return rrhs;
}

std::string NInteger::CodeGen()
{
    std::string reg = loadIntReg(val);
    return reg;
}

std::string NId::CodeGen()
{
    std::string reg = SysRegs.GetReg('r');
    std::string regadd = SysRegs.GetReg('r');

    text += "\tldr " + regadd + ", " + "=" + name + "\n";
    text += "\tldr " + reg + ", " + "[" + regadd + "]\n";

    SysRegs.RelReg(regadd);
    return reg;
}

NVarDec::NVarDec(std::string name, int type) : name(name), type(type)
{
    SymbolTable.AddSymbol(name, type);
}

std::string NVarDec::CodeGen()
{
    switch(type)
    {
        case TINT:
            data += "\t" + name + ":\t.word 0\n";
            break;
    }
    //No return
    return "";
}

std::string NVarAss::CodeGen()
{
    std::string reg = SysRegs.GetReg('r');
    std::string regadd = SysRegs.GetReg('r');
    std::string rrhs = rhs.CodeGen();

    text += "\tmov " + reg + ", " + rrhs + "\n";
    text += "\tldr " + regadd + ", " + "=" + name + "\n";
    text += "\tstr " + reg + ", " + "[" + regadd + "]\n";

    SysRegs.RelReg(reg);
    SysRegs.RelReg(regadd);
    SysRegs.RelReg(rrhs);
    return regadd;
}

std::string NReturn::CodeGen()
{
    //Evaluate return expression
    std::string rrhs = rhs.CodeGen();
    //Release all registers, as return is end of function (anything cross function should be pushed to the stack or .data.
    SysRegs.RelAll();

    text += "\tmov r0, " + rrhs + "\n";

    return "r0";
}
