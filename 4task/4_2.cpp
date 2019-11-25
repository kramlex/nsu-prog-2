//
// Created by markd on 23.11.2019.
//
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stack>
#include <exception>

using namespace std;

// My Exception
class MyException : public exception{
    string s;

public:
    explicit MyException(string ss) : s(move(ss)) {}
    ~MyException() noexcept override = default;

    const char *what() const noexcept override {
        return s.c_str();
    }
};

// Tokens
class Token {
public:
    Token() {}
    virtual ~Token() = default;
};

class OpenBracketToken : public Token {};
class CloseBracketToken : public Token {};
class AssignOperatorToken : public Token {};
class KeywordToken : public Token {
public:
    KeywordToken(const KeywordToken &keyword) : keyword(keyword.keyword) {}
    KeywordToken(string str) : keyword(str) {}

    string& GetKeyword() {
        return keyword;
    }

private:
    string keyword;
};

class IdentifierToken : public Token {
public:
    IdentifierToken(const IdentifierToken &id) : id(id.id) {}
    IdentifierToken(string id) : id(id) {}

    string& GetId() {
        return id;
    }

private:
    string id;
};

class ValueToken : public Token {
public:
    ValueToken(const ValueToken &val) : value(val.value) {}
    ValueToken(int value) : value(value) {}

    int GetValue() {
        return value;
    }

private:
    int value;
};

// Lexer and Tokenize

const char OPEN_BRACKET = '(';
const char CLOSE_BRACKET = ')';
const char ASSIGN_OPERATOR = '=';
const char MINUS = '-';

enum class TokenizerState {
    WaitToken,
    ReadInt,
    ReadWord
};

class Tokenizer {
public:
    Tokenizer(vector<string> &keywords, istream &in) : in(in), keywords(keywords), state(TokenizerState::WaitToken) {};

    list<Token*> Tokenize() {
        while (!in.eof() && in.peek() >= 0){
            switch (state){
                case TokenizerState::WaitToken:
                    WaitForToken();
                    break;
                case TokenizerState::ReadInt:
                    ReadIntState();
                    break;
                case TokenizerState::ReadWord:
                    ReadWordState();
            }
        }

        if(state == TokenizerState::ReadInt && buff.length())
            tokens.push_back((Token*)CreateValueToken());

        if(state == TokenizerState::ReadWord && buff.length())
            tokens.push_back(CreateWordToken());

        return tokens;
    }
protected:
    void ReadIntState() {
        int ch = in.get();
        if(ch < 0) return;

        if(isdigit(ch)){
            buff += ch;
            return;
        }

        Token  *token = CreateValueToken();
        tokens.push_back(token);
        in.putback(ch);
        state = TokenizerState::WaitToken;
    }

    void ReadWordState() {
        int ch = in.get();
        if(ch < 0) return;

        if(isalnum(ch)) {
            buff += ch;
            return;
        }

        Token *token = CreateWordToken();
        tokens.push_back(token);
        in.putback(ch);
        state = TokenizerState::WaitToken;
    }

    void WaitForToken() {
        int ch = in.get();
        if(ch < 0) return;

        if(isspace(ch) || isblank(ch)) return;

        if(isalnum(ch) || ch == MINUS){
            buff += ch;
            state = isalpha(ch)
                    ? TokenizerState::ReadWord
                    : TokenizerState::ReadInt;
            return;
        }

        if(ch == OPEN_BRACKET){
            tokens.push_back((Token*)(new OpenBracketToken()));
            return;
        }

        if(ch == CLOSE_BRACKET){
            tokens.push_back((Token*)(new CloseBracketToken()));
            return;
        }

        if(ch == ASSIGN_OPERATOR){
            tokens.push_back((Token*)(new AssignOperatorToken()));
            return;
        }

        throw MyException("Unknown character: " + to_string(ch));

    }

    ValueToken *CreateValueToken() {
        ValueToken *token = new ValueToken(stoi(buff));
        buff = "";
        return token;
    }
    Token *CreateWordToken() {
        bool isKeyword = find(keywords.begin(), keywords.end(), buff) != keywords.end();
        Token *token = isKeyword
                       ? (Token*) new KeywordToken(buff)
                       : (Token*) new IdentifierToken(buff);
        buff = "";
        return token;
    }
private:
    vector<string> &keywords;
    istream &in;
    list<Token*> tokens;
    TokenizerState state;
    string buff;
};

