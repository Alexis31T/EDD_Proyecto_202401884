#ifndef COLA_H
#define COLA_H

#include "NodoCola.h"
#include <fstream>

// Cola FIFO para pasajeros - primero en entrar, primero en salir
class Cola {
private:
    NodoCola* frente;  // Aquí sacamos (dequeue)
    NodoCola* final;   // Aquí metemos (enqueue)
    int tamanio;

public:
    Cola() : frente(nullptr), final(nullptr), tamanio(0) {}

    // Liberar toda la memoria recorriendo la cola
    ~Cola() {
        while (frente != nullptr) {
            NodoCola* temp = frente;
            frente = frente->siguiente;
            delete temp;
        }
    }

    // Meter un pasajero al final de la cola
    void enqueue(const Pasajero& pasajero) {
        NodoCola* nuevo = new NodoCola(pasajero);
        
        if (final == nullptr) {
            frente = final = nuevo;  // Primera persona en la cola
        } else {
            final->siguiente = nuevo;  // Enlazar al final
            final = nuevo;             // Mover el puntero final
        }
        tamanio++;
    }

    // Sacar el primer pasajero de la cola y retornarlo
    Pasajero dequeue() {
        if (frente == nullptr) {
            throw "Cola vacia";
        }

        NodoCola* temp = frente;
        Pasajero pasajero = frente->pasajero;  // Guardar antes de borrar
        frente = frente->siguiente;             // Avanzar el frente
        
        if (frente == nullptr) {  // Si era el último, resetear final también
            final = nullptr;
        }
        
        delete temp;
        tamanio--;
        return pasajero;
    }

    // Imprimir todos los pasajeros sin sacarlos de la cola
    void mostrar() const {
        if (frente == nullptr) {
            std::cout << "Cola vacia." << std::endl;
            return;
        }

        NodoCola* actual = frente;
        while (actual != nullptr) {
            actual->pasajero.mostrar();
            actual = actual->siguiente;
        }
    }

    // Crear el archivo DOT para visualizar la cola
    void generarReporteGraphviz(const char* filename) const {
        std::ofstream archivo(filename);
        archivo << "digraph G {" << std::endl;
        archivo << "    rankdir=LR;" << std::endl;  // Cola horizontal
        archivo << "    node [shape=record];" << std::endl;
        archivo << "    label=\"Cola de Pasajeros\";" << std::endl;
        archivo << "    labelloc=t;" << std::endl;

        if (frente == nullptr) {
            archivo << "    vacio [label=\"Cola Vacia\"];" << std::endl;
        } else {
            NodoCola* actual = frente;
            int id = 0;
            while (actual != nullptr) {
                archivo << "    nodo" << id << " [label=\"{" << actual->pasajero.nombre
                        << "\\nPasaporte: " << actual->pasajero.numero_pasaporte
                        << "\\nVuelo: " << actual->pasajero.vuelo << "}\"];" << std::endl;
                
                if (actual->siguiente != nullptr) {  // Flechas van de frente hacia final
                    archivo << "    nodo" << id << " -> nodo" << (id + 1) << ";" << std::endl;
                }
                
                actual = actual->siguiente;
                id++;
            }
        }

        archivo << "}" << std::endl;
        archivo.close();
    }

    bool estaVacia() const {
        return frente == nullptr;
    }

    int getTamanio() const {
        return tamanio;
    }
};

#endif