#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <stdexcept>
#include <string>
#include "lexico.h"
#include "ast.h"

class Parser {
    Lexico &lex;
    int tok;

    void avanzar();
    bool aceptar(int tipo);
    void exigir(int tipo, const std::string& msg);

    std::shared_ptr<ASTNode> programa();
    std::shared_ptr<ASTNode> stmt_list();
    std::shared_ptr<ASTNode> stmt();
    std::shared_ptr<ASTNode> decl_or_assign();
    std::shared_ptr<ASTNode> expr();
    std::shared_ptr<ASTNode> or_expr();
    std::shared_ptr<ASTNode> and_expr();
    std::shared_ptr<ASTNode> equality_expr();
    std::shared_ptr<ASTNode> rel_expr();
    std::shared_ptr<ASTNode> add_expr();
    std::shared_ptr<ASTNode> mul_expr();
    std::shared_ptr<ASTNode> unary_expr();
    std::shared_ptr<ASTNode> primary();

    TipoDato mapTipoTokenToTipoDato(const std::string& tipoLexema);

public:
    Parser(Lexico &l);
    std::shared_ptr<ASTNode> parse();
};

#endif