// Abstract Syntax Tree

class Expression {
public:
    Expression() {}
    virtual ~Expression() = default;
    virtual Expression *Clone() {
        return new Expression();
    }
    virtual string ToString() {
        return "(expr)";
    }
};

class ValExpression : public Expression {
public:
    ValExpression(int val) : value(val) {};
    int GetValue() const {
        return value;
    }
    virtual Expression *Clone() {
        return (Expression*) new ValExpression(value);
    }
    virtual string ToString() {
        return "(val " + to_string(value) + ")";
    }
private:
    int value;
};

class VarExpression : public Expression {
public:
    VarExpression(string &str) : id(str) {};
    string GetId() const {
        return id;
    }

    Expression *Clone() override {
        return (Expression*) new VarExpression(id);
    }

    string ToString() override {
        return "(var " + id + ")";
    }

private:
    string id;
};

class AddExpression : public Expression {
public:
    AddExpression(Expression *left, Expression *right) : left(left), right(right) {};
    ~AddExpression() {
        delete left; delete right;
    }

    Expression *GetLeftOperand() const {
        return left;
    }

    Expression *GetRightOperand() const {
        return right;
    }

    virtual Expression *Clone() {
        return (Expression*) new AddExpression(left->Clone(), right->Clone());
    }

    virtual string ToString() {
        return "(add " + left->ToString() + " " + right->ToString() + ")";
    }
private:
    Expression *left;
    Expression *right;
};

class IfExpression : public Expression {
public:
    IfExpression(Expression *left, Expression *right, Expression *thenBranch, Expression *elseBranch) :
            left(left), right(right), thenBranch(thenBranch), elseBranch(elseBranch) {};
    ~IfExpression() {
        delete left;
        delete right;
        delete thenBranch;
        delete elseBranch;
    }

    Expression *GetLeftOperand() const {
        return left;
    }

    Expression *GetRightOperand() const {
        return right;
    }

    Expression *GetThenBranch() const {
        return thenBranch;
    }

    Expression *GetElseBranch() const {
        return elseBranch;
    }

    Expression *Clone() override {
        return (Expression*)new IfExpression(
                left->Clone(),
                right->Clone(),
                thenBranch->Clone(),
                elseBranch->Clone()
        );
    }

    string ToString() override {
        return "(if " + left->ToString() + " " + right->ToString() + " then " + thenBranch->ToString() + " else " + elseBranch->ToString() + ")";
    }

private:
    Expression *left;
    Expression *right;
    Expression *thenBranch;
    Expression *elseBranch;
};

class LetExpression : public Expression {
public:
    LetExpression(const string &id, Expression *expression, Expression *body) :
            id(id), expression(expression), body(body) {};
    ~LetExpression() {
        delete expression;
        delete body;
    }

    string GetId() const {
        return id;
    }

    Expression *GetExpression() const {
        return expression;
    }

    Expression  *GetBody() const {
        return body;
    }

    Expression *Clone() override {
        return (Expression*)new LetExpression(id, expression->Clone(), body->Clone());
    }

    string ToString() override {
        return "(let " + id + " = " + expression->ToString() + " in " + body->ToString() + ")";
    }

private:
    string id;
    Expression  *expression;
    Expression  *body;
};

class FunctionExpression : public Expression {
public:
    FunctionExpression(const string &id, Expression *body) : id(id), body(body) {};
    ~FunctionExpression() {
        delete body;
    }

    string GetId() const {
        return id;
    }

    Expression *GetBody() const{
        return body;
    }

    Expression *Clone() override {
        return (Expression*) new FunctionExpression(id, body->Clone());
    }

    string ToString() override {
        return "(function " + id + " " + body->ToString() + ")";
    }

private:
    string id;
    Expression  *body;
};

class CallExpression : public Expression {
public:
    CallExpression(Expression *callable, Expression *argument) : callable(callable), argument(argument) {};
    ~CallExpression() {
        delete callable; delete argument;
    }

    Expression *GetCallable() const {
        return callable;
    }

    Expression *GetArgument() const {
        return argument;
    }

