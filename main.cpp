
#include <iostream>
#include <string>
#include <vector>
#include "Interpreter.h"
using namespace std;
int main(int argc, char *argv[]){
    try{
        Interpreter inter;
        SymbolTable *instance = SymbolTable::getInstance();
        //insert the paths to the sim map
        for (int t=0; t<36; t++){
            instance->putInSimMap(t,0);
        }
        vector<string> splittedStrings = inter.lexer(argv[1]);
        //execute all the commands
        inter.parser (splittedStrings);
    }catch(const char* error){
        cout << error << endl;
    }
    return 0;
}
