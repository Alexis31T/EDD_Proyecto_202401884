#ifndef LISTA_CIRCULAR_DOBLE_H
#define LISTA_CIRCULAR_DOBLE_H

#include "NodoListaCircular.h"
#include <fstream>

class ListaCircularDoble {
private:
    NodoListaCircular* cabeza;  // Apuntador al primer nodo
    int tamanio;                 // Contador de elementos

public:
    ListaCircularDoble() : cabeza(nullptr), tamanio(0) {}

    /**
     * Libera toda la memoria de los nodos
     * Debe romper el ciclo antes de eliminar para evitar bucle infinito
     */
    ~ListaCircularDoble() {
        if (cabeza == nullptr) return;
        
        NodoListaCircular* actual = cabeza;
        do {
            NodoListaCircular* temp = actual;
            actual = actual->siguiente;
            delete temp;  // Liberar memoria de cada nodo
        } while (actual != cabeza);
    }

    /**
     * Inserta un nuevo avión al final de la lista circular
     */
    void insertar(const Avion& avion) {
        NodoListaCircular* nuevo = new NodoListaCircular(avion);
        
        // Caso 1: Lista vacía
        if (cabeza == nullptr) {
            cabeza = nuevo;
            nuevo->siguiente = nuevo;  // Apunta a sí mismo
            nuevo->anterior = nuevo;   // Apunta a sí mismo
        } 
        // Caso 2: Lista con elementos
        else {
            NodoListaCircular* ultimo = cabeza->anterior;  // Último nodo
            
            // Insertar al final y mantener circularidad
            nuevo->siguiente = cabeza;
            nuevo->anterior = ultimo;
            ultimo->siguiente = nuevo;
            cabeza->anterior = nuevo;
        }
        tamanio++;
    }

    /**
     * Elimina un avión de la lista por su número de registro
     * Retorna true si se eliminó, false si no se encontró
     */
    bool eliminar(const char* registro) {
        if (cabeza == nullptr) return false;

        NodoListaCircular* actual = cabeza;
        do {
            // Comparar registro del avión
            if (strcmp(actual->avion.registro, registro) == 0) {
                // Caso especial: Único nodo
                if (actual == cabeza && actual->siguiente == cabeza) {
                    delete actual;
                    cabeza = nullptr;
                } 
                // Caso general: Múltiples nodos
                else {
                    // Reconectar nodos adyacentes
                    actual->anterior->siguiente = actual->siguiente;
                    actual->siguiente->anterior = actual->anterior;
                    
                    // Si se elimina la cabeza, actualizarla
                    if (actual == cabeza) {
                        cabeza = actual->siguiente;
                    }
                    delete actual;
                }
                tamanio--;
                return true;
            }
            actual = actual->siguiente;
        } while (actual != cabeza);
        
        return false;
    }

    /**
     * Busca un avión por su registro
     * Retorna puntero al avión o nullptr si no existe
     */
    Avion* buscar(const char* registro) {
        if (cabeza == nullptr) return nullptr;

        NodoListaCircular* actual = cabeza;
        do {
            if (strcmp(actual->avion.registro, registro) == 0) {
                return &(actual->avion);
            }
            actual = actual->siguiente;
        } while (actual != cabeza);
        
        return nullptr;
    }

    /**
     * Muestra todos los aviones de la lista por consola
     */
    void mostrar() const {
        if (cabeza == nullptr) {
            std::cout << "Lista vacia." << std::endl;
            return;
        }

        NodoListaCircular* actual = cabeza;
        do {
            actual->avion.mostrar();
            actual = actual->siguiente;
        } while (actual != cabeza);
    }

    /**
     * Genera archivo .dot para Graphviz con la representación visual de la lista
     */
    void generarReporteGraphviz(const char* filename, const char* titulo) const {
        std::ofstream archivo(filename);
        
        // Encabezado Graphviz
        archivo << "digraph G {" << std::endl;
        archivo << "    rankdir=LR;" << std::endl;          // Orientación horizontal
        archivo << "    node [shape=record];" << std::endl; // Nodos rectangulares
        archivo << "    label=\"" << titulo << "\";" << std::endl;
        archivo << "    labelloc=t;" << std::endl;

        if (cabeza == nullptr) {
            archivo << "    vacio [label=\"Lista Vacia\"];" << std::endl;
        } else {
            // Generar nodos
            NodoListaCircular* actual = cabeza;
            int id = 0;
            do {
                archivo << "    nodo" << id << " [label=\"{Registro: " << actual->avion.registro
                        << "\\nModelo: " << actual->avion.modelo
                        << "\\nEstado: " << actual->avion.estado << "}\"];" << std::endl;
                actual = actual->siguiente;
                id++;
            } while (actual != cabeza);

            // Generar conexiones circulares bidireccionales
            for (int i = 0; i < id; i++) {
                int next = (i + 1) % id;  // Módulo para crear el ciclo
                archivo << "    nodo" << i << " -> nodo" << next << " [dir=both];" << std::endl;
            }
        }

        archivo << "}" << std::endl;
        archivo.close();
    }

    bool estaVacia() const {
        return cabeza == nullptr;
    }
    
    /**
     * Obtener puntero a la cabeza de la lista 
     */
    NodoListaCircular* getCabeza() const {
        return cabeza;
    }
};

#endif