    Expression *Clone() override {
        return (Expression*) new CallExpression(callable->Clone(), argument->Clone());
    }

    string ToString() override {
        return "(call " + callable->ToString() + " " + argument->ToString() + ")";
    }

private:
    Expression  *callable;
    Expression  *argument;
};

class BlockExpression : public Expression {
public:
    void AddExpression(Expression *expression) {
        expressions.push_back(expression);
    }
    ~BlockExpression() {
        auto it = expressions.begin();
        while (it != expressions.end()){
            delete *it;
            it++;
        }
    }

    list<Expression*> GetExpressions() const {
        return expressions;
    }

    Expression *Clone() override {
        auto block = new BlockExpression();
        for(auto p : expressions){
            block->AddExpression(p->Clone());
        }
        return (Expression*)block;
    }

    string ToString() override {
        string blockValues;
        for (auto expr : expressions) {
            blockValues += expr->ToString() + " ";
        }
        return "(block " + blockValues + ")";
    }

private:
    list<Expression*> expressions;
};

class SetExpression : public Expression {
public:
    SetExpression(const string &id, Expression *expression) :
            id(id), expression(expression) {};
    ~SetExpression() {
        delete expression;
    }

    string GetId() const {
        return id;
    }

    Expression *GetExpression() const {
        return expression;
    }

    Expression *Clone() override {
        return (Expression*) new SetExpression(id, expression->Clone());
    }

    string ToString() override {
        return "(set " + id + " " + expression->ToString() + ")";
    }

private:
    string id;
    Expression *expression;
};

// Parser

class Parser {
public:
    Parser(list<Token*> &tokens) {
        it = tokens.begin();
        end = tokens.end();
    }

    Expression *Parse() {
        return ParseExpression();
    }

    // Recursive descent parser
    Expression *ParseExpression() {
        static_cast<void>(GetToken<OpenBracketToken>());
        auto keyword = GetToken<KeywordToken>();
        Expression *expression = nullptr;

        const string &str = keyword->GetKeyword();

        if(str == "block")
            expression = (Expression*) ParseBlockExpression();
        else if(str == "val")
            expression = (Expression*) ParseValExpression();
        else if(str == "let")
            expression = (Expression*) ParseLetExpression();
        else if(str == "var")
            expression = (Expression*)ParseVarExpression();
        else if(str == "add")
            expression = (Expression*)ParseAddExpression();
        else if(str == "if")
            expression = (Expression*)ParseIfxpression();
        else if(str == "function")
            expression = (Expression*)ParseFunctionExpression();
        else if(str == "call")
            expression = (Expression*)ParseCallExpression();
        else if(str == "set")
            expression = (Expression*)ParseSetExpression();

        static_cast<void>(GetToken<CloseBracketToken>());
        return expression;
    }

protected:
    template<class T>
    T *GetToken(){
        auto token = NextToken();
        T *t = dynamic_cast<T*>(token);
        if(t) return t;
        throw MyException("Unexpected token");
    }

    KeywordToken *GetKeyword(const string &str) {
        auto token = GetToken<KeywordToken>();
        if(token->GetKeyword() != str) throw MyException("Unexpected keyword");
        return token;
    }

    Token *NextToken(){
        if(it != end){
            auto token = *it;
            it++;
            return token;
        }
        throw MyException("Unexpected end of file");
    }

    ValExpression *ParseValExpression(){
        auto valueToken = GetToken<ValueToken>();
        int value = valueToken->GetValue();
        return new ValExpression(value);
    }

    BlockExpression *ParseBlockExpression() {
        BlockExpression  *expr = new BlockExpression();
        do {
            auto token = NextToken();
            auto closeBracket = dynamic_cast<CloseBracketToken*>(token);
            it--;
            if(closeBracket) {
                break;
            } else {
                Expression *nestedExpression = ParseExpression();
                expr->AddExpression(nestedExpression);
            }

        } while (true);

        if(expr->GetExpressions().size() == 0){
            delete expr;
            throw MyException("Block epxression should have one nested expression at least");
        }
        return expr;
    }

