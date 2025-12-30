#ifndef MATRIZ_DISPERSA_H
#define MATRIZ_DISPERSA_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

class NodoMatriz {
public:
    char vuelo[50];      // clave fila
    char ciudad[100];    // clave columna
    char piloto[100];    // valor (puede ser nombre o ID)

    NodoMatriz* derecha; // siguiente en la fila (por ciudad)
    NodoMatriz* abajo;   // siguiente en la columna (por vuelo)

    NodoMatriz(const char* v, const char* c, const char* p)
        : derecha(nullptr), abajo(nullptr) {
        strcpy(vuelo, v);
        strcpy(ciudad, c);
        strcpy(piloto, p);
    }
};


class NodoFilaVuelo {
public:
    char vuelo[50];
    NodoMatriz* inicio;      // primera celda de la fila (ordenada por ciudad)
    NodoFilaVuelo* siguiente;

    NodoFilaVuelo(const char* v) : inicio(nullptr), siguiente(nullptr) {
        strcpy(vuelo, v);
    }
};


class NodoColCiudad {
public:
    char ciudad[100];
    NodoMatriz* inicio;      // primera celda de la columna (ordenada por vuelo)
    NodoColCiudad* siguiente;

    NodoColCiudad(const char* c) : inicio(nullptr), siguiente(nullptr) {
        strcpy(ciudad, c);
    }
};


class MatrizDispersa {
private:
    NodoFilaVuelo* filas;     // lista ordenada por vuelo
    NodoColCiudad* columnas;  // lista ordenada por ciudad

    // ---------- Crear/Buscar fila (vuelo) ordenada ----------
    NodoFilaVuelo* buscarOCrearFila(const char* vuelo) {
        NodoFilaVuelo* ant = nullptr;
        NodoFilaVuelo* act = filas;

        while (act && strcmp(act->vuelo, vuelo) < 0) {
            ant = act;
            act = act->siguiente;
        }

        if (act && strcmp(act->vuelo, vuelo) == 0) return act;

        NodoFilaVuelo* nueva = new NodoFilaVuelo(vuelo);
        if (!ant) {
            nueva->siguiente = filas;
            filas = nueva;
        } else {
            nueva->siguiente = act;
            ant->siguiente = nueva;
        }
        return nueva;
    }

    // ---------- Crear/Buscar columna (ciudad) ordenada ----------
    NodoColCiudad* buscarOCrearColumna(const char* ciudad) {
        NodoColCiudad* ant = nullptr;
        NodoColCiudad* act = columnas;

        while (act && strcmp(act->ciudad, ciudad) < 0) {
            ant = act;
            act = act->siguiente;
        }

        if (act && strcmp(act->ciudad, ciudad) == 0) return act;

        NodoColCiudad* nueva = new NodoColCiudad(ciudad);
        if (!ant) {
            nueva->siguiente = columnas;
            columnas = nueva;
        } else {
            nueva->siguiente = act;
            ant->siguiente = nueva;
        }
        return nueva;
    }

    // ---------- Insertar celda en fila (orden ciudad) ----------
    void insertarEnFilaOrdenado(NodoFilaVuelo* fila, NodoMatriz* celda) {
        NodoMatriz* ant = nullptr;
        NodoMatriz* act = fila->inicio;

        while (act && strcmp(act->ciudad, celda->ciudad) < 0) {
            ant = act;
            act = act->derecha;
        }

        celda->derecha = act;
        if (!ant) fila->inicio = celda;
        else ant->derecha = celda;
    }

    // ---------- Insertar celda en columna (orden vuelo) ----------
    void insertarEnColumnaOrdenado(NodoColCiudad* col, NodoMatriz* celda) {
        NodoMatriz* ant = nullptr;
        NodoMatriz* act = col->inicio;

        while (act && strcmp(act->vuelo, celda->vuelo) < 0) {
            ant = act;
            act = act->abajo;
        }

        celda->abajo = act;
        if (!ant) col->inicio = celda;
        else ant->abajo = celda;
    }

    // ---------- Buscar celda exacta (vuelo, ciudad) en una fila ----------
    NodoMatriz* buscarCeldaEnFila(NodoFilaVuelo* fila, const char* ciudad) {
        NodoMatriz* act = fila->inicio;
        while (act) {
            int cmp = strcmp(act->ciudad, ciudad);
            if (cmp == 0) return act;
            if (cmp > 0) return nullptr; // ya se pasó (porque está ordenado)
            act = act->derecha;
        }
        return nullptr;
    }

