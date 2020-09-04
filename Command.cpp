#include "Command.h"
#include "Interpreter.h"
#include "ex1.h"
#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <sstream>

using namespace std;
//the global mutex for the programm.
mutex mutexForChangeMaps;

//this method updates the symbol map by index.
void updateValueInSymbolMap(vector<string> arrayStr, int i) {
    //create instance for the maps, and get he sim and value.
    SymbolTable *instance = SymbolTable::getInstance();
    string sim = instance->getVariable(arrayStr[i]).getSim();
    int newValue = instance->getValueFromSim(sim);
    string dir = instance->symbolTable.find(arrayStr[i])->second.getDir();
    //update the symbole map.
    instance->updateValImSymbolTable(arrayStr[i], newValue);
}

//function of open Command
int openDataServer(int port) {
    //creating new socket
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer == -1) {
        //cant create socket
        std::cerr << "cant create socket" << std::endl;
    }
    //create the object for the bind
    struct sockaddr_in addressSer;
    addressSer.sin_family = AF_INET;
    addressSer.sin_addr.s_addr = INADDR_ANY;
    addressSer.sin_port = htons(port);
    //bind the socket to ip
    int binding = bind(socketServer, (struct sockaddr *) &addressSer, sizeof(addressSer));
    if (binding == -1) {
        std::cerr << "cant bind socket to ip" << std::endl;
    }
    //listen to the port
    int listenning = listen(socketServer, 5);
    if (listenning == -1) {
        std::cerr << "cant listen to ip" << std::endl;
    }
    //accept the client- flight gear
    int acc = accept(socketServer, (struct sockaddr *) &addressSer, (socklen_t *) &addressSer);

    if (acc == -1) {
        std::cerr << "cant accept client" << std::endl;
    }
    return acc;
}

void readFromServer(int acc) {
    //reading from the socket line by line-use loop
    //save each line to the data structure
    char bufferRead[1024] = {0};
    int value;
    SymbolTable *instance = SymbolTable::getInstance();
    size_t i = 0;
    string token;
    string s;
    float num;
    int index = 0;
    //this loop receives data from the simulator and updates the map.
    while (true) {
        //read from the server
        value = read(acc, bufferRead, 1024);
        if (value == -1) {
            std::cerr << "error with read" << std::endl;
        }
        while (i < sizeof(bufferRead)) {
            s = s + bufferRead[i];
            if ((bufferRead[i] == ',')) {
                mutexForChangeMaps.lock();
                token = s.substr(0, s.find(','));
                num = stof(token);
                //insert the number and the sim
                instance->putInSimMap(index, num);
                s = s.erase(0, s.find(',') + 1);
                index++;
                mutexForChangeMaps.unlock();
            }
            if ((bufferRead[i] == '\n')) {
                mutexForChangeMaps.lock();
                token = s.substr(0, s.find('\n'));
                num = stof(token);
                //insert the number and the sim
                instance->putInSimMap(index, num);
                s = s.erase(0, s.find('\n') + 1);
                index = 0;
                mutexForChangeMaps.unlock();
                break;
            }
            i++;
        }
        i = 0;
    }
}

/**
 * Class for OpenServerCommand
 */
//constructor
OpenServerCommand::OpenServerCommand() {};

int OpenServerCommand::execute(vector<string> arrayStr, int i) {
    try {
        port = stoi(arrayStr[i + 1]);
    } catch (const std::exception &e) {
        cout << "problem with port in server";
    }
    //open thread to to connect
    int acc = openDataServer(port);
    thread threadServer(readFromServer, acc);
    threadServer.detach();
    return 2;
};

//destructor
OpenServerCommand::~OpenServerCommand() {
    close(socketServer);
};

/**
 * Class for ConnectCommand
 */
ConnectCommand::ConnectCommand() {};

//method of connect Command
void ConnectCommand::connectControlClient(string adress, int port) {
    //create new socket
    if (socketClient == -1) {
        std::cerr << "cant create socket" << std::endl;
    }
    //create the object for the bind
    sockaddr_in addressClient;
    addressClient.sin_family = AF_INET;
    addressClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addressClient.sin_port = htons(port);
    int connectUs = connect(socketClient, (struct sockaddr *) &addressClient, sizeof(addressClient));
    //check if connected
    if (connectUs == -1) {
        std::cerr << "cant create socket" << std::endl << std::flush;
    } else {
        std::cerr << "connected to server" << std::endl << std::flush;
    }

    //this loop sends data to the simulator.
    while (true) {
        while (!simQueue.empty()) {
            mutexForChangeMaps.lock();
            string valueString = to_string(simQueue.front().second);
            string toSim = "set " + simQueue.front().first + " " + valueString + "\r\n";
            int is_sent = write(socketClient, toSim.c_str(), toSim.length());
            if (is_sent < 0) {
                std::cerr << "error with send" << std::endl;
            }
            simQueue.pop();
            mutexForChangeMaps.unlock();
        }
    }
}

