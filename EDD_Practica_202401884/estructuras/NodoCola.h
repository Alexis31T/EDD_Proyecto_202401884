#ifndef NODO_COLA_H
#define NODO_COLA_H

#include "../models/Pasajero.h"

// Nodo simple de la cola - guarda un pasajero y apunta al siguiente en la fila
class NodoCola {
public:
    Pasajero pasajero;      // Datos del pasajero
    NodoCola* siguiente;    // Siguiente en la cola (nullptr si es el último)

    NodoCola(const Pasajero& pas) : pasajero(pas), siguiente(nullptr) {}  // Crear nodo sin enlace
};

#endif
