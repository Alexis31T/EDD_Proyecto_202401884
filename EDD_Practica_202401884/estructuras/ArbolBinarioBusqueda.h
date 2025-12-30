#ifndef ARBOL_BINARIO_BUSQUEDA_H
#define ARBOL_BINARIO_BUSQUEDA_H

#include "NodoArbolBinario.h"
#include "../models/Piloto.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdio>

// ABB para organizar pilotos según horas de vuelo
// Funciona como un árbol donde izquierda = menos horas, derecha = más horas
class ArbolBinarioBusqueda {
private:
    NodoArbolBinario* raiz;
    
    // Insertar recursivamente comparando horas de vuelo para decidir si va a izquierda o derecha
    NodoArbolBinario* insertarRecursivo(NodoArbolBinario* nodo, const Piloto& piloto) {
        if (nodo == nullptr) {
            return new NodoArbolBinario(piloto);  // Encontramos el lugar, crear nodo aquí
        }
        
        if (piloto.horas_de_vuelo < nodo->piloto.horas_de_vuelo) {
            nodo->izquierdo = insertarRecursivo(nodo->izquierdo, piloto);  // Menos horas, ir a izquierda
        } else if (piloto.horas_de_vuelo > nodo->piloto.horas_de_vuelo) {
            nodo->derecho = insertarRecursivo(nodo->derecho, piloto);  // Más horas, ir a derecha
        } else {
            nodo->derecho = insertarRecursivo(nodo->derecho, piloto);  // Mismas horas, meter a la derecha
        }
        
        return nodo;
    }
    
    // Buscar por horas de vuelo navegando el árbol según el valor
    NodoArbolBinario* buscarRecursivo(NodoArbolBinario* nodo, int horas) {
        if (nodo == nullptr || nodo->piloto.horas_de_vuelo == horas) {
            return nodo;  // Lo encontramos o no existe
        }
        
        if (horas < nodo->piloto.horas_de_vuelo) {
            return buscarRecursivo(nodo->izquierdo, horas);  // Buscar en rama izquierda
        }
        
        return buscarRecursivo(nodo->derecho, horas);  // Buscar en rama derecha
    }
    
    // Encontrar el mínimo es ir siempre a la izquierda hasta el final
    NodoArbolBinario* encontrarMinimo(NodoArbolBinario* nodo) {
        while (nodo && nodo->izquierdo != nullptr) {
            nodo = nodo->izquierdo;
        }
        return nodo;
    }
    
    // Eliminar tiene 3 casos: sin hijos, un hijo, o dos hijos (este último requiere sucesor inorden)
    NodoArbolBinario* eliminarPorHoras(NodoArbolBinario* nodo, int horas_de_vuelo) {
        if (nodo == nullptr) {
            return nullptr;
        }
        
        // El ABB está ordenado por horas de vuelo
        if (horas_de_vuelo < nodo->piloto.horas_de_vuelo) {
            nodo->izquierdo = eliminarPorHoras(nodo->izquierdo, horas_de_vuelo);
        } else if (horas_de_vuelo > nodo->piloto.horas_de_vuelo) {
            nodo->derecho = eliminarPorHoras(nodo->derecho, horas_de_vuelo);
        } else {
            // Nodo encontrado, revisar cuántos hijos tiene
            if (nodo->izquierdo == nullptr) {
                NodoArbolBinario* temp = nodo->derecho;  // Solo hijo derecho
                delete nodo;
                return temp;
            } else if (nodo->derecho == nullptr) {
                NodoArbolBinario* temp = nodo->izquierdo;  // Solo hijo izquierdo
                delete nodo;
                return temp;
            }
            
            // Tiene dos hijos: copiar el sucesor inorden y eliminar ese sucesor
            NodoArbolBinario* temp = encontrarMinimo(nodo->derecho);
            nodo->piloto = temp->piloto;
            nodo->derecho = eliminarPorHoras(nodo->derecho, temp->piloto.horas_de_vuelo);
        }
        
        return nodo;
    }
    
    // Buscar piloto por ID en todo el árbol (sin usar estructura del ABB)
    Piloto* buscarPorID(NodoArbolBinario* nodo, const char* numero_id) {
        if (nodo == nullptr) {
            return nullptr;
        }
        
        if (strcmp(nodo->piloto.numero_de_id, numero_id) == 0) {
            return &nodo->piloto;
        }
        
        Piloto* resultado = buscarPorID(nodo->izquierdo, numero_id);
        if (resultado != nullptr) {
            return resultado;
        }
        
        return buscarPorID(nodo->derecho, numero_id);
    }
    
    // Preorden visita raíz primero, luego izquierda, luego derecha
    void preordenRecursivo(NodoArbolBinario* nodo) {
        if (nodo != nullptr) {
            std::cout << "Piloto: " << nodo->piloto.nombre 
                      << " | Horas: " << nodo->piloto.horas_de_vuelo << std::endl;
            preordenRecursivo(nodo->izquierdo);
            preordenRecursivo(nodo->derecho);
        }
    }
    
