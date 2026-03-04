#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

enum Operator {
    Op_Add,
    Op_Sub,
    Op_Mul,
    Op_Div,
    Op_Mod,
};

enum OpAss {
    OpAss_Left,
    OpAss_Right,
};

enum ExprKind {
    Expr_Atom,
    Expr_Operator,
};

struct Expr {
    ExprKind kind;
    std::string blah;
    union {
        TokenKind t; // (This is temporary) Expr_Atom
        struct {     // Expr_Operator
            Operator op;
            Expr *left;
            Expr *right;
        };
    };
};

Expr* parse_primary(Lexer& l);

Expr *make_op(Expr *left, Expr *right, Operator op)
{
    Expr *e = new Expr();

    e->kind = Expr_Operator;
    e->op = op;
    e->left = left;
    e->right = right;

    return e;
}

// a + b * c

std::unordered_map<Operator, int> precedence_table = {
    {Op_Add,  1},
    {Op_Sub, 1},
    {Op_Mul,   2},
    {Op_Div,   2},
    {Op_Mod,   3},
};

std::unordered_map<Operator, OpAss> opass_table = {
    {Op_Add, OpAss_Left},
    {Op_Sub, OpAss_Left},
    {Op_Mul, OpAss_Left},
    {Op_Div, OpAss_Left},
    {Op_Mod, OpAss_Left},
};

std::unordered_map<TokenKind, Operator> op_table = {
    {Tok_Plus, Op_Add},
    {Tok_Minus, Op_Sub},
    {Tok_Star, Op_Mul},
    {Tok_FSlash, Op_Div},
    {Tok_Percentage, Op_Mod},

};



bool is_op(Token t){
    auto it = op_table.find(t.kind);

    return !(it == op_table.end());
}

Expr* parse_expression(Lexer& l, int min_prec){
    Expr* primary_lhs = parse_primary(l);
    while(true){
        if (!is_op(lexer_current(l))){
            break;
        }
        Operator op = op_table[lexer_current(l).kind];
        if (precedence_table[op]<min_prec){
            break;
        }
        lexer_next(l);

        int next_min_prec;

        switch (opass_table[op]) {
            case OpAss_Left:
                next_min_prec = precedence_table[op]+1; break;
            case OpAss_Right:
                next_min_prec = precedence_table[op]; break;
        }

        Expr* primary_rhs = parse_expression(l, next_min_prec);

        primary_lhs = make_op(primary_lhs,primary_rhs, op );
    }

    return primary_lhs;
}

Expr* parse_primary(Lexer& l){
    Expr *e = nullptr;
    switch (lexer_current(l).kind) {
        case Tok_Identifier:
        case Tok_Number:
            e = new Expr();
            e->kind = Expr_Atom;
            e->t = lexer_current(l).kind;
            e->blah = lexer_current(l).literal;
            break;
        case Tok_LParen:
            lexer_next(l);
            e = parse_expression(l, 0);
            if(lexer_current(l).kind != Tok_RParen){
                std::cout<<"bleh bluh bluh"<<std::endl;
            }
            lexer_next(l);
            return e;

            break;
        default:
            std::cout << "Bleh Bleh Bleh Bluh Bluh Bluh\n";
    }
    lexer_next(l);

    return e;
}



int main() {

    std::ifstream file("main.at");
    std::ostringstream ss;

    ss << file.rdbuf();

    std::string source_code = ss.str();

    Lexer l = lexer_lex_file(source_code);

    while (!lexer_is_eof(l)) {
        lexer_print_token(lexer_current(l));
        lexer_next(l);
    }

    l.current = 0;
    Expr *ast = parse_expression(l, 0);

    return 0;
}

