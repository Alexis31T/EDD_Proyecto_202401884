#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include "NodoHash.h"
#include "../models/Piloto.h"
#include <iostream>
#include <fstream>

class TablaHash {
private:
    static const int M = 19; // Tamaño fijo de la tabla (19 posiciones)
    NodoHash* tabla[M];      // Arreglo de punteros a listas encadenadas
    
    // Calcular posición en la tabla: ASCII de la letra + valores numéricos de los dígitos, luego módulo 19
    // Ejemplo: "X10000123" -> (88+1+0+0+0+0+1+2+3) % 19 = 95 % 19 = 0
    int funcionHash(const char* numero_id) {
        int suma = 0;
        for (int i = 0; numero_id[i] != '\0'; i++) {
            if (numero_id[i] >= '0' && numero_id[i] <= '9') {
                suma += (numero_id[i] - '0');  // Convertir char a su valor numérico 
            } else {
                suma += (int)numero_id[i];  // Letras u otros caracteres: usar ASCII
            }
        }
        return suma % M;  // Aplicar módulo para obtener posición (0-18)
    }
    
public:
    TablaHash() {
        for (int i = 0; i < M; i++) {
            tabla[i] = nullptr;  // Todas las posiciones vacías al inicio
        }
    }
    
    ~TablaHash() {
        // Recorrer cada posición de la tabla y liberar sus cadenas
        for (int i = 0; i < M; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* siguiente = actual->siguiente;
                delete actual;  // Liberar nodo
                actual = siguiente;
            }
        }
    }
    
    // Insertar un piloto calculando su posición hash
    void insertar(const Piloto& piloto) {
        int indice = funcionHash(piloto.numero_de_id);  // Calcular dónde va
        
        // Verificar que no exista ya (recorrer la cadena de esa posición)
        NodoHash* actual = tabla[indice];
        while (actual != nullptr) {
            if (strcmp(actual->piloto.numero_de_id, piloto.numero_de_id) == 0) {  // Ya existe
                std::cout << "Advertencia: Piloto con ID " << piloto.numero_de_id 
                          << " ya existe en la tabla." << std::endl;
                return;
            }
            actual = actual->siguiente;
        }
        
        // No existe, insertar al final de la cadena (respeta orden de llegada)
        NodoHash* nuevo = new NodoHash(piloto);
        
        if (tabla[indice] == nullptr) {
            // La cadena está vacía, el nuevo es el primero
            tabla[indice] = nuevo;
        } else {
            // Recorrer hasta el final de la cadena
            NodoHash* temp = tabla[indice];
            while (temp->siguiente != nullptr) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevo;  // Agregar al final
        }
        
        std::cout << "Piloto " << piloto.nombre << " insertado en posicion " 
                  << indice << std::endl;
    }
    
    // Buscar un piloto por ID calculando su hash y recorriendo su cadena
    Piloto* buscar(const char* numero_id) {
        int indice = funcionHash(numero_id);  // Calcular en qué posición debería estar
        NodoHash* actual = tabla[indice];
        
        // Recorrer la cadena de esa posición buscando el ID
        while (actual != nullptr) {
            if (strcmp(actual->piloto.numero_de_id, numero_id) == 0) {  // Lo encontramos
                return &(actual->piloto);
            }
            actual = actual->siguiente;
        }
        
        return nullptr;  // No está en la cadena
    }
    
    // Eliminar un piloto de la tabla hash
    bool eliminar(const char* numero_id) {
        int indice = funcionHash(numero_id);  // Calcular posición
        NodoHash* actual = tabla[indice];
        NodoHash* anterior = nullptr;
        
        // Buscar en la cadena manteniendo referencia al anterior
        while (actual != nullptr) {
            if (strcmp(actual->piloto.numero_de_id, numero_id) == 0) {  // Lo encontramos
                if (anterior == nullptr) {
                    tabla[indice] = actual->siguiente;  // Era el primero, mover inicio
                } else {
                    anterior->siguiente = actual->siguiente;  // Saltarlo de la cadena
                }
                delete actual;
                std::cout << "Piloto con ID " << numero_id << " eliminado." << std::endl;
                return true;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
        
        std::cout << "Piloto con ID " << numero_id << " no encontrado." << std::endl;
        return false;
    }
    
    // Imprimir toda la tabla mostrando cada posición y sus cadenas
    void mostrar() {
        std::cout << "\n=== Tabla Hash de Pilotos (M=" << M << ") ===" << std::endl;
        
        for (int i = 0; i < M; i++) {
            std::cout << "Posicion " << i << ": ";
            
            NodoHash* actual = tabla[i];
            if (actual == nullptr) {
                std::cout << "[vacia]";
            } else {
                while (actual != nullptr) {
                    std::cout << "[" << actual->piloto.nombre << " - " 
                              << actual->piloto.numero_de_id << "]";
                    if (actual->siguiente != nullptr) {
                        std::cout << " -> ";
                    }
                    actual = actual->siguiente;
                }
            }
            std::cout << std::endl;
        }
    }
    
    // Generar archivo DOT mostrando la tabla y sus cadenas de colisiones
    void generarReporteGraphviz(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph TablaHashPilotos {\n";
        archivo << "    rankdir=LR;\n";
        archivo << "    node [shape=record, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n\n";
        
        // Crear el nodo de la tabla con 19 posiciones (0-18)
        archivo << "    tabla [label=\"";
        for (int i = 0; i < M; i++) {
            archivo << "<f" << i << "> " << i;
            if (i < M - 1) archivo << " | ";
        }
        archivo << "\", fillcolor=lightgray, style=filled];\n\n";
        
        // Crear nodos para cada piloto y conectarlos desde la tabla
        for (int i = 0; i < M; i++) {
            NodoHash* actual = tabla[i];
            int contador = 0;  // Para numerar los nodos en cada cadena
            
            while (actual != nullptr) {
                std::string nodoNombre = "nodo_" + std::to_string(i) + "_" + std::to_string(contador);
                
                archivo << "    " << nodoNombre << " [label=\"{" 
                        << actual->piloto.nombre << "\\n" 
                        << actual->piloto.numero_de_id << "\\n"
                        << "Horas: " << actual->piloto.horas_de_vuelo 
                        << "}\", fillcolor=lightblue, style=filled];\n";
                
                if (contador == 0) {
                    // Primera conexión desde la posición de la tabla
                    archivo << "    tabla:f" << i << " -> " << nodoNombre << ";\n";
                } else {
                    // Conexión desde el nodo anterior en la cadena
                    std::string nodoAnterior = "nodo_" + std::to_string(i) + "_" + std::to_string(contador - 1);
                    archivo << "    " << nodoAnterior << " -> " << nodoNombre << ";\n";
                }
                
                actual = actual->siguiente;
                contador++;
            }
        }
        
        archivo << "}\n";
        archivo.close();
        
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
    
    // Calcular y mostrar estadísticas de uso de la tabla hash
    void mostrarEstadisticas() {
        int cadenasVacias = 0;
        int longitudMaxima = 0;
        int totalElementos = 0;
        
        // Recorrer toda la tabla calculando métricas
        for (int i = 0; i < M; i++) {
            int longitudCadena = 0;
            NodoHash* actual = tabla[i];
            
            // Contar elementos en esta cadena
            while (actual != nullptr) {
                longitudCadena++;
                totalElementos++;
                actual = actual->siguiente;
            }
            
            if (longitudCadena == 0) {
                cadenasVacias++;  // Posición sin elementos
            }
            
            if (longitudCadena > longitudMaxima) {
                longitudMaxima = longitudCadena;  // Actualizar máximo
            }
        }
        
        std::cout << "\n=== Estadisticas de la Tabla Hash ===" << std::endl;
        std::cout << "Tamano de la tabla: " << M << std::endl;
        std::cout << "Total de pilotos: " << totalElementos << std::endl;
        std::cout << "Posiciones vacias: " << cadenasVacias << std::endl;
        std::cout << "Longitud maxima de cadena: " << longitudMaxima << std::endl;
        std::cout << "Factor de carga: " << (float)totalElementos / M << std::endl;  // Qué tan llena está
    }
};

#endif
