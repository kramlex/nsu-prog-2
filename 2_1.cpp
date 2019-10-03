//
// Created by markd on 03.10.2019.
//

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cctype>
#include <exception>
#include <fstream>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

class MyException : public exception{
    string s;

public:
    explicit MyException(string ss) : s(std::move(ss)) {}
    ~MyException() noexcept override = default;

    const char *what() const noexcept override {
        return s.c_str();
    }
};

enum TokenType {
    NUMBER,
    VARIABLE,
    ADD,
    SUB,
    MUL,
    DIV,
    L_BRACKET,
    R_BRACKET
};

struct Token {
    TokenType type;
    union {
        double number;
        char variable;
    };
    string str;
};

// CLASSES

class Expression{
public:
    virtual Expression *diff(char) = 0;
    virtual void print() = 0;
    virtual void print(ofstream&) = 0;

    virtual ~Expression() {};
};

class Number : public Expression {
    double number;
public:
    Number() : number(0.0) {}
    explicit Number(const double& n){
        number = n;
    }
    ~Number() override = default;

    Expression* diff(char _) override {
        return new Number(0.0);
    }

    void print() override {
        cout << number;
    }
    void print(ofstream& out) override {
        out << number;
    }
};

class Variable : public Expression {
    char var;
public:
    Variable() : var('x') {}
    explicit Variable(char const var) : var(var) {}
    ~Variable() override {}

    void print() override {
        cout << var;
    }

    void print(ofstream &ofstream1) override {
        ofstream1 << var;
    }

    Expression* diff (char ch) override {
        return new Number((ch = var) ? 1. : 0.);
    }
};

class Add : public Expression{
    Expression* left;
    Expression *right;

public:
    Add(Expression *left, Expression* right) : left(left) , right(right) {}
    ~Add() override {
        delete left;
        delete right;
    }

    Expression* diff(char ch) override{
        return new Add(left->diff(ch), right->diff(ch));
    }
    void print() override{
        cout << "(";
        left->print();
        cout << "+";
        right->print();
        cout << ")";
    }
    void print(ofstream& ofstream1) override {
        ofstream1 << "(";
        left->print(ofstream1);
        ofstream1 << "+";
        right->print(ofstream1);
        ofstream1 << ")";
    }
};

class Sub : public Expression{
    Expression* left;
    Expression* right;
public:
    Sub(Expression* left, Expression* right) : left(left) , right(right) {}
    ~Sub() override{
        delete left;
        delete right;
    }

    Expression* diff(char ch) override{
        return new Sub(left->diff(ch) , right->diff(ch));
    }
    void print() override{
        cout << "(";
        left->print();
        cout << "-";
        right->print();
        cout<< ")";
    }

    void print(ofstream &ofstream1) override {
        ofstream1 << "(";
        left->print(ofstream1);
        ofstream1 << "-";
        right->print(ofstream1);
        ofstream1 << ")";
    }
};

class Mul : public Expression{
    Expression* left;
    Expression* right;
public:
    Mul(Expression* left , Expression* right) : left(left) , right(right) {}
    ~Mul() override{
        delete left;
        delete right;
    }
    Expression* diff(char ch) override{
        return new Add(new Mul(left->diff(ch), right) , new Mul(left , right->diff(ch)));
    }
    void print() override{
        cout << "(";
        left->print();
        cout << "*";
        right->print();
        cout << ")";
    }

    void print(ofstream &ofstream1) override {
        ofstream1 << "(";
        left->print(ofstream1);
        ofstream1 << "*";
        right->print(ofstream1);
        ofstream1 << ")";
    }
};

class Div : public Expression {
    Expression* left;
    Expression* right;
public:
    Div(Expression* left , Expression* right) : left(left) , right(right) {}
    ~Div() override{
        delete left;
        delete right;
    }
    Expression* diff(char ch) override{
        return new Div(
                new Sub(new Mul(left->diff(ch) , right) , new Mul(left , right->diff(ch))),
                new Mul(right, right)
                );
    }
    void print() override{
        cout << "(";
        left->print();
        cout << "/";
        right->print();
        cout << ")";
    }

    void print(ofstream &ofstream1) override {
        ofstream1 << "(";
        left->print(ofstream1);
        ofstream1 << "/";
        right->print(ofstream1);
        ofstream1 << ")";
    }
};

class Minus : public Expression {
    Expression* val;
public:
    explicit Minus(Expression* expression) : val(expression) {}

    ~Minus() override {
        delete val;
    }

    Expression *diff(char ch) override {
        return new Minus(val->diff(ch));
    }

    void print() override {
        cout << "-";
        val->print();
    }

    void print(ofstream &ofstream1) override {
        ofstream1 << "-";
        val->print(ofstream1);
    }
};

// PARSING
void skip_whitespace(string &str , int &pos){
    while(isspace(str[pos]) && pos < str.length())
        ++pos;
}

