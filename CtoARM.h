//
// Header Containg Register class, Symbol Table, and Parse tree nodes
// Created by W. Duncan Fraser on 4/28/15.
//

#ifndef CTOARMCOMPILER_CTOARM_H
#define CTOARMCOMPILER_CTOARM_H

#include <string>
#include <map>
#include <array>
#include <vector>
#include <fstream>
#include <iostream>

#define rRegCount 10
#define sRegCount 32

extern std::string data;	//contains the data section of the MIPS assembly
extern std::string text;	//contains the text section of the MIPS assembly

//Register tracker
class Registers
{
private:
    std::array<int, rRegCount> rRegisters;
    std::array<int, sRegCount> sRegisters;
public:
    //Constructor
    Registers() { rRegisters.fill(0); sRegisters.fill(0); }
    //Get an available register based on type (s or t)
    std::string GetReg(char t);
    //Release a register by full name
    void RelReg(std::string reg);
    //Release all registers
    void RelAll();
};

//Symbol Table
class SymTab{
private:
    std::map<std::string, int> table;
public:
    bool AddSymbol(std::string name, int type);
};

//Base Node
class Node
{
public:
    virtual ~Node() { };
    virtual std::string CodeGen() { std::string temp; return temp; };
};

//Statement Node
class NStatement : public Node
{ };


//Expressions Node
class NExpression : public NStatement
{ };

//Statement Block
class NStatementBlock : public NStatement
{
public:
    std::vector<NStatement*> stmtlist;
    //Constructor
    NStatementBlock() { };
    std::string CodeGen();
};

//Condition Node. End result leaves condition flag used by branches
class NCondition : public NStatement
{
public:
    int oper;
    NExpression & lhs;
    NExpression & rhs;
    //Constructor
    NCondition(NExpression & lhs, int oper, NExpression & rhs) : lhs(lhs), rhs(rhs), oper(oper) { };
    std::string CodeGen();
};

//Flow Control Node
class NFlowControl : public NStatement
{
public:
    int type;
    NCondition & cond;
    NStatement & stmts;
    //Constructor
    NFlowControl(int type, NCondition & cond, NStatement & stmts) : type(type), cond(cond), stmts(stmts) { };
    std::string CodeGen();
};

//Binary Operator Node
class NBinaryOperator : public NExpression
{
public:
    int oper;
    NExpression & lhs;
    NExpression & rhs;
    //Constructor
    NBinaryOperator(NExpression & lhs, int oper, NExpression & rhs) : lhs(lhs), rhs(rhs), oper(oper) { };
    std::string CodeGen();
};

//Incriment and Decrament Node
class NIncDecOperator : public NExpression
{
public:
    int oper;
    NExpression & rhs;
    //Constructor
    NIncDecOperator(int oper, NExpression & rhs) : oper(oper), rhs(rhs) { };
    std::string CodeGen();
};

//Integer Node
class NInteger : public NExpression
{
public:
    int val;
    //Constructor
    NInteger(int val) : val(val) { };
    std::string CodeGen();
};

//Variable Node
class NId : public NExpression
{
public:
    std::string name;
    //Constructor
    NId(std::string name) : name(name) { };
    std::string CodeGen();
};

//Variable Declaration Node
class NVarDec : public NStatement
{
public:
    std::string name;
    int type;
    //Constructor
    NVarDec(std::string name, int type);
    std::string CodeGen();
};

//Variable Assignment Node
class NVarAss : public NStatement
{
public:
    std::string name;
    NExpression & rhs;
    //Concstructor
    NVarAss(std::string name, NExpression & rhs) : name(name), rhs(rhs) { };
    std::string CodeGen();
};

//Return Node. Return puts expression into r0 as that is typically the parameter passing/return register
class NReturn : public NStatement
{
public:
    NExpression & rhs;
    //Constructor
    NReturn(NExpression & rhs) : rhs(rhs) { };
    std::string CodeGen();
};

//Function for writing code to file
void writeFile(std::string filename);

//Function to load int into register. Returns register loaded into.
std::string loadIntReg(int i);

#endif //CTOARMCOMPILER_CTOARM_H
