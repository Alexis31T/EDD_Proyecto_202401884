#ifndef PASAJERO_H
#define PASAJERO_H

#include <iostream>
#include <cstring>

class Pasajero {
public:
    char nombre[100];
    char nacionalidad[50];
    char numero_pasaporte[50];
    char vuelo[20];
    int asiento;
    char destino[100];
    char origen[100];
    int equipaje_facturado;

    Pasajero() {
        strcpy(nombre, "");
        strcpy(nacionalidad, "");
        strcpy(numero_pasaporte, "");
        strcpy(vuelo, "");
        asiento = 0;
        strcpy(destino, "");
        strcpy(origen, "");
        equipaje_facturado = 0;
    }

    Pasajero(const char* nom, const char* nac, const char* pass, const char* vuel, 
             int asi, const char* dest, const char* orig, int equip) {
        strcpy(nombre, nom);
        strcpy(nacionalidad, nac);
        strcpy(numero_pasaporte, pass);
        strcpy(vuelo, vuel);
        asiento = asi;
        strcpy(destino, dest);
        strcpy(origen, orig);
        equipaje_facturado = equip;
    }

    void mostrar() const {
        std::cout << "Pasajero: " << nombre << " | Pasaporte: " << numero_pasaporte
                  << " | Vuelo: " << vuelo << " | Asiento: " << asiento
                  << " | Destino: " << destino << " | Equipaje: " << equipaje_facturado << std::endl;
    }
};

#endif
