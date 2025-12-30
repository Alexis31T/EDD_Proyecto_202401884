#include <iostream>
#include <fstream>
#include <cstring>
#include "models/Avion.h"
#include "models/Pasajero.h"
#include "models/Equipaje.h"
#include "models/Piloto.h"
#include "models/Ruta.h"
#include "estructuras/ListaCircularDoble.h"
#include "estructuras/ListaDoble.h"
#include "estructuras/Cola.h"
#include "estructuras/Pila.h"
#include "estructuras/ArbolB.h"
#include "estructuras/ArbolBinarioBusqueda.h"
#include "estructuras/TablaHash.h"
#include "estructuras/Grafo.h"
#include "estructuras/MatrizDispersa.h"
#include "utils/JsonParser.h"
#include "utils/Graphviz.h"

using namespace std;

// Variables globales que almacenan las estructuras de datos principales del sistema
ListaCircularDoble avionesDisponibles;      // Lista circular para aviones operativos
ListaCircularDoble avionesMantenimiento;    // Lista circular para aviones en reparación
Cola colaPasajeros;                         // Cola FIFO para pasajeros en ventanilla
Pila pilaEquipaje;                          // Pila LIFO para equipaje facturado
ListaDoble listaPasajerosRegistrados;       // Lista ordenada de pasajeros procesados

// Estructuras avanzadas para extensión del sistema
ArbolB arbolAvionesDisponibles;             // Árbol B orden 5 para aviones disponibles
ArbolBinarioBusqueda arbolPilotos;          // ABB ordenado por horas de vuelo
TablaHash tablaPilotos;                     // Tabla Hash M=19 para búsqueda rápida de pilotos
Grafo grafoRutas;                           // Grafo dirigido de rutas entre ciudades
MatrizDispersa matrizAsignaciones;          // Matriz dispersa piloto-vuelo-ciudad

// Estructura auxiliar para inserción en matriz
struct InsertadorMatriz {
    static MatrizDispersa* matriz;
    static void insertar(const Piloto& p) {
        static const char* ciudades[] = {"Madrid", "Barcelona", "Valencia", "Sevilla", 
                                        "Bilbao", "Zaragoza", "Granada", "Murcia", 
                                        "Córdoba", "Girona", "Málaga", "Alicante"};
        static int indice = 0;
        const char* ciudad = ciudades[indice % 12];
        indice++;
        matriz->insertar(ciudad, p.vuelo, p.nombre);
    }
};
MatrizDispersa* InsertadorMatriz::matriz = nullptr;

/**
 * Muestra el menú principal del sistema con todas las opciones disponibles
 */
void mostrarMenu() {
    cout << "\n========================================" << endl;
    cout << "   SISTEMA DE GESTION DE AEROPUERTO" << endl;
    cout << "========================================" << endl;
    cout << "1. Cargar aviones" << endl;
    cout << "2. Cargar movimientos" << endl;
    cout << "3. Cargar pilotos" << endl;
    cout << "4. Cargar rutas" << endl;
    cout << "5. Consultar pilotos (Arbol/Hash)" << endl;
    cout << "6. Visualizar recorridos de pilotos" << endl;
    cout << "7. Consultar ruta mas corta" << endl;
    cout << "8. Mostrar asignaciones piloto-vuelo" << endl;
    cout << "9. Generar reportes avanzados" << endl;
    cout << "========================================" << endl;
    cout << "10. Salir" << endl;
    cout << "========================================" << endl;
    cout << "Opcion: ";
}

/**
 * Carga los aviones desde un archivo JSON y los distribuye en las listas
 * correspondientes según su estado (Disponible o Mantenimiento)
 */
void cargarAviones() {
    char ruta[512];
    cout << "Ingrese la ruta del archivo de aviones (ej: data/aviones.json): ";
    cin.getline(ruta, 512);
    
    // Limpieza de comillas en caso de que el usuario pegue una ruta con comillas
    if (ruta[0] == '"') {
        int len = strlen(ruta);
        for (int i = 0; i < len - 1; i++) {
            ruta[i] = ruta[i + 1];
        }
        if (ruta[strlen(ruta) - 1] == '"') {
            ruta[strlen(ruta) - 1] = '\0';
        }
    }
    
    // Delegar la carga al parser JSON que distribuye automáticamente según estado
    JsonParser::cargarAviones(ruta, avionesDisponibles, avionesMantenimiento);
    
    // Copiar aviones disponibles al Árbol B para el módulo avanzado
    NodoListaCircular* actual = avionesDisponibles.getCabeza();
    if (actual != nullptr) {
        NodoListaCircular* inicio = actual;
        do {
            arbolAvionesDisponibles.insertar(actual->avion);
            actual = actual->siguiente;
        } while (actual != inicio);
    }
}