    // ---------- Remover encabezado fila si queda vacío ----------
    void limpiarFilaSiVacia(const char* vuelo) {
        NodoFilaVuelo* ant = nullptr;
        NodoFilaVuelo* act = filas;

        while (act && strcmp(act->vuelo, vuelo) != 0) {
            ant = act;
            act = act->siguiente;
        }
        if (!act) return;

        if (act->inicio == nullptr) {
            if (!ant) filas = act->siguiente;
            else ant->siguiente = act->siguiente;
            delete act;
        }
    }

    // ---------- Remover encabezado columna si queda vacío ----------
    void limpiarColumnaSiVacia(const char* ciudad) {
        NodoColCiudad* ant = nullptr;
        NodoColCiudad* act = columnas;

        while (act && strcmp(act->ciudad, ciudad) != 0) {
            ant = act;
            act = act->siguiente;
        }
        if (!act) return;

        if (act->inicio == nullptr) {
            if (!ant) columnas = act->siguiente;
            else ant->siguiente = act->siguiente;
            delete act;
        }
    }

public:
    MatrizDispersa() : filas(nullptr), columnas(nullptr) {}

    ~MatrizDispersa() {
        // liberar todas las celdas recorriendo filas
        NodoFilaVuelo* f = filas;
        while (f) {
            NodoMatriz* c = f->inicio;
            while (c) {
                NodoMatriz* tmp = c;
                c = c->derecha;
                delete tmp;
            }
            NodoFilaVuelo* ft = f;
            f = f->siguiente;
            delete ft;
        }

        // liberar encabezados columnas
        NodoColCiudad* col = columnas;
        while (col) {
            NodoColCiudad* ct = col;
            col = col->siguiente;
            delete ct;
        }
    }

    void insertar(const char* ciudad, const char* vuelo, const char* piloto) {
        NodoFilaVuelo* fila = buscarOCrearFila(vuelo);
        NodoColCiudad* col = buscarOCrearColumna(ciudad);

        // Si ya existe la celda (vuelo, ciudad), solo actualiza piloto
        NodoMatriz* existente = buscarCeldaEnFila(fila, ciudad);
        if (existente) {
            strcpy(existente->piloto, piloto);
            return;
        }

        // Crear nueva celda y enlazarla ordenada en fila y columna
        NodoMatriz* nueva = new NodoMatriz(vuelo, ciudad, piloto);
        insertarEnFilaOrdenado(fila, nueva);
        insertarEnColumnaOrdenado(col, nueva);
    }


    void buscarPorPiloto(const char* piloto) {
        std::cout << "\nAsignaciones del piloto " << piloto << ":\n";
        bool encontrado = false;

        NodoFilaVuelo* fila = filas;
        while (fila) {
            NodoMatriz* celda = fila->inicio;
            while (celda) {
                if (strcmp(celda->piloto, piloto) == 0) {
                    std::cout << "Vuelo: " << celda->vuelo
                              << " | Ciudad: " << celda->ciudad << "\n";
                    encontrado = true;
                }
                celda = celda->derecha;
            }
            fila = fila->siguiente;
        }

        if (!encontrado) std::cout << "No encontrado.\n";
    }


    void buscarPorVuelo(const char* vuelo) {
        NodoFilaVuelo* fila = filas;
        while (fila && strcmp(fila->vuelo, vuelo) != 0) fila = fila->siguiente;

        if (!fila) {
            std::cout << "Vuelo no encontrado.\n";
            return;
        }

        std::cout << "\nAsignaciones del vuelo " << vuelo << ":\n";
        NodoMatriz* celda = fila->inicio;
        while (celda) {
            std::cout << "Ciudad: " << celda->ciudad
                      << " | Piloto: " << celda->piloto << "\n";
            celda = celda->derecha;
        }
    }


    void mostrar() {
        NodoFilaVuelo* fila = filas;
        while (fila) {
            std::cout << "\nVuelo: " << fila->vuelo << "\n";
            NodoMatriz* celda = fila->inicio;
            while (celda) {
                std::cout << "  Ciudad: " << celda->ciudad
                          << " | Piloto: " << celda->piloto << "\n";
                celda = celda->derecha;
            }
            fila = fila->siguiente;
        }
    }

