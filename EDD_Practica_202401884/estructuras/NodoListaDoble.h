#ifndef NODO_LISTA_DOBLE_H
#define NODO_LISTA_DOBLE_H

#include "../models/Pasajero.h"

// Nodo de lista doblemente enlazada - puede navegar hacia adelante y atrás
class NodoListaDoble {
public:
    Pasajero pasajero;          // Datos del pasajero
    NodoListaDoble* siguiente;  // Siguiente nodo en la lista (nullptr al final)
    NodoListaDoble* anterior;   // Nodo anterior en la lista (nullptr al inicio)

    NodoListaDoble(const Pasajero& pas) : pasajero(pas), siguiente(nullptr), anterior(nullptr) {}  // Crear nodo sin enlaces
};

#endif