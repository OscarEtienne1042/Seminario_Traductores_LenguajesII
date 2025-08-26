#include "lexico.h"
#include <cctype> // Para isalpha, isdigit
using namespace std;

Lexico::Lexico(string fuente) {
    this->fuente = fuente;
    ind = 0;
}

Lexico::Lexico() {
    fuente = "";
    ind = 0;
}

void Lexico::entrada(string fuente) {
    this->fuente = fuente;
    ind = 0;
}

string Lexico::tipoAcad(int tipo) {
    switch (tipo) {
        case TipoSimbolo::IDENTIFICADOR: return "0";
        case TipoSimbolo::ENTERO: return "1";
        case TipoSimbolo::REAL: return "2";
        case TipoSimbolo::CADENA: return "3";
        case TipoSimbolo::TIPO: return "4";
        case TipoSimbolo::OPSUMA: return "5";
        case TipoSimbolo::OPMUL: return "6";
        case TipoSimbolo::OPRELAC: return "7";
        case TipoSimbolo::OPOR: return "8";
        case TipoSimbolo::OPAND: return "9";
        case TipoSimbolo::OPNOT: return "10";
        case TipoSimbolo::OPIGUALDAD: return "11";
        case TipoSimbolo::PUNTOYCOMA: return "12";
        case TipoSimbolo::COMA: return "13";
        case TipoSimbolo::PARENIZQ: return "14";
        case TipoSimbolo::PARENDER: return "15";
        case TipoSimbolo::LLAVEIZQ: return "16";
        case TipoSimbolo::LLAVEDER: return "17";
        case TipoSimbolo::ASIGNACION: return "18";
        case TipoSimbolo::IF: return "19";
        case TipoSimbolo::WHILE: return "20";
        case TipoSimbolo::RETURN: return "21";
        case TipoSimbolo::ELSE: return "22";
        case TipoSimbolo::PESOS: return "23";
        default: return "Error";
    }
}

bool Lexico::terminado() {
    return ind >= fuente.length();
}

char Lexico::sigCaracter() {
    if (ind < fuente.length()) {
        return fuente[ind++];
    } else {
        return '$'; // Fin de entrada
    }
}

void Lexico::retroceso() {
    if (ind > 0) ind--;
}

bool Lexico::esLetra(char c) {
    return isalpha(c);
}

bool Lexico::esDigito(char c) {
    return isdigit(c);
}

bool Lexico::esEspacio(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void Lexico::sigEstado(int nuevoEstado) {
    estado = nuevoEstado;
    simbolo += c;
}

void Lexico::aceptacion(int estadoFinal) {
    estado = estadoFinal;
    simbolo += c;
    continua = false;
}

int Lexico::sigSimbolo() {
    estado = 0;
    continua = true;
    simbolo = "";

    while (continua) {
        c = sigCaracter();

        switch (estado) {
            case 0:
                if (esEspacio(c)) continue;
                if (esLetra(c)) sigEstado(1);
                else if (esDigito(c)) sigEstado(10);
                else if (c == '+' || c == '-') aceptacion(TipoSimbolo::OPSUMA);
                else if (c == '*' || c == '/') aceptacion(TipoSimbolo::OPMUL);
                else if (c == '=') sigEstado(4);
                else if (c == '!') sigEstado(5);
                else if (c == '<' || c == '>') sigEstado(6);
                else if (c == '&') sigEstado(7);
                else if (c == '|') sigEstado(8);
                else if (c == ';') aceptacion(TipoSimbolo::PUNTOYCOMA);
                else if (c == ',') aceptacion(TipoSimbolo::COMA);
                else if (c == '(') aceptacion(TipoSimbolo::PARENIZQ);
                else if (c == ')') aceptacion(TipoSimbolo::PARENDER);
                else if (c == '{') aceptacion(TipoSimbolo::LLAVEIZQ);
                else if (c == '}') aceptacion(TipoSimbolo::LLAVEDER);
                else if (c == '$') aceptacion(TipoSimbolo::PESOS);
                else aceptacion(TipoSimbolo::ERROR);
                break;

            case 1:
                if (esLetra(c) || esDigito(c)) sigEstado(1);
                else {
                    retroceso();
                    continua = false;
                }
                break;

            case 10:
                if (esDigito(c)) sigEstado(10);
                else if (c == '.') sigEstado(11);
                else {
                    retroceso();
                    continua = false;
                }
                break;

            case 11:
                if (esDigito(c)) sigEstado(12);
                else {
                    aceptacion(TipoSimbolo::ERROR);
                }
                break;

            case 12:
                if (esDigito(c)) sigEstado(12);
                else {
                    retroceso();
                    continua = false;
                }
                break;

            case 4:
                if (c == '=') aceptacion(TipoSimbolo::OPIGUALDAD);
                else {
                    retroceso();
                    aceptacion(TipoSimbolo::ASIGNACION);
                }
                break;

            case 5:
                if (c == '=') aceptacion(TipoSimbolo::OPIGUALDAD);
                else {
                    retroceso();
                    aceptacion(TipoSimbolo::OPNOT);
                }
                break;

            case 6:
                if (c == '=') aceptacion(TipoSimbolo::OPRELAC);
                else {
                    retroceso();
                    aceptacion(TipoSimbolo::OPRELAC);
                }
                break;

            case 7:
                if (c == '&') aceptacion(TipoSimbolo::OPAND);
                else {
                    retroceso();
                    aceptacion(TipoSimbolo::ERROR);
                }
                break;

            case 8:
                if (c == '|') aceptacion(TipoSimbolo::OPOR);
                else {
                    retroceso();
                    aceptacion(TipoSimbolo::ERROR);
                }
                break;
        }
    }

    // Clasificación final
    if (estado == 1) {
        if (simbolo == "if") tipo = TipoSimbolo::IF;
        else if (simbolo == "while") tipo = TipoSimbolo::WHILE;
        else if (simbolo == "return") tipo = TipoSimbolo::RETURN;
        else if (simbolo == "else") tipo = TipoSimbolo::ELSE;
        else if (simbolo == "int" || simbolo == "float") tipo = TipoSimbolo::TIPO;
        else tipo = TipoSimbolo::IDENTIFICADOR;
    } else if (estado == 10) tipo = TipoSimbolo::ENTERO;
    else if (estado == 12) tipo = TipoSimbolo::REAL;
    else tipo = estado >= 0 ? estado : TipoSimbolo::ERROR;

    return tipo;
}
