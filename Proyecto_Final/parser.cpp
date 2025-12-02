#include "parser.h"
#include <iostream>

Parser::Parser(Lexico &l) : lex(l) {
    avanzar();
}

void Parser::avanzar() {
    tok = lex.sigSimbolo();
}

bool Parser::aceptar(int tipo) {
    if (tok == tipo) {
        avanzar();
        return true;
    }
    return false;
}

void Parser::exigir(int tipo, const std::string& msg) {
    if (!aceptar(tipo)) {
        std::cerr << "DEBUG: token actual = " << tok
                  << " simbolo = '" << lex.simbolo << "' esperado = " << tipo << std::endl;
        throw std::runtime_error("Error sintáctico: se esperaba " + msg + " cerca de '" + lex.simbolo + "'");
    }
}

std::shared_ptr<ASTNode> Parser::parse() {
    auto root = programa();
    if (tok != TipoSimbolo::PESOS) {
        throw std::runtime_error("Error sintáctico: tokens extra después del final.");
    }
    return root;
}

// programa -> stmt_list
std::shared_ptr<ASTNode> Parser::programa() {
    auto n = std::make_shared<ASTNode>("programa");
    n->add(stmt_list());
    return n;
}

// stmt_list -> stmt stmt_list | ε
std::shared_ptr<ASTNode> Parser::stmt_list() {
    auto list = std::make_shared<ASTNode>("stmt_list");
    while (tok != TipoSimbolo::PESOS && tok != TipoSimbolo::LLAVEDER) {
        list->add(stmt());
    }
    return list;
}

// stmt -> decl_or_assign ';' | '{' stmt_list '}' | if '(' expr ')' stmt [else stmt] | while '(' expr ')' stmt | return expr ';'
std::shared_ptr<ASTNode> Parser::stmt() {
    if (tok == TipoSimbolo::LLAVEIZQ) {
        avanzar();
        auto block = std::make_shared<ASTNode>("bloque");
        block->add(stmt_list());
        exigir(TipoSimbolo::LLAVEDER, "}");
        return block;
    }
    if (tok == TipoSimbolo::IF) {
        avanzar();
        exigir(TipoSimbolo::PARENIZQ, "(");
        auto cond = expr();
        exigir(TipoSimbolo::PARENDER, ")");
        auto thenS = stmt();
        auto node = std::make_shared<ASTNode>("if");
        node->add(cond);
        node->add(thenS);
        if (tok == TipoSimbolo::ELSE) {
            avanzar();
            node->add(stmt());
        }
        return node;
    }
    if (tok == TipoSimbolo::WHILE) {
        avanzar();
        exigir(TipoSimbolo::PARENIZQ, "(");
        auto cond = expr();
        exigir(TipoSimbolo::PARENDER, ")");
        auto body = stmt();
        auto node = std::make_shared<ASTNode>("while");
        node->add(cond);
        node->add(body);
        return node;
    }
    if (tok == TipoSimbolo::RETURN) {
        avanzar();
        auto e = expr();
        exigir(TipoSimbolo::PUNTOYCOMA, ";");
        auto node = std::make_shared<ASTNode>("return");
        node->add(e);
        return node;
    }
    auto da = decl_or_assign();
    exigir(TipoSimbolo::PUNTOYCOMA, ";");
    return da;
}

