#ifndef NODO_ARBOL_BINARIO_H
#define NODO_ARBOL_BINARIO_H

#include "../models/Piloto.h"

// Nodo del ABB - guarda un piloto y apunta a izquierda (menos horas) y derecha (más horas)
class NodoArbolBinario {
public:
    Piloto piloto;                  // Datos del piloto (incluye horas_de_vuelo para ordenar)
    NodoArbolBinario* izquierdo;    // Hijo izquierdo: pilotos con menos horas
    NodoArbolBinario* derecho;      // Hijo derecho: pilotos con más horas
    
    NodoArbolBinario(const Piloto& p) : piloto(p), izquierdo(nullptr), derecho(nullptr) {}  // Crear nodo hoja
};

#endif
