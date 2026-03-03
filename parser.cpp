#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.h"

#include "lexer.cpp"

// default types
enum Type {
    Type_u8,
    Type_u16,
    Type_u32,
    Type_u64,
    Type_i8,
    Type_i16,
    Type_i32,
    Type_i64,
    Type_f32,
    Type_f64,
    Type_char8,
    Type_char16,
    Type_char32,
    Type_string,
};

struct Parameter {
    std::string name;
    Type type;
};

struct Statement {
    int a;
};

struct FunctionDefinition {
    std::vector<Parameter> parameters;
    Type return_type;
    std::string name;
    std::vector<Statement> statements;
};

enum Operation {
    Op_Plus,
    Op_Minus,
    Op_Mul,
    Op_Div,
    Op_Mod,
};

enum ExprKind {
    Expr_Atom,
    Expr_Operator,
};

struct Expr {
    ExprKind kind;
    union {
        Token t; // (This is temporary) Expr_Atom
        struct {     // Expr_Operator
            Operation op;
            Expr *left;
            Expr *right;
        };
    };
};

Expr *make_op(Expr *left, Expr *right, Operation op)
{
    Expr *e = new Expr();

    e->kind = Expr_Operator;
    e->op = op;
    e->left = left;
    e->right = right;

    return e;
}

std::unordered_map<Operation, int> precedence_table = {
    {Op_Plus,  1},
    {Op_Minus, 1},
    {Op_Mul,   2},
    {Op_Div,   2},
    {Op_Mod,   2},
};

int main() {

    std::ifstream file("main.at");
    std::ostringstream ss;

    ss << file.rdbuf();

    std::string source_code = ss.str();

    Lexer l = lexer_lex_file(source_code);

    while (!lexer_is_eof(l)) {
        lexer_print_token(lexer_current(l));
        lexer_move_next(l);
    }

    return 0;
}

