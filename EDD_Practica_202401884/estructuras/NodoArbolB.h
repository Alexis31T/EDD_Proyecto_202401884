#ifndef NODO_ARBOL_B_H
#define NODO_ARBOL_B_H

#include "../models/Avion.h"

// Nodo del Árbol B de orden 5 - cada nodo puede tener hasta 4 claves y 5 hijos
class NodoArbolB {
public:
    static const int ORDEN = 5;                 // Orden del árbol
    static const int MAX_CLAVES = ORDEN - 1;    // Máximo 4 claves por nodo
    static const int MIN_CLAVES = ORDEN / 2;    // Mínimo 2 claves (excepto raíz puede tener 1)
    
    int n;                              // Cuántas claves tiene actualmente este nodo
    int claves[MAX_CLAVES];            // Los registros convertidos a entero (N12345 -> 12345)
    Avion aviones[MAX_CLAVES];         // Los aviones completos asociados a cada clave
    NodoArbolB* hijos[ORDEN];          // Apuntadores a los 5 hijos posibles
    bool esHoja;                        // true = no tiene hijos, false = tiene hijos
    
    NodoArbolB(bool hoja = true) {
        n = 0;         // Empieza sin claves
        esHoja = hoja; // Por defecto es hoja hasta que le agreguen hijos
        for (int i = 0; i < ORDEN; i++) {
            hijos[i] = nullptr;  // Todos los hijos en null al inicio
        }
    }
    
    ~NodoArbolB() {
        // La liberación de hijos la maneja el destructor del árbol completo
    }
};

#endif
