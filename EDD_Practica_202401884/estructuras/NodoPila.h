#ifndef NODO_PILA_H
#define NODO_PILA_H

#include "../models/Equipaje.h"

// Nodo simple de la pila - guarda un equipaje y apunta al siguiente (el que está debajo)
class NodoPila {
public:
    Equipaje equipaje;      // Datos del equipaje
    NodoPila* siguiente;    // Siguiente en la pila (nullptr en el fondo)

    NodoPila(const Equipaje& eq) : equipaje(eq), siguiente(nullptr) {}  // Crear nodo sin enlace
};

#endif