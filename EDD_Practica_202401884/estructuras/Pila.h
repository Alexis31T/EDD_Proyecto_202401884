#ifndef PILA_H
#define PILA_H

#include "NodoPila.h"
#include <fstream>

// Pila LIFO para equipaje - el último en entrar es el primero en salir
class Pila {
private:
    NodoPila* tope;  // Apunta arriba de todo, aquí sacamos y metemos
    int tamanio;

public:
    Pila() : tope(nullptr), tamanio(0) {}

    // Liberar toda la memoria vaciando la pila
    ~Pila() {
        while (tope != nullptr) {
            NodoPila* temp = tope;
            tope = tope->siguiente;
            delete temp;
        }
    }

    // Meter un equipaje arriba de todo (push)
    void push(const Equipaje& equipaje) {
        NodoPila* nuevo = new NodoPila(equipaje);
        nuevo->siguiente = tope;  // El nuevo apunta al que estaba arriba
        tope = nuevo;             // Ahora el nuevo está arriba
        tamanio++;
    }

    // Sacar el equipaje de arriba y retornarlo (pop)
    Equipaje pop() {
        if (tope == nullptr) {
            throw "Pila vacia";
        }

        NodoPila* temp = tope;
        Equipaje equipaje = tope->equipaje;  // Guardar antes de borrar
        tope = tope->siguiente;               // Bajar el tope
        delete temp;
        tamanio--;
        return equipaje;
    }

    // Imprimir todo el equipaje sin sacarlo
    void mostrar() const {
        if (tope == nullptr) {
            std::cout << "Pila vacia." << std::endl;
            return;
        }

        NodoPila* actual = tope;
        while (actual != nullptr) {
            actual->equipaje.mostrar();
            actual = actual->siguiente;
        }
    }

    // Crear archivo DOT para visualizar la pila verticalmente
    void generarReporteGraphviz(const char* filename) const {
        std::ofstream archivo(filename);
        archivo << "digraph G {" << std::endl;
        archivo << "    rankdir=TB;" << std::endl;  // Vertical: tope arriba
        archivo << "    node [shape=record];" << std::endl;
        archivo << "    label=\"Pila de Equipaje\";" << std::endl;
        archivo << "    labelloc=t;" << std::endl;

        if (tope == nullptr) {
            archivo << "    vacio [label=\"Pila Vacia\"];" << std::endl;
        } else {
            NodoPila* actual = tope;
            int id = 0;
            while (actual != nullptr) {
                archivo << "    nodo" << id << " [label=\"{Pasaporte: " 
                        << actual->equipaje.numero_pasaporte
                        << "\\nCantidad: " << actual->equipaje.cantidad << "}\"];" << std::endl;
                
                if (actual->siguiente != nullptr) {  // Flecha hacia abajo
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
        return tope == nullptr;
    }

    int getTamanio() const {
        return tamanio;
    }
};

#endif