    LetExpression *ParseLetExpression() {
        auto id = GetToken<IdentifierToken>();
        static_cast<void>(GetToken<AssignOperatorToken>());
        auto expression = ParseExpression();
        static_cast<void>(GetKeyword("in"));
        auto body = ParseExpression();
        return new LetExpression(id->GetId(), expression, body);
    }

    VarExpression *ParseVarExpression() {
        auto token = GetToken<IdentifierToken>();
        return new VarExpression(token->GetId());
    }

    AddExpression *ParseAddExpression() {
        auto left = ParseExpression();
        auto right = ParseExpression();
        return new AddExpression(left, right);
    }

    IfExpression *ParseIfxpression() {
        auto left = ParseExpression();
        auto right = ParseExpression();
        static_cast<void>(GetKeyword("then"));
        auto trueBranch = ParseExpression();
        static_cast<void>(GetKeyword("else"));
        auto elseBranch = ParseExpression();
        return new IfExpression(left, right, trueBranch, elseBranch);
    }

    FunctionExpression *ParseFunctionExpression() {
        auto id = GetToken<IdentifierToken>();
        auto body = ParseExpression();
        return new FunctionExpression(id->GetId(), body);
    }

    CallExpression *ParseCallExpression() {
        auto function = ParseExpression();
        auto argument = ParseExpression();
        return new CallExpression(function, argument);
    }

    SetExpression *ParseSetExpression() {
        auto id = GetToken<IdentifierToken>();
        auto val = ParseExpression();
        return new SetExpression(id->GetId(), val);
    }

private:
    list<Token*> tokens;
    list<Token*>::iterator it;
    list<Token*>::iterator end;
};

// Scope
class Scope{
public:
    Scope() = default;
    Scope(Scope *parent) : parentScope(parent) {}
    ~Scope() {
        for (auto p : values){
            auto expr = p.second;
            if(expr) delete expr;
        }
    }
    const Expression *GetValue(const string &key) {
        return values.at(key);
    }

    void AddValue(const string &key, Expression *value) {
//        values.insert(pair<const string, Expression*>(key, value));
        values.insert({key,value});
    }

    void SetValue(const string &key, Expression *value) {
        static_cast<void>(values.at(key));
        values[key] = value;
    }

    bool TryGet(const string &key, Expression* *value) {
        if(values.count(key)) {
            *value = values.at(key);
            return true;
        }
        return false;
    }

    bool TrySet(const string &key, Expression *value) {
        if(values.count(key)) {
            values[key] = value;
            return true;
        }
        return false;
    }
    Scope *GetParent() {
        return parentScope;
    }

private:
    Scope  *parentScope = nullptr;
    unordered_map<string, Expression*> values;
};

// Closure scope
class ClosureExpression : public FunctionExpression {
public:
    ClosureExpression(FunctionExpression *func, Scope *scope)
            : FunctionExpression(func->GetId(), func->GetBody()->Clone()), scope(scope) {}

    Scope *GetScope() {
        return scope;
    }

private:
    Scope *scope;
};

class VM {
public:
    VM() {
        scopeStack.push(new Scope());
    }
    ~VM() {
        delete PopScope();
    }

    Expression *Eval(Expression *expression) {
        if(auto valExpr = dynamic_cast<ValExpression*>(expression))
            return (Expression *) Eval(valExpr);

        if(auto varExpr = dynamic_cast<VarExpression*>(expression))
            return Eval(varExpr);

        if(auto addExpr = dynamic_cast<AddExpression*>(expression))
            return (Expression*)Eval(addExpr);

        if(auto ifExpr = dynamic_cast<IfExpression*>(expression))
            return Eval(ifExpr);

        if(auto letExpr = dynamic_cast<LetExpression*>(expression))
            return Eval(letExpr);

        if(auto funcExpr = dynamic_cast<FunctionExpression*>(expression))
            return (Expression*)Eval(funcExpr);

        if(auto callExpr = dynamic_cast<CallExpression*>(expression))
            return (Expression*)Eval(callExpr);

        if(auto setExpr = dynamic_cast<SetExpression*>(expression))
            return (Expression*)Eval(setExpr);

        if(auto blockExpr = dynamic_cast<BlockExpression*>(expression))
            return Eval(blockExpr);

        throw MyException("Unknown expression type");
    }

protected:
    Scope *CurrentScope() const {
        return scopeStack.top();
    }