std::vector<Token> tokenize(string& str) {
    vector<Token> tokens;
    int pos = 0;
    int const length = str.length();
    while (pos < length) {
        skip_whitespace(str, pos);

        Token token;

        if (isalpha(str[pos])) {
            char var = str[pos];

            if (pos + 1 == length || !isalpha(str[pos + 1])) {
                token.type = VARIABLE;
                token.variable = var;
                token.str = var;
                tokens.push_back(token);
                ++pos;
            }
        }
        else if (isdigit(str[pos]) || (str[pos] == '.' && pos + 1 < length && isdigit(str[pos + 1]))) {
            string num;
            num += str[pos++];
            bool point_found = false;
            while (isdigit(str[pos]) || str[pos] == '.') {
                if (str[pos] == '.') {
                    if (!point_found) {
                        point_found = true;
                    }
                    else {
                        throw MyException("too many points in number");
                    }
                }
                num += str[pos++];
            }
            token.type = NUMBER;
            token.number = stod(num); // basic_string
            token.str = num;
            tokens.push_back(token);
        }
        else {
            switch (str[pos]) {
                case '+': {
                    token.type = ADD;
                    token.str = "+";
                    break;
                }
                case '-': {
                    token.type = SUB;
                    token.str = "-";
                    break;
                }
                case '*': {
                    token.type = MUL;
                    token.str = "*";
                    break;
                }
                case '/': {
                    token.type = DIV;
                    token.str = "/";
                    break;
                }
                case '(': {
                    token.type = L_BRACKET;
                    token.str = "(";
                    break;
                }
                case ')': {
                    token.type = R_BRACKET;
                    token.str = ")";
                    break;
                }
                default: {
                    throw MyException("unexppected character");
                }
            }
            tokens.push_back(token);
            ++pos;
        }
    }
    return tokens;
}

inline int get_precendence(Token token) {
    if (token.type == NUMBER || token.type == VARIABLE) return 0;
    if (token.type == L_BRACKET || token.type == R_BRACKET) return 1;
    if (token.type == ADD || token.type == SUB || token.type == MUL) return 3;
    return 2;
}

int get_highest_precedence_token(vector<Token> &tokens, int start, int stop) {
    int prec = -1;
    int indx = 0;
    int brackets = 0;
    for (int i = start; i < stop; i++) {
        int p = get_precendence(tokens[i]);
        if (p > prec || (p == prec && p > 1)) {
            prec = p;
            indx = i;
        }
        if (tokens[i].type == L_BRACKET) {
            brackets = 1;
            ++i;
            while (brackets > 0 && i < stop) {
                if (tokens[i].type == L_BRACKET) {
                    ++brackets;
                }
                if (tokens[i].type == R_BRACKET) {
                    --brackets;
                }
                i++;
            }
            if (brackets > 0){
                throw MyException("unbalanced brackets");
            }
            i--;
        }
    }
    return indx;
}

int find_close_bracket(vector<Token>& tokens, int start, int stop) {
    int pos = start + 1;
    int brackets = 1;
    while (brackets && pos < stop) {
        if (tokens[pos].type == L_BRACKET) {
            brackets++;
        }
        if (tokens[pos].type == R_BRACKET) {
            brackets--;
        }
        pos++;
    }
    if (brackets > 0) {
        throw MyException("unbalanced brackets");
    }
    pos--;
    return pos;
}

Expression* build_expression(vector<Token>& tokens , int start , int stop){
    if(stop - start == 1){
        Token &token = tokens[start];
        if(token.type == VARIABLE){
            return new Variable(token.variable);
        }
        if(token.type == NUMBER){
            return new Number(token.number);
        }
        throw MyException("unexpected single token");
    }
    int index = get_highest_precedence_token(tokens, start, stop);
    Token &token = tokens[index];

    if (index == start) {
        if (token.type == SUB) {
            return new Minus(build_expression(tokens, start + 1, stop));
        }
        else if (token.type == L_BRACKET) {
            int close_bracket = find_close_bracket(tokens, index, stop);
            if (close_bracket != stop - 1) {
                throw MyException("brackets are not balanced");
            }
            return build_expression(tokens, index + 1, close_bracket);
        }
        throw MyException("unexpected token with start index");
    }

    Expression* left = build_expression(tokens, start, index);
    Expression* right = build_expression(tokens, index + 1, stop);

    if (token.type == ADD) {
        return new Add(left, right);
    }
    if (token.type == SUB) {
        return new Sub(left, right);
    }
    if (token.type == MUL) {
        return new Mul(left, right);
    }
    if (token.type == DIV) {
        return new Div(left, right);
    }

    throw MyException("unexpected token");

}

Expression* build_expression(vector<Token> tokens) {
    return build_expression(tokens, 0, tokens.size());
}

Expression* parse_expression(string str){
    return build_expression(tokenize(str));
}



int main(){
    string str;
    getline(fin , str);
    Expression * expression = parse_expression(str);
    expression->diff('x')->print(fout);
    return 0;
}

