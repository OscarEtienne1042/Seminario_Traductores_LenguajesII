#ifndef _LEXICO
#define _LEXICO

#include <string>

enum TipoSimbolo {
    ERROR = -1,
    IDENTIFICADOR = 0,
    ENTERO = 1,
    REAL = 2,
    CADENA = 3,
    TIPO = 4,
    OPSUMA = 5,
    OPMUL = 6,
    OPRELAC = 7,
    OPOR = 8,
    OPAND = 9,
    OPNOT = 10,
    OPIGUALDAD = 11,
    PUNTOYCOMA = 12,
    COMA = 13,
    PARENIZQ = 14,
    PARENDER = 15,
    LLAVEIZQ = 16,
    LLAVEDER = 17,
    ASIGNACION = 18,
    IF = 19,
    WHILE = 20,
    RETURN = 21,
    ELSE = 22,
    PESOS = 23
};

class Lexico {
private:
    std::string fuente;
    int ind;
    bool continua;
    char c;
    int estado;

    char sigCaracter();
    void sigEstado(int estado);
    void aceptacion(int estado);
    bool esLetra(char c);
    bool esDigito(char c);
    bool esEspacio(char c);
    void retroceso();

public:
    std::string simbolo;
    int tipo;

    Lexico(std::string fuente);
    Lexico();

    void entrada(std::string fuente);
    std::string tipoAcad(int tipo);

    int sigSimbolo();
    bool terminado();
};

#endif
