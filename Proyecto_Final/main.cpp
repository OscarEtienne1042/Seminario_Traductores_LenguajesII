#include <iostream>
#include <string>
#include <stdexcept>

#include "lexico.h"
#include "parser.h"
#include "semantico.h"
#include "ir.h"

int main() {
    try {
        std::string fuente =
            "int suma = a + 3.14 * (b - 2);"
            "int a;"
            "int b;"
            "suma = a + 1;"
            "if (suma > 0) { b = b + 1; } else { b = b - 1; }"
            "while (a < 10) { a = a + 1; }"
            "return suma;"
            "$";

        Lexico lex;
        lex.entrada(fuente);

        Parser parser(lex);
        auto ast = parser.parse();

        AnalizadorSemantico sem;
        sem.analizar(ast);

        CodeGen gen;
        gen.generar(ast);

        std::cout << "Código intermedio generado:\n";
        gen.imprimir();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
    return 0;
}
