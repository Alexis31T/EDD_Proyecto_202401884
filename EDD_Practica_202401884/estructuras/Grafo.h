#ifndef GRAFO_H
#define GRAFO_H

#include "../models/Ruta.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <limits.h>

// Nodo para la lista de ciudades adyacentes con su distancia
class NodoAdyacente {
public:
    char ciudad[100];
    int distancia;
    NodoAdyacente* siguiente;
    
    NodoAdyacente(const char* c, int d) : distancia(d), siguiente(nullptr) {
        strcpy(ciudad, c);
    }
};

// Cada ciudad es un nodo con su lista de conexiones
class NodoGrafo {
public:
    char ciudad[100];
    NodoAdyacente* listaAdyacencia;
    NodoGrafo* siguiente;
    
    NodoGrafo(const char* c) : listaAdyacencia(nullptr), siguiente(nullptr) {
        strcpy(ciudad, c);
    }
    
    ~NodoGrafo() {
        NodoAdyacente* actual = listaAdyacencia;
        while (actual != nullptr) {
            NodoAdyacente* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
};

// Grafo dirigido con listas de adyacencia - implementa Dijkstra para rutas más cortas
class Grafo {
private:
    NodoGrafo* vertices;
    
    // Buscar una ciudad en la lista de vértices
    NodoGrafo* buscarVertice(const char* ciudad) {
        NodoGrafo* actual = vertices;
        while (actual != nullptr) {
            if (strcmp(actual->ciudad, ciudad) == 0) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }
    
    // Crear un vértice nuevo si no existe ya
    void agregarVertice(const char* ciudad) {
        if (buscarVertice(ciudad) == nullptr) {
            NodoGrafo* nuevo = new NodoGrafo(ciudad);
            nuevo->siguiente = vertices;
            vertices = nuevo;
        }
    }
    
    // Contar cuántas ciudades hay en el grafo
    int contarVertices() {
        int contador = 0;
        NodoGrafo* actual = vertices;
        while (actual != nullptr) {
            contador++;
            actual = actual->siguiente;
        }
        return contador;
    }
    
    // Encontrar la posición de una ciudad en el arreglo
    int encontrarIndice(char ciudades[][100], int n, const char* ciudad) {
        for (int i = 0; i < n; i++) {
            if (strcmp(ciudades[i], ciudad) == 0) {
                return i;
            }
        }
        return -1;
    }
    
    // Para Dijkstra: encontrar el nodo no visitado con menor distancia
    int distanciaMinima(int dist[], bool visitado[], int n) {
        int min = INT_MAX;
        int minIndice = -1;
        
        for (int v = 0; v < n; v++) {
            if (!visitado[v] && dist[v] < min) {
                min = dist[v];
                minIndice = v;
            }
        }
        
        return minIndice;
    }
    
    // Reconstruir y mostrar la ruta siguiendo los padres desde destino hasta origen
    void imprimirRuta(int padre[], int j, char ciudades[][100]) {
        if (padre[j] == -1) {  // Llegamos al origen
            std::cout << ciudades[j];
            return;
        }
        
        imprimirRuta(padre, padre[j], ciudades);  // Primero imprimir el camino hasta aquí
        std::cout << " -> " << ciudades[j];
    }
    
public:
    Grafo() : vertices(nullptr) {}
    
    ~Grafo() {
        while (vertices != nullptr) {
            NodoGrafo* temp = vertices;
            vertices = vertices->siguiente;
            delete temp;
        }
    }
    
    // Agregar una conexión dirigida entre dos ciudades
    void agregarArista(const char* origen, const char* destino, int distancia) {
        // Asegurar que ambos vértices existen
        agregarVertice(origen);
        agregarVertice(destino);
        
        NodoGrafo* verticeOrigen = buscarVertice(origen);
        
        // Meter el destino en la lista de adyacencia del origen
        NodoAdyacente* nuevo = new NodoAdyacente(destino, distancia);
        nuevo->siguiente = verticeOrigen->listaAdyacencia;
        verticeOrigen->listaAdyacencia = nuevo;
        
        std::cout << "Ruta agregada: " << origen << " -> " << destino 
                  << " (" << distancia << " km)" << std::endl;
    }
    
    // Mostrar todas las conexiones del grafo
    void mostrar() {
        std::cout << "\n=== Grafo de Rutas ===" << std::endl;
        
        NodoGrafo* vertice = vertices;
        while (vertice != nullptr) {
            std::cout << vertice->ciudad << " -> ";
            
            NodoAdyacente* adyacente = vertice->listaAdyacencia;
            if (adyacente == nullptr) {
                std::cout << "[sin conexiones]";
            } else {
                while (adyacente != nullptr) {
                    std::cout << "[" << adyacente->ciudad << ":" << adyacente->distancia << "km]";
                    if (adyacente->siguiente != nullptr) {
                        std::cout << ", ";
                    }
                    adyacente = adyacente->siguiente;
                }
            }
            std::cout << std::endl;
            vertice = vertice->siguiente;
        }
    }
    
    // Dijkstra para calcular la ruta más corta entre dos ciudades
    void dijkstra(const char* origen, const char* destino) {
        // Verificar que ambas ciudades existen
        if (buscarVertice(origen) == nullptr) {
            std::cout << "Error: Ciudad origen '" << origen << "' no existe en el grafo." << std::endl;
            return;
        }
        
        if (buscarVertice(destino) == nullptr) {
            std::cout << "Error: Ciudad destino '" << destino << "' no existe en el grafo." << std::endl;
            return;
        }
        
        // Preparar arreglos para el algoritmo
        int n = contarVertices();
        char ciudades[100][100];
        int dist[100];
        bool visitado[100];
        int padre[100];  // Para reconstruir el camino
        
        // Inicializar todo en infinito y no visitado
        NodoGrafo* vertice = vertices;
        int indice = 0;
        while (vertice != nullptr) {
            strcpy(ciudades[indice], vertice->ciudad);
            dist[indice] = INT_MAX;
            visitado[indice] = false;
            padre[indice] = -1;
            indice++;
            vertice = vertice->siguiente;
        }
        
        // Encontrar índice de origen
        int indiceOrigen = encontrarIndice(ciudades, n, origen);
        int indiceDestino = encontrarIndice(ciudades, n, destino);
        
        dist[indiceOrigen] = 0;  // La distancia del origen a sí mismo es 0
        
        // Bucle principal de Dijkstra
        for (int count = 0; count < n - 1; count++) {
            int u = distanciaMinima(dist, visitado, n);  // Elegir el más cercano
            
            if (u == -1) break;
            
            visitado[u] = true;  // Marcar como procesado
            
            NodoGrafo* verticeU = buscarVertice(ciudades[u]);
            NodoAdyacente* adyacente = verticeU->listaAdyacencia;
            
            // Revisar todas las ciudades conectadas y actualizar distancias si encontramos un camino más corto
            while (adyacente != nullptr) {
                int v = encontrarIndice(ciudades, n, adyacente->ciudad);
                
                if (!visitado[v] && dist[u] != INT_MAX && 
                    dist[u] + adyacente->distancia < dist[v]) {
                    dist[v] = dist[u] + adyacente->distancia;
                    padre[v] = u;
                }
                
                adyacente = adyacente->siguiente;
            }
        }
        
        // Mostrar resultado
        std::cout << "\n=== Ruta Mas Corta (Dijkstra) ===" << std::endl;
        std::cout << "Origen: " << origen << std::endl;
        std::cout << "Destino: " << destino << std::endl;
        
        if (dist[indiceDestino] == INT_MAX) {
            std::cout << "No existe ruta entre " << origen << " y " << destino << std::endl;
        } else {
            std::cout << "Distancia Total: " << dist[indiceDestino] << " km" << std::endl;
            std::cout << "Ruta: ";
            imprimirRuta(padre, indiceDestino, ciudades);
            std::cout << std::endl;
        }
    }
    
    // Crear archivo DOT para visualizar el grafo de rutas
    void generarReporteGraphviz(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo crear el archivo " << nombreArchivo << std::endl;
            return;
        }
        
        archivo << "digraph GrafoRutas {\n";
        archivo << "    rankdir=LR;\n";
        archivo << "    node [shape=circle, style=filled, fillcolor=lightgreen, fontname=\"Arial\"];\n";
        archivo << "    edge [fontname=\"Arial\"];\n\n";
        
        // Recorrer todos los vértices y sus conexiones
        NodoGrafo* vertice = vertices;
        while (vertice != nullptr) {
            archivo << "    \"" << vertice->ciudad << "\";\n";
            
            NodoAdyacente* adyacente = vertice->listaAdyacencia;
            while (adyacente != nullptr) {
                archivo << "    \"" << vertice->ciudad << "\" -> \"" 
                        << adyacente->ciudad << "\" [label=\"" 
                        << adyacente->distancia << " km\"];\n";
                adyacente = adyacente->siguiente;
            }
            
            vertice = vertice->siguiente;
        }
        
        archivo << "}\n";
        archivo.close();
        
        std::cout << "Reporte Graphviz generado: " << nombreArchivo << std::endl;
    }
};

#endif
