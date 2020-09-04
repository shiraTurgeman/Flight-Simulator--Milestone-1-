#ifndef PROJECT_EX1_H
#define PROJECT_EX1_H
#include <queue>
#include <map>
/**
 * Expression Interface
 */
class Expression {

public:
    virtual double calculate() = 0;
    virtual ~Expression() {}
};

using namespace std;
class Value : public Expression
{
protected:
    double value;
public:
    Value(double value);
    double calculate();
    ~Value();
};
class Var : public Expression
{
protected:
    double value;
    char* name;
public:
    Var(string name, double variable);
    void inputValidValue(string nameValue);
    double calculate();
    Var& operator++();
    Var& operator--();
    Var& operator+=(double num);
    Var& operator-=(double num);
    Var& operator++(int num);
    Var& operator--(int num);
    ~Var();
};
class UnaryOperator : virtual public Expression {
protected:
    Expression* expression;
public:
    UnaryOperator(Expression* const expression = 0);
    double calculate();
    ~UnaryOperator();
};
class UPlus : virtual public UnaryOperator {
protected:
    Expression *expression;
public:
    UPlus(Expression *expression);
    double calculate();
    ~UPlus();
};
class UMinus : virtual public UnaryOperator {
protected:
    Expression *expression;
public:
    UMinus(Expression *expression);
    double calculate();
    ~UMinus();
};
class BinaryOperator : virtual public Expression {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    BinaryOperator(Expression* const leftExpression = 0, Expression* const rightExpression = 0);
    double calculate();
    ~BinaryOperator();
};
class Plus : virtual public BinaryOperator {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    Plus(Expression* leftExpression, Expression* rightExpression);
    double calculate();
    ~Plus();
};
class Minus : virtual public BinaryOperator {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    Minus(Expression* leftExpression, Expression* rightExpression);
    double calculate();
    ~Minus();
};
class Mul : virtual public BinaryOperator {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    Mul(Expression* leftExpression, Expression* rightExpression);
    double calculate();
    ~Mul();
};
class Div : virtual public BinaryOperator {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    Div(Expression* leftExpression, Expression* rightExpression);
    double calculate();
    ~Div();
};

class Inter {
public:
    map<string, double> variable;
    Inter();
    Expression* interpret(string str);
    queue<string> strToQueue(string str);
    Expression* creatExpression(queue<string> operands);
    void setVariables(string str);
    double intFromString(string str);
    bool operatorPrecedence(char currentOperator, char topStackOperator);
    void inputValidVariable(string nameValue);
    void inputValidNum(string num);

    ~Inter();
    string trim(const string &str);
};

#endif //PROJECT_EXPRESSION_H
#pragma once
