
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

enum class TipoDato {
    TIPO_ERROR,
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_BOOL,
    TIPO_VOID
};

struct ASTNode {
    std::string etiqueta;
    std::string lexema;
    TipoDato tipoInferido = TipoDato::TIPO_ERROR;
    std::vector<std::shared_ptr<ASTNode>> hijos;

    ASTNode(std::string etq) : etiqueta(std::move(etq)) {}
    ASTNode(std::string etq, std::string lex) : etiqueta(std::move(etq)), lexema(std::move(lex)) {}

    void add(const std::shared_ptr<ASTNode>& h) { hijos.push_back(h); }
};

inline std::string tipoDatoToString(TipoDato t) {
    switch (t) {
        case TipoDato::TIPO_INT: return "int";
        case TipoDato::TIPO_FLOAT: return "float";
        case TipoDato::TIPO_BOOL: return "bool";
        case TipoDato::TIPO_VOID: return "void";
        default: return "error";
    }
}

#endif
