#ifndef GRAPHVIZ_H
#define GRAPHVIZ_H

#include <cstdlib>
#include <iostream>

class Graphviz {
public:
    static void generarImagen(const char* dotFile, const char* pngFile) {
        char comando[512];
        sprintf(comando, "dot -Tpng %s -o %s", dotFile, pngFile);
        
        int resultado = system(comando);
        if (resultado == 0) {
            std::cout << "Reporte generado: " << pngFile << std::endl;
        } else {
            std::cout << "Error al generar la imagen. Asegurese de tener Graphviz instalado." << std::endl;
        }
    }

    static void abrirImagen(const char* pngFile) {
        char comando[512];
        #ifdef _WIN32
            sprintf(comando, "start %s", pngFile);
        #elif __APPLE__
            sprintf(comando, "open %s", pngFile);
        #else
            sprintf(comando, "xdg-open %s", pngFile);
        #endif
        
        system(comando);
    }
};

#endif