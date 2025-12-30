#ifndef AVION_H
#define AVION_H

#include <iostream>
#include <cstring>

class Avion {
public:
    char vuelo[20];
    char registro[20];
    char modelo[50];
    char fabricante[50];
    int capacidad;
    int peso_max_despegue;
    char aerolinea[50];
    char ciudad_destino[100];
    char estado[20]; // "Disponible" o "Mantenimiento"

    Avion() {
        strcpy(vuelo, "");
        strcpy(registro, "");
        strcpy(modelo, "");
        strcpy(fabricante, "");
        capacidad = 0;
        peso_max_despegue = 0;
        strcpy(aerolinea, "");
        strcpy(ciudad_destino, "");
        strcpy(estado, "Disponible");
    }

    Avion(const char* reg, const char* mod, const char* fab, int cap, int peso, const char* aero, const char* est) {
        strcpy(vuelo, "");
        strcpy(registro, reg);
        strcpy(modelo, mod);
        strcpy(fabricante, fab);
        capacidad = cap;
        peso_max_despegue = peso;
        strcpy(aerolinea, aero);
        strcpy(ciudad_destino, "");
        strcpy(estado, est);
    }

    void mostrar() const {
        std::cout << "Vuelo: " << vuelo << " | Registro: " << registro << " | Modelo: " << modelo 
                  << " | Capacidad: " << capacidad << " | Aerolinea: " << aerolinea
                  << " | Ciudad Destino: " << ciudad_destino << " | Estado: " << estado << std::endl;
    }
};

#endif