    void eliminarPiloto(const char* piloto) {
        NodoFilaVuelo* fila = filas;

        while (fila) {
            NodoMatriz* act = fila->inicio;
            NodoMatriz* ant = nullptr;

            while (act) {
                if (strcmp(act->piloto, piloto) == 0) {
                    // 1) quitar de la FILA
                    if (ant) ant->derecha = act->derecha;
                    else fila->inicio = act->derecha;

                    // 2) quitar de la COLUMNA correspondiente (ciudad)
                    NodoColCiudad* col = columnas;
                    while (col && strcmp(col->ciudad, act->ciudad) != 0) col = col->siguiente;

                    if (col) {
                        NodoMatriz* ca = col->inicio;
                        NodoMatriz* cp = nullptr;
                        while (ca) {
                            if (ca == act) {
                                if (cp) cp->abajo = ca->abajo;
                                else col->inicio = ca->abajo;
                                break;
                            }
                            cp = ca;
                            ca = ca->abajo;
                        }
                    }

                    char vueloBorrar[50]; strcpy(vueloBorrar, act->vuelo);
                    char ciudadBorrar[100]; strcpy(ciudadBorrar, act->ciudad);

                    delete act;

                    // 3) limpiar encabezados si quedaron vacíos
                    limpiarFilaSiVacia(vueloBorrar);
                    limpiarColumnaSiVacia(ciudadBorrar);
                    return;
                }

                ant = act;
                act = act->derecha;
            }

            fila = fila->siguiente;
        }

        std::cout << "Piloto no encontrado en matriz.\n";
    }

  
    void generarReporteGraphviz(const char* nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        if (!archivo.is_open()) return;

        // Recolectar vuelos (columnas) en orden
        std::vector<const char*> vuelos;
        NodoFilaVuelo* filaV = filas;
        while (filaV) {
            vuelos.push_back(filaV->vuelo);
            filaV = filaV->siguiente;
        }

        archivo << "digraph TablaMatrizDispersa {\n";
        archivo << "  node [shape=plaintext, fontname=\"Arial\"];\n\n";
        archivo << "  Tabla [\n";
        archivo << "    label=<\n";
        archivo << "      <table border=\"1\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"6\">\n";

        // Encabezado: Ciudad \ Vuelo
        archivo << "        <tr>\n";
        archivo << "          <td bgcolor=\"#FFD700\"><b>Ciudad \\\\ Vuelo</b></td>\n";
        for (const char* v : vuelos) {
            archivo << "          <td bgcolor=\"#90EE90\"><b>" << v << "</b></td>\n";
        }
        archivo << "        </tr>\n\n";

        // Filas (ciudades) en orden
        NodoColCiudad* col = columnas;
        while (col) {
            archivo << "        <tr>\n";
            archivo << "          <td bgcolor=\"#ADD8E6\"><b>" << col->ciudad << "</b></td>\n";

            // Para cada vuelo, buscar si existe celda (vuelo, ciudad)
            for (const char* v : vuelos) {
                const char* piloto = "";

                // Buscar en la fila de ese vuelo
                NodoFilaVuelo* filaVuelo = filas;
                while (filaVuelo && strcmp(filaVuelo->vuelo, v) != 0) {
                    filaVuelo = filaVuelo->siguiente;
                }

                if (filaVuelo) {
                    NodoMatriz* celda = filaVuelo->inicio;
                    while (celda) {
                        if (strcmp(celda->ciudad, col->ciudad) == 0) {
                            piloto = celda->piloto;
                            break;
                        }
                        celda = celda->derecha;
                    }
                }

                if (strlen(piloto) > 0)
                    archivo << "          <td bgcolor=\"#FFFFE0\">" << piloto << "</td>\n";
                else
                    archivo << "          <td></td>\n";
            }

            archivo << "        </tr>\n\n";
            col = col->siguiente;
        }

        archivo << "      </table>\n";
        archivo << "    >\n";
        archivo << "  ];\n";
        archivo << "}\n";

        archivo.close();
        std::cout << "Reporte Graphviz generado (Ciudades x Vuelos): " << nombreArchivo << "\n";
    }
};

#endif