/**
 * Carga los pasajeros desde un archivo JSON y los encola en la cola de registro
 * Simula la llegada de pasajeros a la ventanilla del aeropuerto
 */
void cargarPasajeros() {
    char ruta[512];
    cout << "Ingrese la ruta del archivo de pasajeros (ej: data/pasajeros.json): ";
    cin.getline(ruta, 512);
    
    // Limpieza de comillas
    if (ruta[0] == '"') {
        int len = strlen(ruta);
        for (int i = 0; i < len - 1; i++) {
            ruta[i] = ruta[i + 1];
        }
        if (ruta[strlen(ruta) - 1] == '"') {
            ruta[strlen(ruta) - 1] = '\0';
        }
    }
    
    // Los pasajeros se agregan a la cola en orden FIFO
    JsonParser::cargarPasajeros(ruta, colaPasajeros);
}

/**
 * Procesa un archivo de movimientos que contiene comandos para:
 * - IngresoEquipajes: Registrar pasajeros y procesar su equipaje
 * - MantenimientoAviones: Mover aviones entre listas según su estado
 */
void procesarMovimientos() {
    char ruta[512];
    cout << "Ingrese la ruta del archivo de movimientos: ";
    cin.getline(ruta, 512);
    
    // Limpieza de comillas
    if (ruta[0] == '"') {
        int len = strlen(ruta);
        for (int i = 0; i < len - 1; i++) {
            ruta[i] = ruta[i + 1];
        }
        if (ruta[strlen(ruta) - 1] == '"') {
            ruta[strlen(ruta) - 1] = '\0';
        }
    }

    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << ruta << endl;
        return;
    }

    char linea[512];
    while (archivo.getline(linea, 512)) {
        // Limpiar la línea: eliminar espacios, punto y coma y saltos de línea
        char comando[256];
        int j = 0;
        for (int i = 0; linea[i] != '\0'; i++) {
            if (linea[i] != ' ' && linea[i] != ';' && linea[i] != '\n' && linea[i] != '\r') {
                comando[j++] = linea[i];
            }
        }
        comando[j] = '\0';
        
        if (strlen(comando) == 0) continue;
        
        // Comando 1: IngresoEquipajes
        // Desencola un pasajero, lo registra en la lista ordenada y apila su equipaje
        if (strstr(comando, "IngresoEquipajes") != nullptr) {
            if (!colaPasajeros.estaVacia()) {
                // Sacar pasajero de la cola (FIFO)
                Pasajero p = colaPasajeros.dequeue();
                
                // Insertar en lista ordenada por vuelo y asiento
                listaPasajerosRegistrados.insertarOrdenado(p);
                
                // Solo apilar equipaje si el pasajero tiene maletas
                if (p.equipaje_facturado > 0) {
                    Equipaje eq(p.numero_pasaporte, p.equipaje_facturado);
                    pilaEquipaje.push(eq);  // Push en pila (LIFO)
                    cout << "Pasajero registrado: " << p.nombre << " (con equipaje)" << endl;
                } else {
                    cout << "Pasajero registrado: " << p.nombre << " (sin equipaje)" << endl;
                }
            } else {
                cout << "No hay pasajeros en la cola." << endl;
            }
        }
        // Comando 2: MantenimientoAviones,Ingreso/Salida,Registro
        // Formato: MantenimientoAviones,Ingreso,N456AB
        else if (strstr(comando, "MantenimientoAviones") != nullptr) {
            // Tokenizar por comas para extraer acción y registro
            char* token = strtok(comando, ",");
            token = strtok(nullptr, ",");  // Obtener Ingreso o Salida
            
            if (token == nullptr) continue;
            
            char accion[50];
            strcpy(accion, token);
            
            token = strtok(nullptr, ",");  // Obtener número de registro
            if (token == nullptr) continue;
            
            char registro[50];
            strcpy(registro, token);
            
            // Ingreso: Mover avión de Disponibles a Mantenimiento
            if (strcmp(accion, "Ingreso") == 0) {
                Avion* avion = avionesDisponibles.buscar(registro);
                if (avion != nullptr) {
                    // Copiar el avión, actualizar estado y mover entre listas
                    Avion temp = *avion;
                    strcpy(temp.estado, "Mantenimiento");
                    avionesDisponibles.eliminar(registro);
                    avionesMantenimiento.insertar(temp);
                    cout << "Avion " << registro << " movido a Mantenimiento" << endl;
                } else {
                    cout << "Avion " << registro << " no encontrado en Disponibles" << endl;
                }
            }
            // Salida: Mover avión de Mantenimiento a Disponibles
            else if (strcmp(accion, "Salida") == 0) {
                Avion* avion = avionesMantenimiento.buscar(registro);
                if (avion != nullptr) {
                    // Copiar el avión, actualizar estado y mover entre listas
                    Avion temp = *avion;
                    strcpy(temp.estado, "Disponible");
                    avionesMantenimiento.eliminar(registro);
                    avionesDisponibles.insertar(temp);
                    cout << "Avion " << registro << " movido a Disponible" << endl;
                } else {
                    cout << "Avion " << registro << " no encontrado en Mantenimiento" << endl;
                }
            }
        }
        // Comando 3: AsignarPiloto,NombrePiloto,Vuelo,Ciudad
        // Formato: AsignarPiloto,Carlos Mendez,AV100,Guatemala;
        else if (strstr(comando, "AsignarPiloto") != nullptr) {
            // Parsear sin eliminar espacios (necesarios para nombres)
            char lineaOriginal[512];
            strcpy(lineaOriginal, linea);
            
            char* token = strtok(lineaOriginal, ",");
            token = strtok(nullptr, ",");  // Nombre del piloto
            
            if (token == nullptr) continue;
            char piloto[100];
            strcpy(piloto, token);
            
            token = strtok(nullptr, ",");  // Vuelo
            if (token == nullptr) continue;
            char vuelo[50];
            strcpy(vuelo, token);
            
            token = strtok(nullptr, ";");  // Ciudad
            if (token == nullptr) continue;
            char ciudad[100];
            strcpy(ciudad, token);
            
            // Insertar en matriz dispersa (ciudad, vuelo, piloto)
            matrizAsignaciones.insertar(ciudad, vuelo, piloto);
        }
        // Comando 4: DarDeBaja,NumeroID
        // Formato: DarDeBaja,P12345;
        else if (strstr(comando, "DarDeBaja") != nullptr) {
            char* token = strtok(comando, ",");
            token = strtok(nullptr, ";");  // Obtener número de ID
            
            if (token == nullptr) continue;
            
            char numero_id[50];
            strcpy(numero_id, token);
            
            // Buscar el nombre del piloto en la tabla hash antes de eliminar
            Piloto* piloto = tablaPilotos.buscar(numero_id);
            if (piloto != nullptr) {
                // Eliminar de matriz usando el nombre
                matrizAsignaciones.eliminarPiloto(piloto->nombre);
                
                // Eliminar de ABB (ya imprime mensaje)
                arbolPilotos.eliminar(numero_id);
                
                // Eliminar de Hash
                tablaPilotos.eliminar(numero_id);
                
                cout << "Piloto " << numero_id << " dado de baja del sistema." << endl;
            } else {
                cout << "Piloto " << numero_id << " no encontrado." << endl;
            }
        }
    }

    archivo.close();
    cout << "Movimientos procesados exitosamente." << endl;
}

