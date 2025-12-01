#ifndef _PILA
#define _PILA

#include <list>
#include <iostream>
#include <string>

using namespace std;

class ElementoPila {
public:
    virtual void muestra() = 0;
    virtual ~ElementoPila() {}
};

class Terminal : public ElementoPila {
private:
    string valor;

public:
    Terminal(string v) : valor(v) {}

    void muestra() override {
        cout << "Terminal(" << valor << ") ";
    }
};

class NoTerminal : public ElementoPila {
private:
    string nombre;

public:
    NoTerminal(string n) : nombre(n) {}

    void muestra() override {
        cout << "NoTerminal(" << nombre << ") ";
    }
};

class Estado : public ElementoPila {
private:
    int numero;

public:
    Estado(int n) : numero(n) {}

    void muestra() override {
        cout << "Estado(" << numero << ") ";
    }
};

class Pila {
private:
    list<ElementoPila*> lista;

public:
    void push(ElementoPila* x);
    ElementoPila* top(); 
    ElementoPila* pop();
    void muestra();
    ~Pila();
};

#endif