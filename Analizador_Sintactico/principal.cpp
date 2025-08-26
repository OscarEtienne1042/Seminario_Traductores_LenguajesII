#include <cstdlib>
#include <iostream>
#include <string>

#include "pila.h" 

using namespace std;

void ejemplo() {
    Pila pila;

    pila.push(new Terminal("hola"));
    pila.push(new Terminal("+"));
    pila.push(new Terminal("mundo"));

    pila.push(new Estado(0));
    pila.push(new NoTerminal("E"));

    cout << "*****************************************" << endl;

    pila.muestra();

}

int main(int argc, char* argv[]) {
    ejemplo();
    cin.get();
    return 0;
}