/**
 * Busca un pasajero en la lista de registrados por su número de pasaporte
 * Utiliza búsqueda lineal O(n) en la lista doble
 */
void consultarPasajero() {
    char pasaporte[50];
    cout << "Ingrese el numero de pasaporte: ";
    cin.getline(pasaporte, 50);

    // Buscar en la lista de pasajeros registrados
    Pasajero* p = listaPasajerosRegistrados.buscar(pasaporte);
    if (p != nullptr) {
        cout << "\nPasajero encontrado:" << endl;
        p->mostrar();
    } else {
        cout << "Pasajero no encontrado." << endl;
    }
}

/**
 * Registra manualmente el siguiente pasajero de la cola
 * Similar a IngresoEquipajes pero ejecutado individualmente
 */
void registrarSiguientePasajero() {
    if (colaPasajeros.estaVacia()) {
        cout << "No hay pasajeros en la cola." << endl;
        return;
    }

    // Desencolar siguiente pasajero (FIFO)
    Pasajero p = colaPasajeros.dequeue();
    
    // Insertar en lista ordenada (por vuelo y asiento)
    listaPasajerosRegistrados.insertarOrdenado(p);
    
    cout << "Pasajero registrado: " << p.nombre << endl;
    
    // Apilar equipaje solo si tiene maletas
    if (p.equipaje_facturado > 0) {
        Equipaje eq(p.numero_pasaporte, p.equipaje_facturado);
        pilaEquipaje.push(eq);
        cout << "Equipaje agregado a la pila." << endl;
    }
}