// decl_or_assign -> TIPO IDENT [= expr] | IDENT = expr
std::shared_ptr<ASTNode> Parser::decl_or_assign() {
    if (tok == TipoSimbolo::TIPO) {
        std::string tipoLex = lex.simbolo;
        avanzar();
        if (tok != TipoSimbolo::IDENTIFICADOR) {
            throw std::runtime_error("Se esperaba identificador en declaración.");
        }
        std::string idLex = lex.simbolo;
        avanzar();

        auto decl = std::make_shared<ASTNode>("decl", idLex);
        decl->tipoInferido = mapTipoTokenToTipoDato(tipoLex);

        if (tok == TipoSimbolo::ASIGNACION) {
            avanzar();
            auto e = expr();
            auto asig = std::make_shared<ASTNode>("asig");
            asig->add(std::make_shared<ASTNode>("id", idLex));
            asig->add(e);
            auto seq = std::make_shared<ASTNode>("seq_decl_asig");
            seq->add(decl);
            seq->add(asig);
            return seq;
        }
        return decl;
    } else if (tok == TipoSimbolo::IDENTIFICADOR) {
        std::string idLex = lex.simbolo;
        avanzar();
        exigir(TipoSimbolo::ASIGNACION, "=");
        auto e = expr();
        auto asig = std::make_shared<ASTNode>("asig");
        asig->add(std::make_shared<ASTNode>("id", idLex));
        asig->add(e);
        return asig;
    } else {
        throw std::runtime_error("Se esperaba declaración o asignación.");
    }
}

// expr precedence: || > && > == != > < > <= >= > + - > * /
std::shared_ptr<ASTNode> Parser::expr() { return or_expr(); }

std::shared_ptr<ASTNode> Parser::or_expr() {
    auto left = and_expr();
    while (tok == TipoSimbolo::OPOR) {
        std::string op = lex.simbolo;
        avanzar();
        auto right = and_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::and_expr() {
    auto left = equality_expr();
    while (tok == TipoSimbolo::OPAND) {
        std::string op = lex.simbolo;
        avanzar();
        auto right = equality_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::equality_expr() {
    auto left = rel_expr();
    while (tok == TipoSimbolo::OPIGUALDAD) {
        std::string op = lex.simbolo; // "==" o "!=" viene de tu léxico como OPIGUALDAD
        avanzar();
        auto right = rel_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::rel_expr() {
    auto left = add_expr();
    while (tok == TipoSimbolo::OPRELAC) {
        std::string op = lex.simbolo; // "<", ">", "<=", ">="
        avanzar();
        auto right = add_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::add_expr() {
    auto left = mul_expr();
    while (tok == TipoSimbolo::OPSUMA) {
        std::string op = lex.simbolo; // "+" o "-"
        avanzar();
        auto right = mul_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::mul_expr() {
    auto left = unary_expr();
    while (tok == TipoSimbolo::OPMUL) {
        std::string op = lex.simbolo; // "*" o "/"
        avanzar();
        auto right = unary_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(left); n->add(right);
        left = n;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::unary_expr() {
    if (tok == TipoSimbolo::OPNOT || tok == TipoSimbolo::OPSUMA) {
        std::string op = lex.simbolo; // "!" o "-" (unario)
        avanzar();
        auto u = unary_expr();
        auto n = std::make_shared<ASTNode>(op);
        n->add(u);
        return n;
    }
    return primary();
}

std::shared_ptr<ASTNode> Parser::primary() {
    if (tok == TipoSimbolo::IDENTIFICADOR) {
        auto n = std::make_shared<ASTNode>("id", lex.simbolo);
        avanzar();
        return n;
    }
    if (tok == TipoSimbolo::ENTERO) {
        auto n = std::make_shared<ASTNode>("int", lex.simbolo);
        avanzar();
        return n;
    }
    if (tok == TipoSimbolo::REAL) {
        auto n = std::make_shared<ASTNode>("float", lex.simbolo);
        avanzar();
        return n;
    }
    if (tok == TipoSimbolo::PARENIZQ) {
        avanzar();
        auto e = expr();
        exigir(TipoSimbolo::PARENDER, ")");
        return e;
    }
    throw std::runtime_error("Se esperaba identificador, literal o '('");
}

TipoDato Parser::mapTipoTokenToTipoDato(const std::string& tipoLexema) {
    if (tipoLexema == "int") return TipoDato::TIPO_INT;
    if (tipoLexema == "float") return TipoDato::TIPO_FLOAT;
    return TipoDato::TIPO_ERROR;
}