    // Inorden visita izquierda, raíz, derecha - sale todo ordenado de menor a mayor
    void inordenRecursivo(NodoArbolBinario* nodo) {
        if (nodo != nullptr) {
            inordenRecursivo(nodo->izquierdo);
            std::cout << "Piloto: " << nodo->piloto.nombre 
                      << " | Horas: " << nodo->piloto.horas_de_vuelo << std::endl;
            inordenRecursivo(nodo->derecho);
        }
    }
    
    // Postorden visita izquierda, derecha, luego raíz - útil para liberar memoria
    void postordenRecursivo(NodoArbolBinario* nodo) {
        if (nodo != nullptr) {
            postordenRecursivo(nodo->izquierdo);
            postordenRecursivo(nodo->derecho);
            std::cout << "Piloto: " << nodo->piloto.nombre 
                      << " | Horas: " << nodo->piloto.horas_de_vuelo << std::endl;
        }
    }
    
    // Genera el DOT numerando los nodos según el tipo de recorrido (1=PRE, 2=IN, 3=POST, 0=normal)
    void generarDOTConOrden(NodoArbolBinario* nodo, std::ofstream& archivo, int& contador, 
                            int& orden, int tipoRecorrido) {
        if (nodo == nullptr) return;
        
        int nodoActual = contador++;
        
        if (tipoRecorrido == 1) {  // Preorden: numerar raíz primero
            orden++;
            archivo << "    nodo" << nodoActual << " [label=\"#" << orden 
                    << "\\n" << nodo->piloto.nombre << "\\nHoras: " 
                    << nodo->piloto.horas_de_vuelo << "\", fillcolor=\"#90EE90\"];\n";
        }
        
        if (nodo->izquierdo != nullptr) {  // Procesar rama izquierda
            int hijoIzq = contador;
            generarDOTConOrden(nodo->izquierdo, archivo, contador, orden, tipoRecorrido);
            archivo << "    nodo" << nodoActual << " -> nodo" << hijoIzq << " [label=\"Izq\"];\n";
        }
        
        if (tipoRecorrido == 2) {  // Inorden: numerar raíz entre los hijos
            orden++;
            archivo << "    nodo" << nodoActual << " [label=\"#" << orden 
                    << "\\n" << nodo->piloto.nombre << "\\nHoras: " 
                    << nodo->piloto.horas_de_vuelo << "\", fillcolor=\"#87CEEB\"];\n";
        }
        
        if (nodo->derecho != nullptr) {  // Procesar rama derecha
            int hijoDer = contador;
            generarDOTConOrden(nodo->derecho, archivo, contador, orden, tipoRecorrido);
            archivo << "    nodo" << nodoActual << " -> nodo" << hijoDer << " [label=\"Der\"];\n";
        }
        
        if (tipoRecorrido == 3) {  // Postorden: numerar raíz al final
            orden++;
            archivo << "    nodo" << nodoActual << " [label=\"#" << orden 
                    << "\\n" << nodo->piloto.nombre << "\\nHoras: " 
                    << nodo->piloto.horas_de_vuelo << "\", fillcolor=\"#DDA0DD\"];\n";
        }
        
        if (tipoRecorrido == 0) {  // Modo normal sin numeración
            archivo << "    nodo" << nodoActual << " [label=\"" 
                    << nodo->piloto.nombre << "\\nHoras: " << nodo->piloto.horas_de_vuelo 
                    << "\\nID: " << nodo->piloto.numero_de_id << "\"];\n";
        }
    }
    
    /**
     * Destruir el árbol recursivamente
     */
    void destruirRecursivo(NodoArbolBinario* nodo) {
        if (nodo != nullptr) {
            destruirRecursivo(nodo->izquierdo);
            destruirRecursivo(nodo->derecho);
            delete nodo;
        }
    }
    
public:
    ArbolBinarioBusqueda() : raiz(nullptr) {}
    
    ~ArbolBinarioBusqueda() {
        destruirRecursivo(raiz);
    }
    
    /**
     * Insertar un piloto en el árbol
     */
    void insertar(const Piloto& piloto) {
        raiz = insertarRecursivo(raiz, piloto);
    }
    
    /**
     * Buscar un piloto por horas de vuelo
     */
    Piloto* buscar(int horas) {
        NodoArbolBinario* resultado = buscarRecursivo(raiz, horas);
        return resultado ? &(resultado->piloto) : nullptr;
    }
    