//method of connect Command- send to simulator.
void ConnectCommand::sendToSimulator(string simPath, float value) {
    mutexForChangeMaps.lock();
    string toSim = "set " + simPath + " " + to_string(value) + "\r\n";
    //send to simulator.
    send(socketClient, toSim.c_str(), toSim.length(), 0);
    mutexForChangeMaps.unlock();
}

int ConnectCommand::execute(vector<string> arrayStr, int i) {
    adress = arrayStr[i + 1];
    int portCoommand = stoi(arrayStr[i + 2]);
    std::thread threadClient(&ConnectCommand::connectControlClient, this, adress, portCoommand);
    threadClient.detach();
    return 2;
}

//destructor
ConnectCommand::~ConnectCommand() {
};


/**
 * Class for DefineVarCommand
 */
//constructor
DefineVarCommand::DefineVarCommand() {};

//this function execute the var and put the right values in the right maps.
int DefineVarCommand::execute(vector<string> arrayStr, int i) {
    index = i;
    //instance to symbol table
    SymbolTable *instance = instance->getInstance();
    //check the direction
    Variable check = instance->symbolTable.find(arrayStr[i])->second;
    string dir = check.getDir();

    if (dir == "->") {
        string saveName = arrayStr[i];
        //send the string after the "=" to expression
        Inter *i1 = new Inter();
        Expression *next;
        next = i1->interpret(arrayStr[i + 2]);
        //calculate the expression
        float value = next->calculate();
        string sim = instance->getVariable(arrayStr[i]).getSim();
        //push to the queue
        mutexForChangeMaps.lock();
        simQueue.push(make_pair(sim, value));
        mutexForChangeMaps.unlock();
        //erase the value from the symbol table
        if (instance->symbolTable.count(arrayStr[i])) {
            instance->symbolTable.erase(arrayStr[i]);
            string s="";
            cout << s << "" << endl;
        }
        Variable *newVar = new Variable(value, sim, "->", 0);
        //insert the value to the symbol table
        instance->symbolTable.insert({arrayStr[i], *newVar});

    } else if (dir == "<-") {
        //save the path and the value
        string sim = instance->getVariable(arrayStr[i]).getSim();
        float saveValue = instance->getValueFromSim(sim);
        Variable *insert = new Variable(saveValue, sim, dir, 1);
        //erase and insert to the symbol map
        instance->symbolTable.erase(arrayStr[i]);
        string name = arrayStr[i];
        instance->symbolTable.insert({name, *insert});
    }
    return 2;
}

//destructor
DefineVarCommand::~DefineVarCommand() {};

/**
 * Class for ConditionParser
 */
//constructor
ConditionParser::ConditionParser() {};

//destructor
ConditionParser::~ConditionParser() {};

/**
 * Class for ifCommand
 */

//constructor
ifCommand::ifCommand(int num){};
void ifCommand::executeIfLoop (int j,int i,vector<string > arrayStr){
    j = j + 5;
    SymbolTable *symbolsMaps = symbolsMaps->getInstance();
    int jump;
    while (arrayStr[j] != "}") {
        if (symbolsMaps->commandMap.count(arrayStr[j])) {
            jump = symbolsMaps->commandMap.find(arrayStr[j])->second->execute(arrayStr, j);
            j = j + jump - 1;
        }
        j++;
    }

    j = i;
    expLeft = interLeft->interpret(arrayStr[i + 1]);
    expRight = interLeft->interpret(arrayStr[i + 3]);
    right = expRight->calculate();
    left = expLeft->calculate();
}

int ifCommand::execute(vector<string> arrayStr, int i){
    int j = i + 1;
    int numVr = 0;
    //count the number of strings in the loop.
    while (arrayStr[j] != "}") {
        j++;
        numVr++;
    }
    //check the sigh of the loop
    string loopSign = arrayStr[i + 2];
    SymbolTable *symbolsMaps = symbolsMaps->getInstance();
    j = i;

    //calculate the right and left expression of the loop
    interLeft = new Inter();
    expLeft = interLeft->interpret(arrayStr[i + 1]);
    left = expLeft->calculate();
    interRight = new Inter();
    expRight = interRight->interpret(arrayStr[i + 3]);
    right = expRight->calculate();

    if (loopSign == "<=") {
        if (left <= right) {
            executeIfLoop(j,i,arrayStr);
        }
    } else if (loopSign == ">=") {
        if (left >= right) {
            executeIfLoop(j,i,arrayStr);
        }
    }else if (loopSign == "==") {
        if (left == right) {
            executeIfLoop(j,i,arrayStr);
        }
    }else if (loopSign == "!=") {
        if (left != right) {
            executeIfLoop(j,i,arrayStr);
        }
    }else if (loopSign == "<") {
        if (left < right) {
            executeIfLoop(j,i,arrayStr);
        }
    }else if (loopSign == ">") {
        if (left > right) {
            executeIfLoop(j,i,arrayStr);
        }
    }
    return numVr;
}
//destructor
ifCommand::~ifCommand(){};


