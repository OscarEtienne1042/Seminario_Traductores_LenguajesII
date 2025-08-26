#include <iostream>
#include "lexico.h"
using namespace std;

int main() {
    Lexico lexico;
    lexico.entrada("int suma = a + 3.14 * (b - 2);");

    cout << "Resultado del Analisis Lexico\n\n";
    cout << "Simbolo\t\tTipo\n";
    cout << "-----------------------------\n";

    while (!lexico.terminado()) {
        lexico.sigSimbolo();
        if (lexico.tipo != TipoSimbolo::ERROR) {
            cout << lexico.simbolo << "\t\t" << lexico.tipoAcad(lexico.tipo) << endl;
        }
    }

    return 0;
}
