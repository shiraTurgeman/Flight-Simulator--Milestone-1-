
//
// Created by shira on 19.12.2019.
//

#ifndef EX3PROJECT_INTERPRETER_H
#define EX3PROJECT_INTERPRETER_H


#include "Command.h"

#include <thread>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <unistd.h>
#include <unordered_set>
#include <arpa/inet.h>
#include <queue>
using namespace std;

extern queue <pair<string, float >> simQueue;

extern int data_received;
extern bool done;
//queue <string> queueForSimulator;

/**
 * Class Interpreter
 */
class Interpreter {
public:
    //constructor
    Interpreter();
    //This function deletes unnecessary spaces from words
    string trim(const string& str);
    //This function divides the text into separate words and put them in array
    vector<string> splitLine(string line, vector<string> splittedStrings );
    //This function reads a text from a file and inserts it into an array
    vector<string> lexer(char* fileName);
    //This function builds a map of strings and commands
    map <string, Command*> intoCommandMap(vector<string> splittedStrings,map <string, Command*> commandMap );
    //This function goes on array and executes the commands
    void parser (vector<string> splittedStrings);
    //constructor
    ~Interpreter();
};


/**
 * Class Variable
 */
class Variable {
private:
    int update;
    float value;
    string sim;
    string direction;

public:
    //constructor
    Variable(float value, string sim, string direction, int update);
    void updateValue(float num);
    void updateStat(int num);
    string getDir();
    string getSim();
    float getVar();
    int getUpdate();

    //constructor
    ~Variable();
};


/**
 * Class SymbolTable
 */
class SymbolTable{
private:
    static SymbolTable* instance;
    //constructor - It is in the private area so objects cannot be created from outside.
    SymbolTable();
public:

    //map to mane of variable and the Variable object
    map<string, Variable> symbolTable;
    map<string, float > simMap;
    //map to commands
    map <string, Command*> commandMap;

    //This method allows you to get the pointer to the only object created.
    static SymbolTable *getInstance();
    map<string, Command *>  intilizationCommandMap(vector<string> splittedStrings);
    void upDateSymbolTable(string,Variable);
    void updateValImSymbolTable(string key, float val);
    Variable getVariable(string name);
    string SetArrayOfSim(int i);
    void putInSimMap (int i, float num);
    float getValueFromSim(string sim);
    //destructor
    ~SymbolTable();
};


#endif //EX3PROJECT_INTERPRETER_H
