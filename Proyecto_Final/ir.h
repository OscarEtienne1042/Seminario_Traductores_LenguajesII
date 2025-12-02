#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <memory>
#include "ast.h"

struct Instr {
    std::string texto;
};

class CodeGen {
    int tempCount = 0;
    std::vector<Instr> code;

    std::string newTemp();

    std::string genExpr(const std::shared_ptr<ASTNode>& n);
    void genStmt(const std::shared_ptr<ASTNode>& n);

public:
    void generar(const std::shared_ptr<ASTNode>& raiz);
    void imprimir() const;
    const std::vector<Instr>& instrucciones() const { return code; }
};

#endif
