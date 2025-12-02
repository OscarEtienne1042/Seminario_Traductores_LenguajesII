#include "ir.h"
#include <stdexcept>
#include <iostream>

std::string CodeGen::newTemp() {
    return "t" + std::to_string(++tempCount);
}

std::string CodeGen::genExpr(const std::shared_ptr<ASTNode>& n) {
    // Literales e identificadores
    if (n->etiqueta == "int" || n->etiqueta == "float" || n->etiqueta == "id") {
        return n->lexema;
    }
    // Unario
    if (n->etiqueta == "!" || (n->etiqueta == "-" && n->hijos.size() == 1)) {
        auto v = genExpr(n->hijos[0]);
        auto t = newTemp();
        code.push_back({ t + " = " + n->etiqueta + " " + v });
        return t;
    }
    // Binario
    if (n->hijos.size() == 2) {
        auto l = genExpr(n->hijos[0]);
        auto r = genExpr(n->hijos[1]);
        auto t = newTemp();
        code.push_back({ t + " = " + l + " " + n->etiqueta + " " + r });
        return t;
    }
    throw std::runtime_error("No se puede generar código para el nodo: " + n->etiqueta);
}

void CodeGen::genStmt(const std::shared_ptr<ASTNode>& n) {
    if (n->etiqueta == "programa" || n->etiqueta == "stmt_list" || n->etiqueta == "bloque") {
        for (auto &h : n->hijos) genStmt(h);
        return;
    }
    if (n->etiqueta == "decl") {
        // En IR, solo registramos declaración como comentario (no hay memoria real aquí)
        code.push_back({ "// decl " + n->lexema + " : " + tipoDatoToString(n->tipoInferido) });
        return;
    }
    if (n->etiqueta == "seq_decl_asig") {
        genStmt(n->hijos[0]);
        genStmt(n->hijos[1]);
        return;
    }
    if (n->etiqueta == "asig") {
        auto id = n->hijos[0]; // id
        auto e = n->hijos[1];
        auto v = genExpr(e);
        code.push_back({ id->lexema + " = " + v });
        return;
    }
    if (n->etiqueta == "if") {
        auto cond = genExpr(n->hijos[0]);
        std::string Ltrue = "L" + std::to_string(tempCount + 1);
        std::string Lend = "L" + std::to_string(tempCount + 2);
        code.push_back({ "if " + cond + " goto " + Ltrue });
        // else
        if (n->hijos.size() == 3) {
            // salto al else
            auto elseLabel = "L" + std::to_string(tempCount + 3);
            code.push_back({ "goto " + elseLabel });
            code.push_back({ Ltrue + ":" });
            genStmt(n->hijos[1]); // then
            code.push_back({ "goto " + Lend });
            code.push_back({ elseLabel + ":" });
            genStmt(n->hijos[2]); // else
            code.push_back({ Lend + ":" });
        } else {
            code.push_back({ Ltrue + ":" });
            genStmt(n->hijos[1]);
            code.push_back({ Lend + ":" });
        }
        return;
    }
    if (n->etiqueta == "while") {
        std::string Lbegin = "L" + std::to_string(tempCount + 1);
        std::string Lcond = "L" + std::to_string(tempCount + 2);
        std::string Lend = "L" + std::to_string(tempCount + 3);
        code.push_back({ Lbegin + ":" });
        auto cond = genExpr(n->hijos[0]);
        code.push_back({ "if " + cond + " goto " + Lcond });
        code.push_back({ "goto " + Lend });
        code.push_back({ Lcond + ":" });
        genStmt(n->hijos[1]);
        code.push_back({ "goto " + Lbegin });
        code.push_back({ Lend + ":" });
        return;
    }
    if (n->etiqueta == "return") {
        auto v = genExpr(n->hijos[0]);
        code.push_back({ "return " + v });
        return;
    }
    // Expresión suelta (no contemplada en gramática de stmt)
}

void CodeGen::generar(const std::shared_ptr<ASTNode>& raiz) {
    genStmt(raiz);
}

void CodeGen::imprimir() const {
    for (auto &i : code) {
        std::cout << i.texto << "\n";
    }
}
