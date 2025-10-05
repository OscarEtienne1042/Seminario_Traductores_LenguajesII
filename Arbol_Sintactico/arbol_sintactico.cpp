#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

// ======== Estructura de Nodo del Árbol =========
struct Nodo {
    string valor;
    vector<Nodo*> hijos;
    Nodo(string v) : valor(v) {}
};

// ======== Estructura de Token =========
struct Token {
    string tipo;
    string valor;
};

// ======== Variables globales =========
vector<Token> tokens;
string codigo;
int pos = 0;

// ======== Funciones auxiliares =========
char peek() { return pos < (int)codigo.size() ? codigo[pos] : '\0'; }
char get() { return pos < (int)codigo.size() ? codigo[pos++] : '\0'; }

bool esLetra(char c) { return isalpha(c) || c == '_'; }
bool esNumero(char c) { return isdigit(c); }

void agregarToken(string tipo, string valor) {
    tokens.push_back({tipo, valor});
}

// ======== Tokenizador — analiza cada símbolo =========
void tokenizar() {
    while (peek() != '\0') {
        char c = get();
        if (isspace(c)) continue;

        if (esLetra(c)) {
            string palabra(1, c);
            while (esLetra(peek()) || esNumero(peek())) palabra += get();
            if (palabra == "int" || palabra == "float" || palabra == "return")
                agregarToken("PALABRA_RESERVADA", palabra);
            else
                agregarToken("IDENTIFICADOR", palabra);
        }
        else if (esNumero(c)) {
            string numero(1, c);
            while (esNumero(peek())) numero += get();
            agregarToken("NUMERO", numero);
        }
        else {
            // Cada símbolo es tratado como un token independiente
            string simbolo(1, c);
            switch (c) {
                case '(': agregarToken("PAREN_IZQ", simbolo); break;
                case ')': agregarToken("PAREN_DER", simbolo); break;
                case '{': agregarToken("LLAVE_IZQ", simbolo); break;
                case '}': agregarToken("LLAVE_DER", simbolo); break;
                case ';': agregarToken("PUNTOYCOMA", simbolo); break;
                case ',': agregarToken("COMA", simbolo); break;
                case '+': agregarToken("OP_SUMA", simbolo); break;
                case '-': agregarToken("OP_RESTA", simbolo); break;
                case '=': agregarToken("OP_ASIGN", simbolo); break;
                default: agregarToken("SIMBOLO", simbolo); break;
            }
        }
    }
}

// ======== Parser sencillo para estructurar el árbol =========
int i = 0;
Token actual() { return i < (int)tokens.size() ? tokens[i] : Token{"EOF", ""}; }
void avanzar() { if (i < (int)tokens.size()) i++; }

Nodo* parseBloque();
Nodo* parseFuncion();
Nodo* parsePrograma();

Nodo* parseSentencia() {
    Nodo* nodo = new Nodo("Sentencia");
    while (actual().tipo != "PUNTOYCOMA" && actual().tipo != "EOF" && actual().tipo != "LLAVE_DER") {
        nodo->hijos.push_back(new Nodo(actual().valor));
        avanzar();
    }
    if (actual().tipo == "PUNTOYCOMA") {
        nodo->hijos.push_back(new Nodo(";"));
        avanzar();
    }
    return nodo;
}

Nodo* parseBloque() {
    Nodo* nodo = new Nodo("Bloque");
    if (actual().tipo == "LLAVE_IZQ") {
        nodo->hijos.push_back(new Nodo("{"));
        avanzar();
    }
    while (actual().tipo != "LLAVE_DER" && actual().tipo != "EOF") {
        nodo->hijos.push_back(parseSentencia());
    }
    if (actual().tipo == "LLAVE_DER") {
        nodo->hijos.push_back(new Nodo("}"));
        avanzar();
    }
    return nodo;
}

Nodo* parseFuncion() {
    Nodo* nodo = new Nodo("Funcion");
    // tipo de retorno
    nodo->hijos.push_back(new Nodo(actual().valor)); avanzar();
    // nombre
    nodo->hijos.push_back(new Nodo(actual().valor)); avanzar();
    // parámetros
    Nodo* parametros = new Nodo("Parametros");
    if (actual().tipo == "PAREN_IZQ") {
        parametros->hijos.push_back(new Nodo("("));
        avanzar();
        while (actual().tipo != "PAREN_DER" && actual().tipo != "EOF") {
            parametros->hijos.push_back(new Nodo(actual().valor));
            avanzar();
        }
        if (actual().tipo == "PAREN_DER") {
            parametros->hijos.push_back(new Nodo(")"));
            avanzar();
        }
    }
    nodo->hijos.push_back(parametros);
    nodo->hijos.push_back(parseBloque());
    return nodo;
}

Nodo* parsePrograma() {
    Nodo* nodo = new Nodo("Programa");
    while (actual().tipo != "EOF") {
        if (actual().valor == "int" || actual().valor == "float") {
            string tipo = actual().valor;
            avanzar();
            string nombre = actual().valor;
            avanzar();
            if (actual().tipo == "PAREN_IZQ") {
                i -= 2;
                nodo->hijos.push_back(parseFuncion());
            } else {
                Nodo* decl = new Nodo("Declaracion");
                decl->hijos.push_back(new Nodo(tipo));
                decl->hijos.push_back(new Nodo(nombre));
                if (actual().tipo == "PUNTOYCOMA") {
                    decl->hijos.push_back(new Nodo(";"));
                    avanzar();
                }
                nodo->hijos.push_back(decl);
            }
        } else {
            avanzar();
        }
    }
    return nodo;
}

// ======== Impresión del Árbol Sintáctico =========
void imprimirArbol(Nodo* nodo, int nivel = 0) {
    for (int j = 0; j < nivel; j++) cout << "   ";
    cout << "- " << nodo->valor << endl;
    for (auto h : nodo->hijos)
        imprimirArbol(h, nivel + 1);
}

int main() {
    codigo =
        "int multiplicar(int x, int y){\n"
        "int resultado;\n"
        "resultado = x * y\n"
        "return resultado;\n"
        "}\n"
        "int main(){\n"
        "int a;\n"
        "int b;\n"
        "int c;\n"
        "a = 5;\n"
        "b = 10;\n"
        "c = multiplicar(a,b);\n"
        "}";

        cout << "--- Codigo analizado --\n";
        cout << codigo <<endl;

    tokenizar();
    Nodo* arbol = parsePrograma();

    cout << "\n\n\n--- Arbol Sintactico ---\n";
    imprimirArbol(arbol);

    return 0;
}
