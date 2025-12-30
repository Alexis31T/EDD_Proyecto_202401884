#ifndef ARBOL_B_H
#define ARBOL_B_H

#include "NodoArbolB.h"
#include <fstream>
#include <iostream>
#include <cstring>

class ArbolB {
private:
    NodoArbolB* raiz;

    int registroAEntero(const char* registro) const {
        int num = 0;
        for (int i = 0; registro[i]; i++)
            if (registro[i] >= '0' && registro[i] <= '9')
                num = num * 10 + (registro[i] - '0');
        return num;
    }

    void liberar(NodoArbolB* nodo) {
        if (!nodo) return;
        if (!nodo->esHoja)
            for (int i = 0; i <= nodo->n; i++)
                liberar(nodo->hijos[i]);
        delete nodo;
    }

    void dividirHijo(NodoArbolB* padre, int idx) {
        NodoArbolB* hijo = padre->hijos[idx];
        NodoArbolB* derecho = new NodoArbolB(hijo->esHoja);

        // Mediana real
        int mediana = 2;

        // El nodo derecho recibe claves 3
        derecho->n = 1;
        derecho->claves[0] = hijo->claves[3];
        derecho->aviones[0] = hijo->aviones[3];

        // Copiar hijos si no es hoja
        if (!hijo->esHoja) {
            derecho->hijos[0] = hijo->hijos[3];
            derecho->hijos[1] = hijo->hijos[4];
        }

        // El hijo izquierdo queda con 2 claves
        hijo->n = 2;

        // Mover hijos del padre
        for (int j = padre->n; j >= idx + 1; j--)
            padre->hijos[j + 1] = padre->hijos[j];

        padre->hijos[idx + 1] = derecho;

        // Mover claves del padre
        for (int j = padre->n - 1; j >= idx; j--) {
            padre->claves[j + 1] = padre->claves[j];
            padre->aviones[j + 1] = padre->aviones[j];
        }

        // Subir mediana
        padre->claves[idx] = hijo->claves[mediana];
        padre->aviones[idx] = hijo->aviones[mediana];
        padre->n++;
    }

    void insertarNoLleno(NodoArbolB* nodo, const Avion& avion) {
        int clave = registroAEntero(avion.registro);
        int i = nodo->n - 1;

        if (nodo->esHoja) {
            while (i >= 0 && clave < nodo->claves[i]) {
                nodo->claves[i + 1] = nodo->claves[i];
                nodo->aviones[i + 1] = nodo->aviones[i];
                i--;
            }
            nodo->claves[i + 1] = clave;
            nodo->aviones[i + 1] = avion;
            nodo->n++;
        } else {
            while (i >= 0 && clave < nodo->claves[i]) i--;
            i++;

            if (nodo->hijos[i]->n == NodoArbolB::MAX_CLAVES) {
                dividirHijo(nodo, i);
                if (clave > nodo->claves[i]) i++;
            }
            insertarNoLleno(nodo->hijos[i], avion);
        }
    }

    void generarDOT(NodoArbolB* nodo, std::ofstream& f, int& id) {
        if (!nodo) return;

        int actual = id++;
        f << "  n" << actual << " [label=\"";
        for (int i = 0; i < nodo->n; i++) {
            if (i > 0) f << "|";
            f << nodo->claves[i];
        }
        f << "\"];\n";

        if (!nodo->esHoja) {
            for (int i = 0; i <= nodo->n; i++) {
                int h = id;
                generarDOT(nodo->hijos[i], f, id);
                f << "  n" << actual << " -> n" << h << ";\n";
            }
        }
    }

public:
    ArbolB() : raiz(nullptr) {}

    ~ArbolB() {
        liberar(raiz);
    }

    void insertar(const Avion& avion) {
        if (!raiz) {
            raiz = new NodoArbolB(true);
            raiz->claves[0] = registroAEntero(avion.registro);
            raiz->aviones[0] = avion;
            raiz->n = 1;
            return;
        }

        if (raiz->n == NodoArbolB::MAX_CLAVES) {
            NodoArbolB* nueva = new NodoArbolB(false);
            nueva->hijos[0] = raiz;
            dividirHijo(nueva, 0);
            raiz = nueva;
        }

        insertarNoLleno(raiz, avion);
    }

    void generarReporteGraphviz(const char* archivo) {
        std::ofstream f(archivo);
        f << "digraph ArbolB {\n";
        f << "  node [shape=record];\n";
        f << "  label=\"Aviones Disponibles (Arbol B)\";\n";
        f << "  labelloc=t;\n\n";
        int id = 0;
        generarDOT(raiz, f, id);
        f << "}\n";
        f.close();
    }
};

#endif
