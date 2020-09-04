

#include <iostream>
#include <stack>
#include <queue>
#include <map>
#include <string.h>
#include <stdio.h>
using namespace std;
#include "ex1.h"
#include "Interpreter.h"
// Constructor for Value
Value::Value(double val)
        :value(val)
{}
//This method return value
double Value::calculate() {
    return this->value;
}
//Destructor for Value
Value::~Value()
{}
// Constructor for Variable
Var::Var(string nameStr, double val)
        :value(val)
{
    this->name = new char[nameStr.length() + 1];
    try
    {
        inputValidValue(nameStr);
    }
    catch (const char* e) {
        std::cout << e << std::endl;
    }
}

void Var::inputValidValue(string nameValue)
{
    //check that the name was good
    if (!(((nameValue[0] >= 65) && (nameValue[0] <= 90)) || ((nameValue[0] >= 97) && (nameValue[0] <= 122)) || (nameValue[0] == '_')))
    {
        throw "bad input1";
    }
    int k = 1;
    while (nameValue[k] != '\0')
    {
        if (!(((nameValue[k] >= 65) && (nameValue[k] <= 90)) || (((nameValue[k] >= 97) && (nameValue[k] <= 122)))
              || (((nameValue[k] >= 48) && (nameValue[k] <= 57))) || (nameValue[k] == '_')))
        {
            throw "bad input2";
        }
        k++;
    }
}

//This method return value
double Var::calculate() {
    return this->value;
}
//op++ method
Var& Var::operator++(){
    this->value = this->value + 1;
    return *this;
}
//op-- method
Var& Var::operator--(){
    this->value = this->value - 1;
    return *this;
}
//op+= method
Var& Var::operator+=(double num){
    this->value = this->value + num;
    return *this;
}
//op-= method
Var& Var::operator-=(double num){
    this->value = this->value - num;
    return *this;
}
//op++ method
Var& Var::operator++(int){
    this->value = this->value + 1;
    return *this;
}
//op-- method
Var& Var::operator--(int){
    this->value = this->value - 1;
    return *this;
}
//Destructor for Variable
Var::~Var() {
    delete []name;
}


//Constractor for UnaryOperator class
UnaryOperator::UnaryOperator(Expression* exp)
        : expression(exp)
{}
double UnaryOperator::calculate()
{
    return 0;
}
//Destructor for UnaryOperator class
UnaryOperator::~UnaryOperator() {
}
//Constractor for UPlus class
UPlus::UPlus(Expression* exp)
        : expression(exp)
{}
//this method calculate the expression
double UPlus::calculate()
{
    return  expression->calculate();
}
//Constractor for UPlus class
UPlus::~UPlus() {
    delete[]expression;
}
//Constractor for UMinus class
UMinus::UMinus(Expression *exp)
        : expression(exp)
{}
//this method calculate the expression
double UMinus::calculate()
{
    return -1 * expression->calculate();
}
//Destructor for UMinus class
UMinus::~UMinus() {
    //delete[]expression;
}
//Constractor for BinaryOperator class
BinaryOperator::BinaryOperator(Expression* left, Expression* right)
        : leftExpression(left), rightExpression(right)
{}
//this method calculate the expression
double BinaryOperator::calculate()
{
    return 0;
}
//Destructor for BinaryOperator class
BinaryOperator::~BinaryOperator() {
    delete[]leftExpression;
    delete[]rightExpression;
}
//Constractor for Plus class
Plus::Plus(Expression* left, Expression* right)
        : leftExpression(left), rightExpression(right)
{}
//this method calculate the expression
double Plus::calculate()
{
    return (leftExpression->calculate() + rightExpression->calculate());
}
//Destructor for Plus class
Plus::~Plus() {}
//Constractor for Minus class
Minus::Minus(Expression* left, Expression* right)
        : leftExpression(left), rightExpression(right)
{}
//this method calculate the expression
double Minus::calculate()
{
    return (leftExpression->calculate() - rightExpression->calculate());
}
//Destructor for Minus class
Minus::~Minus() {}
//Constractor for Mul class
Mul::Mul(Expression* left, Expression* right)
        : leftExpression(left), rightExpression(right)
{}
//this method calculate the expression
double Mul::calculate()
{
    return (leftExpression->calculate() * rightExpression->calculate());
}
//Destructor for Mul class
Mul::~Mul()
{}
//Constractor for Div class
Div::Div(Expression* left, Expression* right)
        : leftExpression(left), rightExpression(right)
{}
//this method calculate the expression
double Div::calculate()
{
    //dividing in zero was not valid
    if (rightExpression->calculate() != 0) {
        return (leftExpression->calculate() / rightExpression->calculate());
    }
    else {
        throw "Divid by zero";
    }
}
//Destructor for Div class
Div::~Div()
{}



