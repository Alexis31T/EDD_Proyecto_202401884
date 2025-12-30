#ifndef NODO_HASH_H
#define NODO_HASH_H

#include "../models/Piloto.h"

// Nodo para encadenamiento en la tabla hash - maneja colisiones creando listas
class NodoHash {
public:
    Piloto piloto;          // Datos del piloto
    NodoHash* siguiente;    // Siguiente piloto con mismo hash (colisión)
    
    NodoHash(const Piloto& p) : piloto(p), siguiente(nullptr) {}  // Crear nodo sin enlace
};

#endif
