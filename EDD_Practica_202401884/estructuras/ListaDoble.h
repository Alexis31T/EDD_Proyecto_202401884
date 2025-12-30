#ifndef LISTA_DOBLE_H
#define LISTA_DOBLE_H

#include "NodoListaDoble.h"
#include <fstream>

class ListaDoble {
private:
    NodoListaDoble* cabeza;
    int tamanio;

public:
    ListaDoble() : cabeza(nullptr), tamanio(0) {}

    ~ListaDoble() {
        while (cabeza != nullptr) {
            NodoListaDoble* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
        }
    }

    // Insertar pasajero manteniendo el orden: primero por vuelo, luego por asiento
    void insertarOrdenado(const Pasajero& pasajero) {
        NodoListaDoble* nuevo = new NodoListaDoble(pasajero);

        if (cabeza == nullptr) {  // Lista vacía, este es el primero
            cabeza = nuevo;
            tamanio++;
            return;
        }

        // Si va antes de la cabeza (menor vuelo o mismo vuelo pero menor asiento)
        if (strcmp(pasajero.vuelo, cabeza->pasajero.vuelo) < 0 ||
            (strcmp(pasajero.vuelo, cabeza->pasajero.vuelo) == 0 && 
             pasajero.asiento < cabeza->pasajero.asiento)) {
            nuevo->siguiente = cabeza;
            cabeza->anterior = nuevo;
            cabeza = nuevo;
            tamanio++;
            return;
        }

        // Buscar dónde insertarlo recorriendo la lista
        NodoListaDoble* actual = cabeza;
        while (actual->siguiente != nullptr) {
            // Revisar si va antes del siguiente nodo
            if (strcmp(pasajero.vuelo, actual->siguiente->pasajero.vuelo) < 0 ||
                (strcmp(pasajero.vuelo, actual->siguiente->pasajero.vuelo) == 0 && 
                 pasajero.asiento < actual->siguiente->pasajero.asiento)) {
                break;  // Aquí es donde va
            }
            actual = actual->siguiente;
        }

        // Enlazar el nuevo nodo entre actual y el siguiente
        nuevo->siguiente = actual->siguiente;
        nuevo->anterior = actual;
        if (actual->siguiente != nullptr) {
            actual->siguiente->anterior = nuevo;
        }
        actual->siguiente = nuevo;
        tamanio++;
    }

    Pasajero* buscar(const char* pasaporte) {
        NodoListaDoble* actual = cabeza;
        while (actual != nullptr) {
            if (strcmp(actual->pasajero.numero_pasaporte, pasaporte) == 0) {
                return &(actual->pasajero);
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void mostrar() const {
        if (cabeza == nullptr) {
            std::cout << "Lista vacia." << std::endl;
            return;
        }

        NodoListaDoble* actual = cabeza;
        while (actual != nullptr) {
            actual->pasajero.mostrar();
            actual = actual->siguiente;
        }
    }

    void generarReporteGraphviz(const char* filename) const {
        std::ofstream archivo(filename);
        archivo << "digraph G {" << std::endl;
        archivo << "    rankdir=LR;" << std::endl;
        archivo << "    node [shape=record];" << std::endl;
        archivo << "    label=\"Lista Doble de Pasajeros Registrados\";" << std::endl;
        archivo << "    labelloc=t;" << std::endl;

        if (cabeza == nullptr) {
            archivo << "    vacio [label=\"Lista Vacia\"];" << std::endl;
        } else {
            NodoListaDoble* actual = cabeza;
            int id = 0;
            while (actual != nullptr) {
                archivo << "    nodo" << id << " [label=\"{" << actual->pasajero.nombre
                        << "\\nPasaporte: " << actual->pasajero.numero_pasaporte
                        << "\\nVuelo: " << actual->pasajero.vuelo
                        << "\\nAsiento: " << actual->pasajero.asiento << "}\"];" << std::endl;
                
                if (actual->siguiente != nullptr) {
                    archivo << "    nodo" << id << " -> nodo" << (id + 1) << " [dir=both];" << std::endl;
                }
                
                actual = actual->siguiente;
                id++;
            }
        }

        archivo << "}" << std::endl;
        archivo.close();
    }

    bool estaVacia() const {
        return cabeza == nullptr;
    }
};

#endif