    /**
     * Eliminar un piloto por su número de ID
     */
    void eliminar(const char* numero_id) {
        // Primero buscar el piloto por ID para obtener sus horas de vuelo
        Piloto* piloto = buscarPorID(raiz, numero_id);
        if (piloto != nullptr) {
            // Eliminar usando horas de vuelo (clave del ABB)
            raiz = eliminarPorHoras(raiz, piloto->horas_de_vuelo);
        }
    }
    
    /**
     * Mostrar recorrido Pre-Orden
     */
    void mostrarPreorden() {
        std::cout << "\n=== Recorrido Pre-Orden ===" << std::endl;
        preordenRecursivo(raiz);
    }
    
    /**
     * Mostrar recorrido In-Orden
     */
    void mostrarInorden() {
        std::cout << "\n=== Recorrido In-Orden (Ordenado por Horas) ===" << std::endl;
        inordenRecursivo(raiz);
    }
    
    /**
     * Mostrar recorrido Post-Orden
     */
    void mostrarPostorden() {
        std::cout << "\n=== Recorrido Post-Orden ===" << std::endl;
        postordenRecursivo(raiz);
    }
    
    // Generar imagen del árbol con numeración en preorden
    void generarReportePreorden(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph ArbolBinarioPilotos {\n";
        archivo << "    node [shape=box, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n";
        archivo << "    rankdir=TB;\n\n";
        
        archivo << "    titulo [label=<<B>RECORRIDO PRE-ORDEN</B><BR/>"
                << "<FONT POINT-SIZE=\"10\">Raíz → Izquierda → Derecha</FONT>>, "
                << "shape=box, fillcolor=\"#E74C3C\", fontcolor=white, style=filled];\n\n";
        
        if (raiz != nullptr) {
            int contador = 0;
            int orden = 0;
            generarDOTConOrden(raiz, archivo, contador, orden, 1);
            archivo << "    titulo -> nodo0 [style=invis];\n";
        }
        
        archivo << "}\n";
        archivo.close();
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
    
    // Generar imagen del árbol con numeración en inorden (muestra el orden ascendente)
    void generarReporteInorden(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph ArbolBinarioPilotos {\n";
        archivo << "    node [shape=box, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n";
        archivo << "    rankdir=TB;\n\n";
        
        archivo << "    titulo [label=<<B>RECORRIDO IN-ORDEN</B><BR/>"
                << "<FONT POINT-SIZE=\"10\">Izquierda → Raíz → Derecha (Ordenado)</FONT>>, "
                << "shape=box, fillcolor=\"#3498DB\", fontcolor=white, style=filled];\n\n";
        
        if (raiz != nullptr) {
            int contador = 0;
            int orden = 0;
            generarDOTConOrden(raiz, archivo, contador, orden, 2);
            archivo << "    titulo -> nodo0 [style=invis];\n";
        }
        
        archivo << "}\n";
        archivo.close();
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
    
    // Generar imagen del árbol con numeración en postorden
    void generarReportePostorden(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph ArbolBinarioPilotos {\n";
        archivo << "    node [shape=box, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n";
        archivo << "    rankdir=TB;\n\n";
        
        archivo << "    titulo [label=<<B>RECORRIDO POST-ORDEN</B><BR/>"
                << "<FONT POINT-SIZE=\"10\">Izquierda → Derecha → Raíz</FONT>>, "
                << "shape=box, fillcolor=\"#9B59B6\", fontcolor=white, style=filled];\n\n";
        
        if (raiz != nullptr) {
            int contador = 0;
            int orden = 0;
            generarDOTConOrden(raiz, archivo, contador, orden, 3);
            archivo << "    titulo -> nodo0 [style=invis];\n";
        }
        
        archivo << "}\n";
        archivo.close();
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
    
    /**
     * Generar reporte Graphviz del árbol 
     */
    void generarReporteGraphviz(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph ArbolBinarioPilotos {\n";
        archivo << "    node [shape=box, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n";
        archivo << "    rankdir=TB;\n\n";
        
        archivo << "    titulo [label=<<B>ÁRBOL BINARIO DE BÚSQUEDA - PILOTOS</B><BR/>"
                << "<FONT POINT-SIZE=\"10\">Ordenado por Horas de Vuelo</FONT>>, "
                << "shape=box, fillcolor=\"#2C3E50\", fontcolor=white, style=filled];\n\n";
        
        if (raiz == nullptr) {
            archivo << "    vacio [label=\"Arbol Vacio\", fillcolor=lightgray];\n";
            archivo << "    titulo -> vacio [style=invis];\n";
        } else {
            int contador = 0;
            int orden = 0;
            generarDOTConOrden(raiz, archivo, contador, orden, 0);
            archivo << "    titulo -> nodo0 [style=invis];\n";
        }
        
        archivo << "}\n";
        archivo.close();
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
    
    /**
     * Verificar si el árbol está vacío
     */
    bool estaVacio() const {
        return raiz == nullptr;
    }
};

#endif