/**
 * Class for loopCommand
 */
loopCommand::loopCommand(int num) : index(num) {};

void loopCommand::executeWhileLoop (int j,int i,vector<string > arrayStr){
    j = j + 5;
    SymbolTable *symbolsMaps = symbolsMaps->getInstance();
    int jump;
    while (arrayStr[j] != "}") {
        if (symbolsMaps->commandMap.count(arrayStr[j])) {
            jump = symbolsMaps->commandMap.find(arrayStr[j])->second->execute(arrayStr, j);
            j = j + jump - 1;
        }
        j++;
    }

    j = i;
    expLeft = interLeft->interpret(arrayStr[i + 1]);
    expRight = interLeft->interpret(arrayStr[i + 3]);
    right = expRight->calculate();
    left = expLeft->calculate();
}

//this method execute the loop command.
int loopCommand::execute(vector<string> arrayStr, int i) {
    int j = i + 1;
    int numVr = 0;
    //count the number of strings in the loop.
    while (arrayStr[j] != "}") {
        //not running without this
        cout<<"";
        arrayStr[j];
        j++;
        numVr++;
    }
    //check the sigh of the loop
    string loopSign = arrayStr[i + 2];
    SymbolTable *symbolsMaps = symbolsMaps->getInstance();
    j = i;

    //calculate the right and left expression of the loop
    interLeft = new Inter();
    expLeft = interLeft->interpret(arrayStr[i + 1]);
    left = expLeft->calculate();
    interRight = new Inter();
    expRight = interRight->interpret(arrayStr[i + 3]);
    right = expRight->calculate();

    //if the direction is <= :
    if (loopSign == "<=") {
        while (left <= right) {

            executeWhileLoop(j,i,arrayStr);
            //not running without this
            if ((symbolsMaps->symbolTable.count(arrayStr[i + 1]))
                && (symbolsMaps->symbolTable.find(arrayStr[i + 1])->second.getDir() == "<-")) {
            }
        }
    }
    if (loopSign == ">=") {
        while (left >= right) {
            executeWhileLoop(j,i,arrayStr);
            //not running without this
            if ((symbolsMaps->symbolTable.count(arrayStr[i + 1]))
                && (symbolsMaps->symbolTable.find(arrayStr[i + 1])->second.getDir() == "<-")) {
            }
        }
    }
    if (loopSign == ">") {
        while (left > right) {
            executeWhileLoop(j,i,arrayStr);
            //not running without this
            if ((symbolsMaps->symbolTable.count(arrayStr[i + 1]))
                && (symbolsMaps->symbolTable.find(arrayStr[i + 1])->second.getDir() == "<-")) {
            }
        }
    }
    if (loopSign == "<") {
        while (left < right) {
            executeWhileLoop(j,i,arrayStr);
        }
    }
    if (loopSign == "!=") {
        while (left != right) {
            executeWhileLoop(j,i,arrayStr);
            //not running without this
            if ((symbolsMaps->symbolTable.count(arrayStr[i + 1]))
                && (symbolsMaps->symbolTable.find(arrayStr[i + 1])->second.getDir() == "<-")) {
            }
        }
    }
    if (loopSign == "==") {
        while (left == right) {
            executeWhileLoop(j,i,arrayStr);
            //not running without this
            if ((symbolsMaps->symbolTable.count(arrayStr[i + 1]))
                && (symbolsMaps->symbolTable.find(arrayStr[i + 1])->second.getDir() == "<-")) {
            }
        }
    }

    return numVr;
}

//destructor
loopCommand::~loopCommand() {};

/**
 * Class for Print
 */
//constructor
Print::Print() {};

int Print::execute(vector<string> arrayStr, int i) {
    //print the string
    SymbolTable *instance = SymbolTable::getInstance();
    //update the value in the symbole map
    if (instance->commandMap.count(arrayStr[i + 1])) {
        mutexForChangeMaps.lock();
        updateValueInSymbolMap(arrayStr, i + 1);
        mutexForChangeMaps.unlock();
        cout << instance->getVariable(arrayStr[i + 1]).getVar()<< endl;
    } else {
        cout << arrayStr[i + 1]<< endl;
    }
    return 2;
}

//destructor
Print::~Print() {};

/**
 * Class for Sleep
 */
//constructor
Sleep::Sleep() {};

int Sleep::execute(vector<string> arrayStr, int i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(stoi(arrayStr[i + 1])));
    return 2;
}

//destructor
Sleep::~Sleep() {};
