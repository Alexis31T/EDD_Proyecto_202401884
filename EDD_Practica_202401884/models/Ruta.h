#ifndef RUTA_H
#define RUTA_H

#include <cstring>
#include <iostream>

/**
 * Modelo de Ruta para el Grafo Dirigido
 * Representa una conexión entre dos ciudades con una distancia
 */
class Ruta {
public:
    char origen[100];
    char destino[100];
    int distancia;
    
    Ruta() {
        strcpy(origen, "");
        strcpy(destino, "");
        distancia = 0;
    }
    
    Ruta(const char* orig, const char* dest, int dist) {
        strcpy(origen, orig);
        strcpy(destino, dest);
        distancia = dist;
    }
    
    void mostrar() const {
        std::cout << origen << " -> " << destino << " (Distancia: " << distancia << " km)" << std::endl;
    }
};

#endif
