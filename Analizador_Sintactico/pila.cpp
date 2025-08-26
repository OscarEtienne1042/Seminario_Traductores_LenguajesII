#include "pila.h"
#include "lexico.h"
#include <sstream>
#include <iostream>
#include <list>

using namespace std;

void Pila::push(ElementoPila* x) {
    lista.push_front(x);
}

ElementoPila* Pila::pop() {
    if (lista.empty()) return nullptr;

    ElementoPila* x = *lista.begin();
    lista.erase(lista.begin());
    return x;
}

ElementoPila* Pila::top() {
    if (lista.empty()) return nullptr;

    return *lista.begin();
}


void Pila::muestra() {
    for (ElementoPila* e : lista) {
        e->muestra(); // llamada virtual
    }
    cout << endl;
}

Pila::~Pila() {
    for (ElementoPila* e : lista) {
        delete e;
    }
    lista.clear();
}