/**
 * Genera los 5 reportes básicos utilizando Graphviz
 */
void generarReportes() {
    cout << "\nGenerando reportes basicos..." << endl;

    system("mkdir reportes 2>nul");

    avionesDisponibles.generarReporteGraphviz("reportes/aviones_disponibles.dot", "Aviones Disponibles");
    Graphviz::generarImagen("reportes/aviones_disponibles.dot", "reportes/aviones_disponibles.png");

    avionesMantenimiento.generarReporteGraphviz("reportes/aviones_mantenimiento.dot", "Aviones en Mantenimiento");
    Graphviz::generarImagen("reportes/aviones_mantenimiento.dot", "reportes/aviones_mantenimiento.png");

    colaPasajeros.generarReporteGraphviz("reportes/cola_pasajeros.dot");
    Graphviz::generarImagen("reportes/cola_pasajeros.dot", "reportes/cola_pasajeros.png");

    pilaEquipaje.generarReporteGraphviz("reportes/pila_equipaje.dot");
    Graphviz::generarImagen("reportes/pila_equipaje.dot", "reportes/pila_equipaje.png");

    listaPasajerosRegistrados.generarReporteGraphviz("reportes/pasajeros_registrados.dot");
    Graphviz::generarImagen("reportes/pasajeros_registrados.dot", "reportes/pasajeros_registrados.png");

    cout << "\nReportes basicos generados en 'reportes/'." << endl;
    
    cout << "\n¿Desea abrir los reportes? (s/n): ";
    char respuesta;
    cin >> respuesta;
    if (respuesta == 's' || respuesta == 'S') {
        Graphviz::abrirImagen("reportes/aviones_disponibles.png");
        Graphviz::abrirImagen("reportes/aviones_mantenimiento.png");
        Graphviz::abrirImagen("reportes/cola_pasajeros.png");
        Graphviz::abrirImagen("reportes/pila_equipaje.png");
        Graphviz::abrirImagen("reportes/pasajeros_registrados.png");
    }
}

/**
 * Cargar pilotos desde archivo JSON e insertarlos en ABB y Tabla Hash
 */
