#ifndef PILOTO_H
#define PILOTO_H

#include <iostream>
#include <cstring>


class Piloto {
public:
    char nombre[100];
    char nacionalidad[50];
    char numero_de_id[50];      // Identificador único (ej: P12345678)
    char vuelo[20];             // Vuelo asignado
    int horas_de_vuelo;         // Usado como clave en ABB
    char tipo_de_licencia[10];  // PPL, CPL, ATPL

    Piloto() {
        strcpy(nombre, "");
        strcpy(nacionalidad, "");
        strcpy(numero_de_id, "");
        strcpy(vuelo, "");
        horas_de_vuelo = 0;
        strcpy(tipo_de_licencia, "");
    }

    Piloto(const char* nom, const char* nac, const char* id, const char* vuel, 
           int horas, const char* lic) {
        strcpy(nombre, nom);
        strcpy(nacionalidad, nac);
        strcpy(numero_de_id, id);
        strcpy(vuelo, vuel);
        horas_de_vuelo = horas;
        strcpy(tipo_de_licencia, lic);
    }

    void mostrar() const {
        std::cout << "Piloto: " << nombre << " | ID: " << numero_de_id
                  << " | Horas: " << horas_de_vuelo << " | Vuelo: " << vuelo
                  << " | Licencia: " << tipo_de_licencia << std::endl;
    }

    // Extrae la parte numérica del ID para la tabla hash
    int extraerNumeroID() const {
        int num = 0;
        for (int i = 0; numero_de_id[i] != '\0'; i++) {
            if (numero_de_id[i] >= '0' && numero_de_id[i] <= '9') {
                num = num * 10 + (numero_de_id[i] - '0');
            }
        }
        return num;
    }
};

#endif