//part two of homework

//Constractor for Interpreter class
Inter::Inter()
{}
//Destructor for Interpreter class
Inter::~Inter() {}
//This method accepts intfix phrase and return expression that represents it
Expression* Inter::interpret(string str)
{
    //pueue with all numbers, variabels and operators in expression
    queue <string> operands = strToQueue(str);
    //The final expression
    Expression* exp = creatExpression(operands);
    return exp;
}

//This method put all operands and operators from string expression in queue
queue<string> Inter::strToQueue(string str)
{

    //stack for all operators in expression
    stack <char> operators;
    //queue for all operators and operands in expression
    queue <string> operands;
    int countOpen = 0;
    int countClose = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ')
        {
            continue;
        }
        else if (str[i] == '(')
        {
            //if it left paren push it onto the operator stack.
            operators.push(str[i]);
            countOpen++;
        }
        else if (str[i] == '*' || str[i] == '/' || str[i] == '+' || str[i] == '-')
        {
            if (str[i+1] == '*' || str[i+1] == '/' || str[i+1] == '+' || str[i+1] == '-')
            {
                throw "illegal math expression";
            }
            /*
                if there is an operator at the top of the operator stack with greater precedence
                or equal -> pop operators from the operator stack onto the output queue.
            */
            if (!operators.empty())
            {
                while ((operatorPrecedence(str[i], operators.top())) || (operators.top() != '('))
                {
                    string s(1, operators.top());
                    operands.push(s);
                    operators.pop();
                    if (operators.empty())
                    {
                        break;
                    }
                }

            }
            //push it onto the operator stack.
            operators.push(str[i]);
        }
        else if (str[i] == ')')
        {
            countClose++;
            //while the operator at the top of the operator stack is not a left paren
            while ( !operators.empty())
            {
                if (operators.top() != '(')
                {
                    //pop the operator from the operator stack onto the output queue.
                    string s(1, operators.top());
                    operands.push(s);
                    operators.pop();
                }
                else {
                    operators.pop();
                }
            }
        }
        else
        {
            int t=i;
            while ((str[t] != '(') && (str[t] != ')') && (str[t] != '+') && (str[t] != '*') &&
                   (str[t] != '/') && (str[t] != '-') && ((unsigned)t < str.length()))
            {
                t++;
            }
            string sub = str.substr(i, t-i);
            //is digit or operand
            i = t-1;
            operands.push(sub);
        }
    }

    while (!operators.empty())
    {
        string s(1, operators.top());
        operands.push(s);
        operators.pop();
    }

    if (countClose != countOpen) {
        throw "bad input3";
    }




    //return the queue
    return operands;
}
//This methos create expression from queue
Expression* Inter::creatExpression(queue <string> operands)
{
    stack <Expression*> stackExpression;
    string s;
    // while in the steck have more one expression
    while (!operands.empty())
    {
        // s = the first string in queue
        s = operands.front();
        operands.pop();
        // if the first string in queue was operator -> need to calculate him with two previous expression
        if ((s[0] == '*') || (s[0] == '/') || (s[0] == '+') || (s[0] == '-'))
        {
            if (!operands.empty())
            {
                string s2 = operands.front();
                if ((s[0] == '-') && (s2[0] == '+'))
                {
                    operands.pop();
                    s[0] = '-';
                }
                else if ((s[0] == '+') && (s2[0] == '+'))
                {
                    operands.pop();
                    s[0] = '+';
                }
            }
            if (stackExpression.size() == 1)
            {
                Expression* e = stackExpression.top();
                stackExpression.pop();
                Expression* unary;
                if (s[0] == '+')
                {
                    unary = new UPlus(e);
                }
                else if (s[0] == '-')
                {
                    unary = new UMinus(e);
                }
                else if ((s[0] == '*') || (s[0] == '/'))
                {
                    return e;
                }
                stackExpression.push(unary);
            }
            else
            {
                Expression* e1 = stackExpression.top();
                stackExpression.pop();
                Expression* e2 = stackExpression.top();
                stackExpression.pop();
                Expression* result;
                if (s[0] == '*')
                {
                    result = new Mul(e2, e1);
                }
                else if (s[0] == '/')
                {
                    result = new Div(e2, e1);
                }
                else if (s[0] == '+')
                {
                    result = new Plus(e2, e1);
                }
                else if (s[0] == '-')
                {
                    result = new Minus(e2, e1);
                }
                stackExpression.push(result);
            }
        }
        else {
            string nextTop = s;
            if (nextTop[0] == '-')
            {
                operands.pop();
                double num = intFromString(s);
                Expression* e2 = new UMinus(new Value(num));
                stackExpression.push(e2);
            }
            else
            {
                double num = intFromString(s);
                Expression* e = new Value(num);
                stackExpression.push(e);
            }
        }
    }
    if ((operands.empty()) && (stackExpression.size() == 2))
    {
        Expression* e1 = stackExpression.top();
        stackExpression.pop();
        Expression* e2 = stackExpression.top();
        stackExpression.pop();
        Expression* result = new Mul(e1, e2);
        stackExpression.push(result);
    }
    //return stack that containing expression
    return stackExpression.top();
}
//This func insert to map variabels that declared in Main
void Inter::setVariables(string str)
{

    SymbolTable *symbolTableMap = symbolTableMap->getInstance();
    string nameValue;
    double number;
    int t = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        //key in map
        if (str[i] == '=')
        {
            string sub;
            if (t == 0)
            {
                sub = str.substr(0, i);
            }
            else
            {
                sub = str.substr(t + 1, i - t - 1);
            }
            if (symbolTableMap->symbolTable.count(sub) > 0)
            {
                symbolTableMap->symbolTable.erase(sub);
            }
            t = i;
            nameValue = sub;
            //check that the name was good
            inputValidVariable(nameValue);
        }
        //value in map
        if ((str[i] == ';')||(i==str.length()-1))
        {
            string num;
            if (i == str.length() - 1)
            {
                num = str.substr(t + 1, str.length() - 1);
            }
            else {
                num = str.substr(t + 1, i - t - 1);
            }
            t = i;
            inputValidNum(num);
            number = atof(num.c_str());
            variable.insert({ nameValue, number });
        }
    }
}