void cargarPilotos() {
    char ruta[512];
    cout << "Ingrese la ruta del archivo de pilotos: ";
    cin.getline(ruta, 512);
    
    if (ruta[0] == '"') {
        int len = strlen(ruta);
        for (int i = 0; i < len - 1; i++) {
            ruta[i] = ruta[i + 1];
        }
        if (ruta[strlen(ruta) - 1] == '"') {
            ruta[strlen(ruta) - 1] = '\0';
        }
    }
    
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    // Leer todo el contenido con buffer más grande
    char* contenido = new char[65536];  // 64KB para archivos grandes
    char buffer[1024];
    contenido[0] = '\0';
    
    while (archivo.getline(buffer, 1024)) {
        if (strlen(contenido) + strlen(buffer) < 65535) {
            strcat(contenido, buffer);
        }
    }
    archivo.close();
    
    // Buscar cada objeto entre llaves
    int contador = 0;
    char* inicio = contenido;
    
    while ((inicio = strchr(inicio, '{')) != nullptr) {
        char* fin = strchr(inicio, '}');
        if (!fin) break;
        
        // Extraer el objeto JSON
        int len = fin - inicio + 1;
        char objeto[1024];
        strncpy(objeto, inicio, len);
        objeto[len] = '\0';
        
        Piloto piloto;
        strcpy(piloto.nombre, "");
        strcpy(piloto.numero_de_id, "");
        piloto.horas_de_vuelo = 0;
        
        // Extraer nombre
        char* campo = strstr(objeto, "\"nombre\"");
        if (campo) {
            char* ini = strchr(campo + 8, '"');
            if (ini) {
                char* fi = strchr(ini + 1, '"');
                if (fi) {
                    strncpy(piloto.nombre, ini + 1, fi - ini - 1);
                    piloto.nombre[fi - ini - 1] = '\0';
                }
            }
        }
        
        // Extraer nacionalidad
        campo = strstr(objeto, "\"nacionalidad\"");
        if (campo) {
            char* ini = strchr(campo + 14, '"');
            if (ini) {
                char* fi = strchr(ini + 1, '"');
                if (fi) {
                    strncpy(piloto.nacionalidad, ini + 1, fi - ini - 1);
                    piloto.nacionalidad[fi - ini - 1] = '\0';
                }
            }
        }
        
        // Extraer numero_de_id
        campo = strstr(objeto, "\"numero_de_id\"");
        if (campo) {
            char* ini = strchr(campo + 14, '"');
            if (ini) {
                char* fi = strchr(ini + 1, '"');
                if (fi) {
                    strncpy(piloto.numero_de_id, ini + 1, fi - ini - 1);
                    piloto.numero_de_id[fi - ini - 1] = '\0';
                }
            }
        }
        
        // Extraer vuelo
        campo = strstr(objeto, "\"vuelo\"");
        if (campo) {
            char* ini = strchr(campo + 7, '"');
            if (ini) {
                char* fi = strchr(ini + 1, '"');
                if (fi) {
                    strncpy(piloto.vuelo, ini + 1, fi - ini - 1);
                    piloto.vuelo[fi - ini - 1] = '\0';
                }
            }
        }
        
        // Extraer horas_de_vuelo
        campo = strstr(objeto, "\"horas_de_vuelo\"");
        if (campo) {
            char* ini = strchr(campo + 15, ':');
            if (ini) {
                piloto.horas_de_vuelo = atoi(ini + 1);
            }
        }
        
        // Extraer tipo_de_licencia
        campo = strstr(objeto, "\"tipo_de_licencia\"");
        if (campo) {
            char* ini = strchr(campo + 17, '"');
            if (ini) {
                char* fi = strchr(ini + 1, '"');
                if (fi) {
                    strncpy(piloto.tipo_de_licencia, ini + 1, fi - ini - 1);
                    piloto.tipo_de_licencia[fi - ini - 1] = '\0';
                }
            }
        }
        
        // Insertar si tiene datos válidos
        if (strlen(piloto.nombre) > 0 && strlen(piloto.numero_de_id) > 0) {
            arbolPilotos.insertar(piloto);
            tablaPilotos.insertar(piloto);
            contador++;
        }
        
        inicio = fin + 1;
    }
    
    delete[] contenido;  // Liberar memoria
    cout << contador << " pilotos cargados exitosamente." << endl;
}

/**
 * Cargar rutas desde archivo de texto y construir grafo dirigido
 * Formato: Origen/Destino/Distancia;
 */
void cargarRutas() {
    char ruta[512];
    cout << "Ingrese la ruta del archivo de rutas: ";
    cin.getline(ruta, 512);
    
    if (ruta[0] == '"') {
        int len = strlen(ruta);
        for (int i = 0; i < len - 1; i++) {
            ruta[i] = ruta[i + 1];
        }
        if (ruta[strlen(ruta) - 1] == '"') {
            ruta[strlen(ruta) - 1] = '\0';
        }
    }
    
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    char linea[512];
    int contador = 0;
    
    while (archivo.getline(linea, 512)) {
        // Formato: Guatemala/Miami/500;
        char origen[100], destino[100];
        int distancia;
        
        char* token = strtok(linea, "/");
        if (token) strcpy(origen, token);
        
        token = strtok(nullptr, "/");
        if (token) strcpy(destino, token);
        
        token = strtok(nullptr, ";");
        if (token) distancia = atoi(token);
        
        if (strlen(origen) > 0 && strlen(destino) > 0) {
            grafoRutas.agregarArista(origen, destino, distancia);
            contador++;
        }
    }
    
    archivo.close();
    cout << contador << " rutas cargadas exitosamente." << endl;
}