    void PushScope(Scope *scope){
        scopeStack.push(scope);
    }

    Scope *PopScope(){
        auto res = scopeStack.top();
        scopeStack.pop();
        return res;
    }

    static ValExpression *Eval(ValExpression *val){
        return (ValExpression*) val->Clone();
    }

    Expression *Eval(VarExpression *var){
        auto id = var->GetId();
        return FromEnv(id);
    }

    ValExpression *Eval(AddExpression *expr) {
        auto leftExpr = Eval(expr->GetLeftOperand());
        auto rightExpr = Eval(expr->GetRightOperand());
        return new ValExpression(GetValue(leftExpr) + GetValue(rightExpr));
    }

    Expression *Eval(IfExpression *expr) {
        auto valLeft = GetValue(Eval(expr->GetLeftOperand()));
        auto valRight = GetValue(Eval(expr->GetRightOperand()));
        return Eval(valLeft > valRight ? expr->GetThenBranch() : expr->GetElseBranch());
    }

    Expression *Eval(LetExpression *expr) {
        auto id = expr->GetId();
        auto value = Eval(expr->GetExpression());
        Scope *scope = new Scope(CurrentScope());
        PushScope(scope);
        scope->AddValue(id, value);
        auto result = Eval(expr->GetBody());
        delete PopScope();
        return result;
    }

    ClosureExpression *Eval(FunctionExpression *func) {
        return new ClosureExpression(func, CurrentScope());
    }

    Expression *Eval(CallExpression *expr) {
        auto res = Eval(expr->GetCallable());
        auto called = dynamic_cast<FunctionExpression*>(res);
        if(called == nullptr)
            throw MyException("Can't call not function expression");
        auto closure = dynamic_cast<ClosureExpression*>(res);

        auto argument = Eval(expr->GetArgument());

        Scope *parentScope = closure ? closure->GetScope() : CurrentScope();
        Scope *scope = new Scope(parentScope);
        scope->AddValue(called->GetId(), argument);
        PushScope(scope);

        auto result = Eval(called->GetBody());
        delete PopScope();
        return result;
    }

    Expression *Eval(SetExpression *expr) {
        auto id = expr->GetId();
        auto value = Eval(expr->GetExpression());
        if(!TrySetInEnv(id, value)){
            throw MyException("Variable doesn't exists " + id);
        }
        return new Expression();
    }

    Expression *Eval(BlockExpression *block) {
        Expression *result = nullptr;
        Scope *scope = new Scope(CurrentScope());
        PushScope(scope);
        for (auto expr : block->GetExpressions()){
            if(result) delete result;
            result = Eval(expr);
        }
        delete PopScope();
        return result;
    }

    static int GetValue(Expression *expression) {
        auto valExpression = dynamic_cast<ValExpression*>(expression);
        if(valExpression) return valExpression->GetValue();
        throw MyException("Expression isn't value expression");
    }

    Expression *FromEnv(string &key) {
        Expression *expr = nullptr;
        Scope *scope;
        for (scope = CurrentScope(); scope != nullptr; scope = scope->GetParent()) {
            if(scope->TryGet(key, &expr)) break;
        }
        if(scope != nullptr) return expr->Clone();
        throw MyException("Unknown key " + key);
    }

    bool TrySetInEnv(string &key, Expression *expression) {
        Scope *scope;
        for (scope = CurrentScope(); scope != nullptr; scope = scope->GetParent()){
            if(scope->TrySet(key, expression)) return true;
        }
        return false;
    }

private:
    stack<Scope*> scopeStack;
};


int main(){
    vector<string> keywords =
            {"val", "var", "add", "if",
             "then", "else", "let", "in",
             "function", "call", "set", "block"};
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    try {
        Tokenizer tokenizer(keywords, fin);
        auto tokens = tokenizer.Tokenize();

        Parser parser(tokens);
        auto AST = parser.Parse();

        VM vm;
        auto expression = vm.Eval(AST);

        if(dynamic_cast<ValExpression*>(expression) == nullptr){
            throw MyException("Wrong expression type");
        }
        fout << expression->ToString();
        delete AST;
        delete expression;

        for(auto i : tokens){
            delete i;
        }
    }
    catch(MyException e) {
        fout << "ERROR";
    }
}