#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <fstream>
#include <iostream>
#include <cstring>
#include "../models/Avion.h"
#include "../models/Pasajero.h"
#include "../estructuras/ListaCircularDoble.h"
#include "../estructuras/Cola.h"

class JsonParser {
private:
    static void limpiarCadena(char* str) {
        int len = strlen(str);
        int inicio = 0, fin = len - 1;
        
        while (inicio < len && (str[inicio] == ' ' || str[inicio] == '"' || str[inicio] == '\t' || str[inicio] == '\n' || str[inicio] == '\r')) {
            inicio++;
        }
        
        while (fin >= 0 && (str[fin] == ' ' || str[fin] == '"' || str[fin] == '\t' || str[fin] == '\n' || str[fin] == '\r' || str[fin] == ',')) {
            fin--;
        }
        
        int j = 0;
        for (int i = inicio; i <= fin; i++) {
            str[j++] = str[i];
        }
        str[j] = '\0';
    }

    static void extraerValor(char* linea, const char* clave, char* resultado) {
        char* pos = strstr(linea, clave);
        if (pos != nullptr) {
            pos += strlen(clave);
            while (*pos == ' ' || *pos == ':' || *pos == '"') pos++;
            
            int i = 0;
            while (*pos != '"' && *pos != ',' && *pos != '\0' && *pos != '\n' && *pos != '\r') {
                resultado[i++] = *pos++;
            }
            resultado[i] = '\0';
            limpiarCadena(resultado);
        }
    }

    static int extraerEntero(char* linea, const char* clave) {
        char valor[100];
        extraerValor(linea, clave, valor);
        return atoi(valor);
    }

public:
    static void cargarAviones(const char* filename, ListaCircularDoble& disponibles, ListaCircularDoble& mantenimiento) {
        std::ifstream archivo(filename);
        if (!archivo.is_open()) {
            std::cout << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }

        char linea[512];
        Avion avionTemp;
        bool leyendoAvion = false;
        int campos = 0;

        while (archivo.getline(linea, 512)) {
            if (strstr(linea, "{") != nullptr) {
                leyendoAvion = true;
                campos = 0;
                avionTemp = Avion();
            }

            if (leyendoAvion) {
                // Lee el vuelo
                if (strstr(linea, "\"vuelo\"") != nullptr) {
                    extraerValor(linea, "vuelo", avionTemp.vuelo);
                    campos++;
                }
                // Lee el numero_de_registro que viene en el JSON
                else if (strstr(linea, "numero_de_registro") != nullptr) {
                    extraerValor(linea, "numero_de_registro", avionTemp.registro);
                    campos++;
                }
                // Lee el modelo del avion
                else if (strstr(linea, "modelo") != nullptr) {
                    extraerValor(linea, "modelo", avionTemp.modelo);
                    campos++;
                }
                // Lee la capacidad de pasajeros
                else if (strstr(linea, "capacidad") != nullptr) {
                    avionTemp.capacidad = extraerEntero(linea, "capacidad");
                    campos++;
                }
                // Lee la aerolinea
                else if (strstr(linea, "aerolinea") != nullptr) {
                    extraerValor(linea, "aerolinea", avionTemp.aerolinea);
                    campos++;
                }
                // Lee la ciudad destino
                else if (strstr(linea, "ciudad_destino") != nullptr) {
                    extraerValor(linea, "ciudad_destino", avionTemp.ciudad_destino);
                    campos++;
                }
                // Lee el estado (Disponible o Mantenimiento)
                else if (strstr(linea, "\"estado\"") != nullptr) {
                    extraerValor(linea, "estado", avionTemp.estado);
                    campos++;
                }

                // Cuando termina el objeto con } y tiene los 7 campos necesarios
                if (strstr(linea, "}") != nullptr && campos >= 7) {
                    // Separa por estado: Disponible va a una lista, Mantenimiento a otra
                    if (strcmp(avionTemp.estado, "Disponible") == 0) {
                        disponibles.insertar(avionTemp);
                        std::cout << "Avion " << avionTemp.registro << " agregado a Disponibles" << std::endl;
                    } else {
                        mantenimiento.insertar(avionTemp);
                        std::cout << "Avion " << avionTemp.registro << " agregado a Mantenimiento" << std::endl;
                    }
                    leyendoAvion = false;
                }
            }
        }

        archivo.close();
        std::cout << "Aviones cargados exitosamente." << std::endl;
    }

    static void cargarPasajeros(const char* filename, Cola& cola) {
        std::ifstream archivo(filename);
        if (!archivo.is_open()) {
            std::cout << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }

        char linea[512];
        Pasajero pasajeroTemp;
        bool leyendoPasajero = false;
        int campos = 0;

        while (archivo.getline(linea, 512)) {
            if (strstr(linea, "{") != nullptr) {
                leyendoPasajero = true;
                campos = 0;
                pasajeroTemp = Pasajero();
            }

            if (leyendoPasajero) {
                if (strstr(linea, "nombre") != nullptr) {
                    extraerValor(linea, "nombre", pasajeroTemp.nombre);
                    campos++;
                }
                else if (strstr(linea, "nacionalidad") != nullptr) {
                    extraerValor(linea, "nacionalidad", pasajeroTemp.nacionalidad);
                    campos++;
                }
                else if (strstr(linea, "numero_de_pasaporte") != nullptr) {
                    extraerValor(linea, "numero_de_pasaporte", pasajeroTemp.numero_pasaporte);
                    campos++;
                }
                else if (strstr(linea, "vuelo") != nullptr) {
                    extraerValor(linea, "vuelo", pasajeroTemp.vuelo);
                    campos++;
                }
                else if (strstr(linea, "asiento") != nullptr) {
                    pasajeroTemp.asiento = extraerEntero(linea, "asiento");
                    campos++;
                }
                else if (strstr(linea, "destino") != nullptr) {
                    extraerValor(linea, "destino", pasajeroTemp.destino);
                    campos++;
                }
                else if (strstr(linea, "origen") != nullptr) {
                    extraerValor(linea, "origen", pasajeroTemp.origen);
                    campos++;
                }
                else if (strstr(linea, "equipaje_facturado") != nullptr) {
                    pasajeroTemp.equipaje_facturado = extraerEntero(linea, "equipaje_facturado");
                    campos++;
                }

                if (strstr(linea, "}") != nullptr && campos >= 7) {
                    cola.enqueue(pasajeroTemp);
                    leyendoPasajero = false;
                }
            }
        }

        archivo.close();
        std::cout << "Pasajeros cargados exitosamente." << std::endl;
    }
};

#endif