/**
 * Consultar pilotos en árbol y tabla hash
 */
void consultarPilotos() {
    cout << "\n--- Consulta de Pilotos ---" << endl;
    cout << "1. Buscar por horas de vuelo" << endl;
    cout << "2. Buscar por ID" << endl;
    cout << "3. Mostrar todos" << endl;
    cout << "Opcion: ";
    
    int opcion;
    cin >> opcion;
    cin.ignore();
    
    if (opcion == 1) {
        cout << "Ingrese horas de vuelo: ";
        int horas;
        cin >> horas;
        cin.ignore();
        
        Piloto* p = arbolPilotos.buscar(horas);
        if (p) {
            cout << "\nPiloto encontrado:" << endl;
            p->mostrar();
        } else {
            cout << "No se encontro piloto con " << horas << " horas." << endl;
        }
    } else if (opcion == 2) {
        char id[50];
        cout << "Ingrese numero de ID: ";
        cin.getline(id, 50);
        
        Piloto* p = tablaPilotos.buscar(id);
        if (p) {
            cout << "\nPiloto encontrado:" << endl;
            p->mostrar();
        } else {
            cout << "Piloto no encontrado." << endl;
        }
    } else if (opcion == 3) {
        tablaPilotos.mostrar();
        tablaPilotos.mostrarEstadisticas();
    }
}

/**
 * Visualizar recorridos del árbol de pilotos
 */
void visualizarRecorridos() {
    cout << "\n--- Recorridos del Arbol de Pilotos ---" << endl;
    cout << "1. Pre-orden" << endl;
    cout << "2. In-orden (ordenado por horas)" << endl;
    cout << "3. Post-orden" << endl;
    cout << "Opcion: ";
    
    int opcion;
    cin >> opcion;
    cin.ignore();
    
    if (opcion == 1) {
        arbolPilotos.mostrarPreorden();
    } else if (opcion == 2) {
        arbolPilotos.mostrarInorden();
    } else if (opcion == 3) {
        arbolPilotos.mostrarPostorden();
    }
}

/**
 * Consultar ruta más corta entre dos ciudades usando Dijkstra
 */
void consultarRutaCorta() {
    char origen[100], destino[100];
    
    cout << "Ingrese ciudad de origen: ";
    cin.getline(origen, 100);
    
    cout << "Ingrese ciudad de destino: ";
    cin.getline(destino, 100);
    
    grafoRutas.dijkstra(origen, destino);
}

/**
 * Mostrar asignaciones de la matriz dispersa
 */
void mostrarAsignaciones() {
    cout << "\n--- Asignaciones Piloto-Vuelo ---" << endl;
    cout << "1. Buscar por piloto" << endl;
    cout << "2. Buscar por vuelo" << endl;
    cout << "3. Mostrar todas las asignaciones" << endl;
    cout << "Opcion: ";
    
    int opcion;
    cin >> opcion;
    cin.ignore();
    
    if (opcion == 1) {
        char piloto[100];
        cout << "Ingrese nombre del piloto: ";
        cin.getline(piloto, 100);
        matrizAsignaciones.buscarPorPiloto(piloto);
    } else if (opcion == 2) {
        char vuelo[50];
        cout << "Ingrese numero de vuelo: ";
        cin.getline(vuelo, 50);
        matrizAsignaciones.buscarPorVuelo(vuelo);
    } else if (opcion == 3) {
        matrizAsignaciones.mostrar();
    }
}

/**
 * Generar reportes avanzados (estructuras nuevas)
 */