//This method check if this string was key in map.
double Inter::intFromString(string strVariable){
    double number = 0;
    SymbolTable *symbolTableMap = symbolTableMap->getInstance();
    string str = trim(strVariable);
    if (symbolTableMap->symbolTable.count(str)>0)
    {
        mutexForChangeMaps.lock();
        number = symbolTableMap->symbolTable.find(str)->second.getVar();
        mutexForChangeMaps.unlock();
    } else {
        //not running without this
        string s="";
        cout<<""<<s<<endl;
        unsigned count=0;
        for (unsigned i = 0; i < str.length(); i++)
        {
            if (((str[i] >= 48) && (str[i] <= 57)) || (str[i] == '.'))
            {
                count++;
            }
        }
        if (count == str.length())
        {
            //if the string is not variable. is digit.
            number = atof(str.c_str());
        }
        else
        {
            //interpret can contain variables that have not been set, throw an exception.
            throw "bad input4";
        }
    }
    return number;
}
//This method accepts two oprerators and check if first was greater precedence from the second.
bool Inter::operatorPrecedence(char currentOperator, char topStackOperator)
{
    if (currentOperator == '+')
    {
        if ((topStackOperator == '*') || (topStackOperator == '/') || (topStackOperator == '+'))
        {
            return true;
        }
        else
        {
            return false;
        }

    } else if (currentOperator == '-')
    {
        if ((topStackOperator == '*') || (topStackOperator == '/') || (topStackOperator == '-'))
        {
            return true;
        }
        else
        {
            return false;
        }

    }else if (currentOperator == '*')
    {
        if (topStackOperator == '*')
        {
            return true;
        }
        else
        {
            return false;
        }

    }else
    {
        //if currentOperator == '/'
        if (topStackOperator == '/')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
//throw exception if thr name of variable was not valid
void Inter::inputValidVariable(string nameValue)
{
    //check that the name was good
    if (!(((nameValue[0] >= 65) && (nameValue[0] <= 90)) || ((nameValue[0] >= 97) && (nameValue[0] <= 122)) || (nameValue[0] == '_')))
    {
        throw "bad input5";
    }
    int k = 1;
    while (nameValue[k] != '\0')
    {
        if (!(((nameValue[k] >= 65) && (nameValue[k] <= 90)) || ((nameValue[k] >= 97) && (nameValue[k] <= 122))
              || ((nameValue[k] >= 0) && (nameValue[k] <= 9)) || (nameValue[k] == '_')))
        {
            throw "bad input6";
        }
        k++;
    }
}
//throw exception if thr value was not valid
void Inter::inputValidNum(string num)
{
    int k = 0;
    while (num[k] != '\0')
    {
        if (!(((num[k] >= 48) && (num[k] <= 57)) || (num[k] == '.')))
        {
            throw "bad input7";
        }
        k++;
    }
}
string Inter::trim(const string &str) {
    size_t first = str.find_first_not_of("\t\n\v\f\r ");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of("\t\n\v\f\r ");
    return str.substr(first, (last - first + 1));
}