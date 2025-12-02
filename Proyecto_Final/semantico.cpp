#include "semantico.h"
#include <stdexcept>

bool TablaSimbolos::insertar(const std::string& nombre, TipoDato tipo) {
    if (tabla.count(nombre)) return false;
    tabla[nombre] = Simbolo{nombre, tipo};
    return true;
}
bool TablaSimbolos::existe(const std::string& nombre) const {
    return tabla.count(nombre) > 0;
}
TipoDato TablaSimbolos::tipoDe(const std::string& nombre) const {
    auto it = tabla.find(nombre);
    if (it == tabla.end()) return TipoDato::TIPO_ERROR;
    return it->second.tipo;
}

bool AnalizadorSemantico::esNumerico(TipoDato t) const {
    return t == TipoDato::TIPO_INT || t == TipoDato::TIPO_FLOAT;
}

TipoDato AnalizadorSemantico::promover(TipoDato a, TipoDato b) const {
    if (a == TipoDato::TIPO_ERROR || b == TipoDato::TIPO_ERROR) return TipoDato::TIPO_ERROR;
    if (a == TipoDato::TIPO_FLOAT || b == TipoDato::TIPO_FLOAT) return TipoDato::TIPO_FLOAT;
    if (a == TipoDato::TIPO_INT && b == TipoDato::TIPO_INT) return TipoDato::TIPO_INT;
    return TipoDato::TIPO_ERROR;
}

TipoDato AnalizadorSemantico::inferirExpresion(const std::shared_ptr<ASTNode>& n) {
    // Literales
    if (n->etiqueta == "int") return TipoDato::TIPO_INT;
    if (n->etiqueta == "float") return TipoDato::TIPO_FLOAT;
    if (n->etiqueta == "id") {
        if (!ts.existe(n->lexema)) {
            throw std::runtime_error("Error semántico: identificador no declarado: " + n->lexema);
        }
        return ts.tipoDe(n->lexema);
    }

    // Unario
    if (n->etiqueta == "!" || (n->etiqueta == "-" && n->hijos.size() == 1)) {
        auto t = inferirExpresion(n->hijos[0]);
        if (n->etiqueta == "!") {
            // Tratamos bool como resultado de !, aunque no lo tokenizas explícitamente
            if (!esNumerico(t) && t != TipoDato::TIPO_BOOL) {
                throw std::runtime_error("Error semántico: operador '!' requiere bool o valor convertible.");
            }
            return TipoDato::TIPO_BOOL;
        } else { // - unario
            if (!esNumerico(t)) throw std::runtime_error("Error semántico: operador unario '-' requiere tipo numérico.");
            return t;
        }
    }

    // Binarios: relacionales, igualdad, lógicos y aritméticos
    if (n->hijos.size() == 2) {
        auto tl = inferirExpresion(n->hijos[0]);
        auto tr = inferirExpresion(n->hijos[1]);

        if (n->etiqueta == "+" || n->etiqueta == "-" || n->etiqueta == "*" || n->etiqueta == "/") {
            if (!esNumerico(tl) || !esNumerico(tr)) {
                throw std::runtime_error("Error semántico: operación aritmética requiere tipos numéricos.");
            }
            return promover(tl, tr);
        }

        if (n->etiqueta == "<" || n->etiqueta == ">" || n->etiqueta == "<=" || n->etiqueta == ">=") {
            if (!esNumerico(tl) || !esNumerico(tr)) {
                throw std::runtime_error("Error semántico: operación relacional requiere tipos numéricos.");
            }
            return TipoDato::TIPO_BOOL;
        }

        if (n->etiqueta == "==" || n->etiqueta == "!=") {
            if (tl == TipoDato::TIPO_ERROR || tr == TipoDato::TIPO_ERROR) {
                throw std::runtime_error("Error semántico: operandos inválidos en igualdad.");
            }
            // Simplificado: permitimos comparar numéricos promovidos
            if (!esNumerico(promover(tl, tr))) {
                throw std::runtime_error("Error semántico: igualdad requiere tipos compatibles.");
            }
            return TipoDato::TIPO_BOOL;
        }

        if (n->etiqueta == "&&" || n->etiqueta == "||") {
            // Permitimos numéricos como truthy; resultado bool
            if (tl == TipoDato::TIPO_ERROR || tr == TipoDato::TIPO_ERROR) {
                throw std::runtime_error("Error semántico: operación lógica con operandos inválidos.");
            }
            return TipoDato::TIPO_BOOL;
        }
    }

    return TipoDato::TIPO_ERROR;
}

void AnalizadorSemantico::visitarStmt(const std::shared_ptr<ASTNode>& n) {
    if (n->etiqueta == "stmt_list" || n->etiqueta == "programa" || n->etiqueta == "bloque") {
        for (auto &h : n->hijos) visitarStmt(h);
        return;
    }
    if (n->etiqueta == "decl") {
        if (!ts.insertar(n->lexema, n->tipoInferido)) {
            throw std::runtime_error("Error semántico: redeclaración de '" + n->lexema + "'");
        }
        return;
    }
    if (n->etiqueta == "seq_decl_asig") {
        // hijos: decl, asig
        visitarStmt(n->hijos[0]);
        visitarStmt(n->hijos[1]);
        return;
    }
    if (n->etiqueta == "asig") {
        auto id = n->hijos[0];
        auto ex = n->hijos[1];
        if (!ts.existe(id->lexema)) {
            throw std::runtime_error("Error semántico: variable no declarada '" + id->lexema + "'");
        }
        auto tId = ts.tipoDe(id->lexema);
        auto tEx = inferirExpresion(ex);
        // Reglas simples de compatibilidad: permitir promoción int->float
        if (tId == TipoDato::TIPO_INT && tEx == TipoDato::TIPO_FLOAT) {
            throw std::runtime_error("Error semántico: asignación float a int no permitida.");
        }
        if (tId == TipoDato::TIPO_FLOAT && tEx == TipoDato::TIPO_INT) {
            // permitido
        } else if (tId != tEx && !(tId == TipoDato::TIPO_FLOAT && tEx == TipoDato::TIPO_INT)) {
            throw std::runtime_error("Error semántico: tipos incompatibles en asignación.");
        }
        return;
    }
    if (n->etiqueta == "if") {
        auto cond = n->hijos[0];
        auto tCond = inferirExpresion(cond);
        (void)tCond; // aceptamos truthy
        visitarStmt(n->hijos[1]); // then
        if (n->hijos.size() == 3) visitarStmt(n->hijos[2]); // else
        return;
    }
    if (n->etiqueta == "while") {
        auto cond = n->hijos[0];
        auto tCond = inferirExpresion(cond);
        (void)tCond;
        visitarStmt(n->hijos[1]);
        return;
    }
    if (n->etiqueta == "return") {
        // Sin funciones con tipo, validación mínima
        inferirExpresion(n->hijos[0]);
        return;
    }

    // También podríamos visitar expresiones solas si las hubiera
    if (n->etiqueta == "asig") return;
}

void AnalizadorSemantico::analizar(const std::shared_ptr<ASTNode>& raiz) {
    visitarStmt(raiz);
}