void generarReportesAvanzados() {
    cout << "\nGenerando reportes avanzados..." << endl;
    
    system("mkdir reportes 2>nul");
    
    // Reconstruir Árbol B con aviones disponibles actuales
    arbolAvionesDisponibles = ArbolB();  // Resetear árbol
    NodoListaCircular* actual = avionesDisponibles.getCabeza();
    if (actual != nullptr) {
        do {
            arbolAvionesDisponibles.insertar(actual->avion);
            actual = actual->siguiente;
        } while (actual != avionesDisponibles.getCabeza());
    }
    
    // Reporte básico: Aviones en mantenimiento
    avionesMantenimiento.generarReporteGraphviz("reportes/aviones_mantenimiento.dot", "Aviones en Mantenimiento");
    Graphviz::generarImagen("reportes/aviones_mantenimiento.dot", "reportes/aviones_mantenimiento.png");
    
    // Arbol B de aviones
    arbolAvionesDisponibles.generarReporteGraphviz("reportes/arbol_b_aviones.dot");
    Graphviz::generarImagen("reportes/arbol_b_aviones.dot", "reportes/arbol_b_aviones.png");
    
    // ABB de pilotos - Reporte general
    arbolPilotos.generarReporteGraphviz("reportes/arbol_pilotos.dot");
    Graphviz::generarImagen("reportes/arbol_pilotos.dot", "reportes/arbol_pilotos.png");
    
    // Tabla Hash
    tablaPilotos.generarReporteGraphviz("reportes/tabla_hash_pilotos.dot");
    Graphviz::generarImagen("reportes/tabla_hash_pilotos.dot", "reportes/tabla_hash_pilotos.png");
    
    // Grafo de rutas
    grafoRutas.generarReporteGraphviz("reportes/grafo_rutas.dot");
    Graphviz::generarImagen("reportes/grafo_rutas.dot", "reportes/grafo_rutas.png");
    
    // Matriz dispersa: resetear y llenar con los pilotos cargados actualmente
    matrizAsignaciones = MatrizDispersa();  // Limpiar matriz anterior
    InsertadorMatriz::matriz = &matrizAsignaciones;
    
    if (!arbolPilotos.estaVacio()) {
        arbolPilotos.aplicarFuncion(InsertadorMatriz::insertar);
    }
    
    matrizAsignaciones.generarReporteGraphviz("reportes/matriz_asignaciones.dot");
    Graphviz::generarImagen("reportes/matriz_asignaciones.dot", "reportes/matriz_asignaciones.png");
    
    cout << "\nReportes avanzados generados." << endl;
    
    cout << "\n¿Desea abrir los reportes? (s/n): ";
    char respuesta;
    cin >> respuesta;
    if (respuesta == 's' || respuesta == 'S') {
        Graphviz::abrirImagen("reportes/aviones_mantenimiento.png");
        Graphviz::abrirImagen("reportes/arbol_b_aviones.png");
        Graphviz::abrirImagen("reportes/arbol_pilotos.png");
        Graphviz::abrirImagen("reportes/tabla_hash_pilotos.png");
        Graphviz::abrirImagen("reportes/grafo_rutas.png");
        Graphviz::abrirImagen("reportes/matriz_asignaciones.png");
    }
}

/**
 * Función principal del programa
 * Implementa un bucle do-while para el menú interactivo
 * Continúa hasta que el usuario seleccione la opción 11 (Salir)
 */
int main() {
    int opcion;
    
    do {
        mostrarMenu();
        
        // Validación robusta de entrada
        // Si el usuario ingresa texto en lugar de número, limpia el buffer y solicita nuevamente
        while (!(cin >> opcion)) {
            cin.clear();                    // Limpiar flag de error del stream
            cin.ignore(10000, '\n');        // Descartar entrada incorrecta
            cout << "Entrada invalida. Por favor ingrese un numero: ";
        }
        cin.ignore();  // Limpiar el salto de línea residual del buffer

        // Ejecutar la opción seleccionada
        switch (opcion) {
            case 1:
                cargarAviones();
                break;
            case 2:
                procesarMovimientos();
                break;
            case 3:
                cargarPilotos();
                break;
            case 4:
                cargarRutas();
                break;
            case 5:
                consultarPilotos();
                break;
            case 6:
                visualizarRecorridos();
                break;
            case 7:
                consultarRutaCorta();
                break;
            case 8:
                mostrarAsignaciones();
                break;
            case 9:
                generarReportesAvanzados();
                break;
            case 10:
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
        }
    } while (opcion != 10);

    // Los destructores de las estructuras liberan automáticamente la memoria
    return 0;
}