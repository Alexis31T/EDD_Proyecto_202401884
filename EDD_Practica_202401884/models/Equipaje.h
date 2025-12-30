#ifndef EQUIPAJE_H
#define EQUIPAJE_H

#include <iostream>
#include <cstring>

class Equipaje {
public:
    char numero_pasaporte[50];
    int cantidad;

    Equipaje() {
        strcpy(numero_pasaporte, "");
        cantidad = 0;
    }

    Equipaje(const char* pass, int cant) {
        strcpy(numero_pasaporte, pass);
        cantidad = cant;
    }

    void mostrar() const {
        std::cout << "Equipaje de pasaporte: " << numero_pasaporte 
                  << " | Cantidad: " << cantidad << std::endl;
    }
};

#endif