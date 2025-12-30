#ifndef NODO_LISTA_CIRCULAR_H
#define NODO_LISTA_CIRCULAR_H

#include "../models/Avion.h"

// Nodo de lista circular doble - apunta al siguiente y anterior, el último conecta con el primero
class NodoListaCircular {
public:
    Avion avion;                    // Datos del avión
    NodoListaCircular* siguiente;   // Siguiente nodo (el último apunta al primero)
    NodoListaCircular* anterior;    // Nodo anterior (el primero apunta al último)

    NodoListaCircular(const Avion& av) : avion(av), siguiente(nullptr), anterior(nullptr) {}  // Crear nodo sin enlaces
};

#endif