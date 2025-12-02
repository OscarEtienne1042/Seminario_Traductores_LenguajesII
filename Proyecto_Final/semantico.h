#ifndef SEMANTICO_H
#define SEMANTICO_H

#include <map>
#include <string>
#include <memory>
#include "ast.h"

struct Simbolo {
    std::string nombre;
    TipoDato tipo;
};

class TablaSimbolos {
    std::map<std::string, Simbolo> tabla;
public:
    bool insertar(const std::string& nombre, TipoDato tipo);
    bool existe(const std::string& nombre) const;
    TipoDato tipoDe(const std::string& nombre) const;
};

class AnalizadorSemantico {
    TablaSimbolos ts;

    TipoDato inferirExpresion(const std::shared_ptr<ASTNode>& nodo);
    bool esNumerico(TipoDato t) const;
    TipoDato promover(TipoDato a, TipoDato b) const;
    void visitarStmt(const std::shared_ptr<ASTNode>& nodo);

public:
    void analizar(const std::shared_ptr<ASTNode>& raiz);
    const TablaSimbolos& tabla() const { return ts; }
};

#endif
