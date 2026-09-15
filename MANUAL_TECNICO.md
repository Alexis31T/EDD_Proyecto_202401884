# Manual Tecnico

Proyecto: **MIA 2S2026 - Proyecto 1**  
Carnet usado para IDs de montaje: **202401884**  
Ultimos dos digitos del carnet: **84**

---

## 1. Objetivo Del Sistema

El sistema simula la administracion de discos, particiones y un sistema de archivos tipo EXT2/EXT3. Permite crear discos virtuales, administrar particiones, montar particiones en memoria, formatearlas, crear usuarios, grupos, carpetas, archivos, consultar archivos y generar reportes graficos/textuales.

El proyecto esta dividido en dos partes:

- **Backend C++**: implementa la logica del sistema de archivos, comandos y servidor HTTP.
- **Frontend HTML/CSS/JS**: interfaz para cargar scripts, ejecutar comandos y visualizar respuestas/reportes.

El backend escucha en el puerto:

```text
8080
```

La aplicacion se abre desde:

```text
http://localhost:8080
```

---

## 2. Arquitectura General

### 2.1 Estructura De Carpetas

```text
MIA_2S2026_P1_202401884/
├── backend/
│   ├── Makefile
│   ├── src/
│   │   └── main.cpp
│   ├── lib/
│   │   ├── api.h
│   │   ├── disco.h
│   │   ├── filemanager.h
│   │   ├── filesystem.h
│   │   ├── logger.h
│   │   ├── mount.h
│   │   ├── report.h
│   │   ├── scanner.h
│   │   ├── shared.h
│   │   ├── structs.h
│   │   └── users.h
│   └── include/
│       ├── api.cpp
│       ├── disco.cpp
│       ├── filemanager.cpp
│       ├── filesystem.cpp
│       ├── logger.cpp
│       ├── mount.cpp
│       ├── report.cpp
│       ├── scanner.cpp
│       ├── shared.cpp
│       └── users.cpp
├── frontend/
│   ├── index.html
│   ├── script.js
│   └── style.css
├── CONTEXTO_PARA_LINUX.md
├── prueba_carnet_84.smia
└── MANUAL_TECNICO.md
```

### 2.2 Flujo General

```text
Usuario
  │
  ▼
Frontend
  │  GET /<comando codificado>
  ▼
API C++
  │
  ├── Scanner / Tokenizador
  │
  ├── Disk
  │   ├── mkdisk
  │   ├── rmdisk
  │   └── fdisk
  │
  ├── Mount
  │   ├── mount
  │   ├── unmount
  │   └── mounted
  │
  ├── FileSystem
  │   └── mkfs EXT2/EXT3
  │
  ├── Users
  │   ├── login/logout
  │   ├── grupos
  │   └── usuarios
  │
  ├── FileManager
  │   ├── mkdir
  │   ├── mkfile
  │   ├── cat
  │   └── chgrp
  │
  └── Report
      ├── mbr
      ├── disk
      ├── inode
      ├── block
      ├── bm_inode
      ├── bm_block
      ├── tree
      ├── sb
      ├── file
      ├── ls
      └── journaling
```

---

## 3. Compilacion Y Ejecucion

### 3.1 Dependencias En Linux

```bash
sudo apt update
sudo apt install g++ make graphviz
```

### 3.2 Carpetas De Trabajo

Los scripts usan rutas en `/home/archivos`. Antes de ejecutar:

```bash
sudo mkdir -p /home/archivos/Discos
sudo mkdir -p /home/archivos/reportes
sudo mkdir -p /home/archivos/pruebas
sudo chown -R $USER:$USER /home/archivos
```

### 3.3 Compilar

```bash
cd MIA_2S2026_P1_202401884/backend
make clean
make
```

### 3.4 Ejecutar

```bash
./bin/filesystem
```

Abrir:

```text
http://localhost:8080
```

> Importante: ejecutar desde la carpeta `backend`, ya que la API sirve el frontend desde `../frontend`.

---

## 4. Makefile

El `Makefile` genera todos los objetos dentro de `backend/bin`.

Fragmento principal:

```makefile
CXX ?= g++
CXXFLAGS ?= -Wall -std=c++14
LDLIBS =

ifeq ($(OS),Windows_NT)
LDLIBS += -lws2_32
endif

SRC = src
INC = include
LIB = lib
BIN = bin

OBJECTS = $(BIN)/main.o $(BIN)/scanner.o $(BIN)/disco.o $(BIN)/mount.o \
          $(BIN)/shared.o $(BIN)/filesystem.o $(BIN)/filemanager.o \
          $(BIN)/report.o $(BIN)/users.o $(BIN)/api.o $(BIN)/logger.o

EXECUTABLE = $(BIN)/filesystem
```

En Linux no se enlaza `ws2_32`; esa libreria solo se usa en Windows.

Regla de enlace:

```makefile
$(EXECUTABLE): $(OBJECTS) | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Compilacion completada"
```

---

## 5. Punto De Entrada

Archivo:

```text
backend/src/main.cpp
```

Codigo:

```cpp
#include <iostream>
#include "../lib/api.h"

int main() {
    API server;
    server.startServer(8080);
    return 0;
}
```

El programa crea una instancia de `API` y arranca el servidor HTTP en el puerto `8080`.

---

## 6. Modulos Del Backend

### 6.1 API

Archivos:

```text
backend/lib/api.h
backend/include/api.cpp
```

Responsabilidades:

- Levantar servidor HTTP.
- Servir frontend.
- Recibir comandos desde el navegador.
- Decodificar URL.
- Ejecutar comandos.
- Devolver JSON.
- Servir reportes generados.

Interfaz principal:

```cpp
class API {
public:
    API();
    void startServer(int port);
    void handleRequests();
    string processCommand(string command);
    string executeCommand(string rawCommand);
    string getLastReportPath();
private:
    int serverPort;
    int serverSocket;
    string lastReportPath;
    string parseUrl(string request);
    string dispatch(string cmd, vector<string> params);
    string cleanOutput(string text);
    string jsonEscape(string text);
    string getCommandName(string command);
    string getFileName(string filePath);
    string contentTypeForPath(string filePath);
    bool looksLikeError(string text);
    string buildJsonResponse(string status, string message);
    string buildJsonResponseWithFile(string status, string message, string filePath);
    string serveFile(string filePath);
};
```

Fragmento del despacho de comandos:

```cpp
string API::dispatch(string cmd, vector<string> tokens) {
    if (cmd == "MKDISK") {
        apiDisk.mkdisk(tokens);
        return "MKDISK ejecutado";
    } else if (cmd == "RMDISK") {
        apiDisk.rmdisk(tokens);
        return "RMDISK ejecutado";
    } else if (cmd == "FDISK") {
        apiDisk.fdisk(tokens);
        return "FDISK ejecutado";
    } else if (cmd == "MOUNT") {
        apiMount.mount(tokens);
        return "MOUNT ejecutado";
    } else if (cmd == "UNMOUNT") {
        apiMount.unmount(tokens);
        return "UNMOUNT ejecutado";
    } else if (cmd == "MKFS") {
        apiFileSystem->mkfs(tokens);
        return "MKFS ejecutado";
    }
    // ...
}
```

La API captura la salida de `cout`, limpia codigos ANSI y genera JSON:

```json
{
  "status": "success",
  "message": "..."
}
```

Para reportes:

```json
{
  "status": "success",
  "message": "...",
  "file": "/home/archivos/reportes/reporte.jpg"
}
```

---

### 6.2 Scanner

Archivos:

```text
backend/lib/scanner.h
backend/include/scanner.cpp
```

Responsabilidades:

- Tokenizar comandos.
- Respetar parametros con comillas.
- Comparar comandos sin distinguir mayusculas/minusculas.
- Ejecutar comandos en modo consola.
- En modo API, confirmar automaticamente operaciones destructivas.

Interfaz:

```cpp
class scanner {
public:
    static bool sinInteraccion;

    scanner();
    void start();
    void functions(string token, vector<string> tks);
    string token(string text);
    vector<string> split(string text, string text_split);
    vector<string> split_tokens(string text);
    bool compare(string a, string b);
    string upper(string a);
    void errores(string operacion, string mensaje);
    void respuesta(string operacion, string mensaje);
    void funcion_excec(vector<string> tokens);
    void excec(string path);
    bool confirmar(string mensaje);
};
```

Punto importante:

```cpp
static bool sinInteraccion;
```

Cuando la API esta activa, `sinInteraccion` evita que `rmdisk` o `fdisk -delete` bloqueen esperando una respuesta por consola.

---

### 6.3 Disk

Archivos:

```text
backend/lib/disco.h
backend/include/disco.cpp
```

Responsabilidades:

- Crear discos.
- Eliminar discos.
- Crear particiones primarias, extendidas y logicas.
- Eliminar particiones.
- Aumentar/reducir particiones.
- Leer MBR y EBR.
- Aplicar teoria de particiones.

Interfaz:

```cpp
class Disk {
public:
    typedef struct _Transition {
        int partition;
        int start;
        int end;
        int before;
        int after;
    } Transition;

    void mkdisk(vector<string> tokens);
    void makeDisk(string s, string f, string u, string p);
    void rmdisk(vector<string> context);
    void fdisk(vector<string> context);
    void generatepartition(string s, string u, string p, string t, string f, string n, string a);
    void deletepartition(string d, string p, string n);
    void addpartition(string add, string u, string n, string p);
    vector<Structs::Partition> getPartitions(Structs::MBR disk);
    Structs::Partition findby(Structs::MBR mbr, string name, string path);
    void logic(Structs::Partition partition, Structs::Partition ep, string p);
    vector<Structs::EBR> getlogics(Structs::Partition partition, string p);
};
```

#### MKDISK

Parametros:

- `-size`
- `-u`
- `-f`
- `-path`

Validaciones:

- `size` obligatorio.
- `path` obligatorio.
- `size > 0`.
- extensiones permitidas: `.mia`, `.dsk`, `.dk`.
- parametros desconocidos generan error.

Fragmento relevante:

```cpp
int size = stoi(s);
if (size <= 0) {
    scan.errores("MKDISK", "size debe ser mayor a 0");
    return;
}
```

Creacion fisica:

```cpp
shared.mkdirs(path);
FILE *file = fopen(path.c_str(), "w+b");
if (file == NULL) {
    scan.errores("MKDISK", "no se pudo crear el archivo en la ruta " + path + ": " + string(strerror(errno)));
    return;
}
```

#### RMDISK

Parametros:

- `-path`

Valida que el archivo exista y elimina el disco. En modo servidor HTTP confirma automaticamente.

#### FDISK

Parametros principales:

- `-size`
- `-u`
- `-path`
- `-type`
- `-f`
- `-name`
- `-delete`
- `-add`

Soporta:

- particiones primarias.
- particion extendida.
- particiones logicas.
- ajuste `BF`, `FF`, `WF`.
- `delete=fast/full`.
- `add` positivo/negativo.

Restricciones:

- maximo 4 entradas en MBR.
- solo una extendida.
- logicas solo dentro de extendida.
- no se sobrepasa el espacio de la particion extendida.
- no montar ni formatear extendidas.

---

### 6.4 Mount

Archivos:

```text
backend/lib/mount.h
backend/include/mount.cpp
```

Responsabilidades:

- Montar particiones en memoria RAM.
- Generar IDs de montaje.
- Desmontar particiones.
- Listar montajes.
- Obtener particion montada por ID.

Estructuras internas:

```cpp
typedef struct _MP {
    char letter;
    char status = '0';
    char name[20];
} MountedPartition;

typedef struct _MD {
    char path[150];
    char status = '0';
    MountedPartition mpartitions[26];
} MountedDisc;

static MountedDisc mounted[99];
```

La tabla `mounted` es estatica para que todos los modulos compartan el mismo estado de montajes.

#### Formula Del ID

El ID usa el carnet:

```text
ultimos dos digitos del carnet + numero de particion + letra de disco
```

Para carnet:

```text
202401884
```

Ultimos dos digitos:

```text
84
```

Ejemplos:

```text
841a -> primera particion del primer disco montado
842a -> segunda particion del primer disco montado
841b -> primera particion del segundo disco montado
```

---

### 6.5 FileSystem

Archivos:

```text
backend/lib/filesystem.h
backend/include/filesystem.cpp
```

Responsabilidades:

- Formatear una particion montada.
- Crear estructura EXT2.
- Crear estructura EXT3 con journaling.
- Inicializar superblock, bitmaps, inodos y bloques.
- Crear `/users.txt`.

Interfaz:

```cpp
class FileSystem {
public:
    FileSystem(Mount m);

    void mkfs(vector<string> context);
    void mkfs(string id, string t, string fs);
    void ext2(Structs::Superblock spr, Structs::Partition p, int n, string path);
    void ext3(Structs::Superblock spr, Structs::Partition p, int n, string path);
private:
    Mount mount;
    Shared shared;
};
```

#### EXT2

Incluye:

- Superblock.
- Bitmap de inodos.
- Bitmap de bloques.
- Tabla de inodos.
- Tabla de bloques.
- Inodo raiz.
- Inodo `/users.txt`.
- Bloque de carpeta raiz.
- Bloque de archivo `users.txt`.

#### EXT3

Incluye lo anterior mas:

- Area de journaling.
- Registro inicial de creacion de `/`.
- Registro inicial de creacion de `/users.txt`.

---

### 6.6 FileManager

Archivos:

```text
backend/lib/filemanager.h
backend/include/filemanager.cpp
```

Responsabilidades:

- Crear carpetas.
- Crear archivos.
- Leer archivos.
- Cambiar grupo de usuario en `users.txt`.
- Manejar apuntadores directos e indirectos.
- Validar permisos.
- Registrar journaling de operaciones de archivos/carpetas.

Interfaz:

```cpp
class FileManager {
public:
    void mkdir(vector<string> context, Structs::Partition partition, string p);
    void mkfile(vector<string> context, Structs::Partition partition, string p);
    void cat(vector<string> context, Structs::Partition partition, string p);
    void chgrp(vector<string> context, Structs::Partition partition, string p);

    vector<string> getpath(string s);
    int getfree(Structs::Superblock spr, string pth, string t);
    void updatebm(Structs::Superblock spr, string pth, string t);
    void setOwner(int uid, int gid);

    int reservarBloque(Structs::Superblock spr, string pth);
    int asignarBloque(FILE *f, Structs::Superblock spr, string pth, Structs::Inodes &inode, int n);
    int bloqueDe(FILE *f, Structs::Superblock spr, Structs::Inodes inode, int n);
    string leerContenido(FILE *f, Structs::Superblock spr, Structs::Inodes inode);
};
```

#### Apuntadores De Archivo

El inodo maneja:

- `i_block[0..11]`: directos.
- `i_block[12]`: simple indirecto.
- `i_block[13]`: doble indirecto.
- `i_block[14]`: triple indirecto.

Constantes internas:

```cpp
static const int APUNT = 16;
static const int DIRECTOS = 12;
static const int CAP_SIMPLE = APUNT;
static const int CAP_DOBLE = APUNT * APUNT;
static const int CAP_TRIPLE = APUNT * APUNT * APUNT;
```

Funcion clave:

```cpp
int FileManager::asignarBloque(FILE *f, Structs::Superblock spr, string pth,
                               Structs::Inodes &inode, int n);
```

Esta funcion recibe el numero logico de bloque de un archivo y devuelve el bloque fisico correspondiente, reservando bloques de apuntadores cuando es necesario.

#### Permisos

Se validan permisos UGO:

- usuario propietario.
- grupo propietario.
- otros.

Reglas:

- root puede realizar operaciones.
- `mkdir` y `mkfile` requieren escritura en carpeta padre.
- `cat` requiere lectura sobre el archivo.

---

### 6.7 Users

Archivos:

```text
backend/lib/users.h
backend/include/users.cpp
```

Responsabilidades:

- Login.
- Logout.
- Crear grupos.
- Eliminar grupos.
- Crear usuarios.
- Eliminar usuarios.
- Cambiar grupo de usuario.
- Leer y escribir `/users.txt`.

Interfaz:

```cpp
class Users {
public:
    typedef struct _User {
        string user;
        string password;
        string id;
        int uid;
        int gid = 1;
        string group;
    } User;

    bool login(vector<string> context, Mount m);
    bool login(string u, string p, string id);
    bool logout();
    void grp(vector<string> context, string action);
    void usr(vector<string> context, string action);
    void mkgrp(string n);
    void rmgrp(string n);
    void mkusr(string usr, string pwd, string grp);
    void rmusr(string usr);
    void chgrp(vector<string> context);
    void chgrp(string usr, string grp);
    string leerUsers(Structs::Superblock super, FILE *f);
    bool escribirUsers(Structs::Superblock super, string pth, FILE *f, string contenido);

    User logged;
};
```

Archivo `users.txt` inicial:

```text
1,G,root
1,U,root,root,123
```

Validaciones:

- solo root administra usuarios y grupos.
- usuario, password y grupo maximo 10 caracteres.
- grupo debe existir y no estar eliminado.
- no se permiten duplicados activos.
- lineas eliminadas se marcan con `0`.

---

### 6.8 Report

Archivos:

```text
backend/lib/report.h
backend/include/report.cpp
```

Responsabilidades:

- Generar reportes del disco y sistema de archivos.
- Crear archivos `.dot`.
- Ejecutar Graphviz.
- Guardar imagenes/textos.
- Servir reportes al frontend mediante API.

Interfaz:

```cpp
class Report {
public:
    void generar(vector<string> context, Mount m);
    void mbr(string p, string id);
    void dks(string p, string id);
    void tree(string p, string id);
    void inode(string p, string id);
    void block(string p, string id);
    void bminode(string p, string id);
    void bmblock(string p, string id);
    void sb(string p, string id);
    void journaling(string p, string id);
    void file(string p, string id, string path_file);
    void ls(string p, string id, string path_folder);
};
```

Reportes implementados:

| Reporte | Descripcion |
|---|---|
| `mbr` | MBR y EBR del disco |
| `disk` | Distribucion grafica del disco |
| `inode` | Tabla de inodos |
| `block` | Bloques usados |
| `bm_inode` | Bitmap de inodos |
| `bm_block` | Bitmap de bloques |
| `tree` | Arbol de inodos y bloques |
| `sb` | Superblock |
| `file` | Contenido de archivo |
| `ls` | Listado de carpeta |
| `journaling` | Registros EXT3 |

Formatos admitidos:

- `jpg`
- `jpeg`
- `png`
- `pdf`
- `svg`
- `txt`

---

### 6.9 Shared

Archivos:

```text
backend/lib/shared.h
backend/include/shared.cpp
```

Responsabilidades:

- Convertir texto a mayusculas/minusculas.
- Comparar strings sin distinguir mayusculas/minusculas.
- Crear carpetas padre.
- Mostrar mensajes comunes.

Funcion importante:

```cpp
void Shared::mkdirs(string filePath)
```

Crea las carpetas padre antes de crear discos o reportes.

---

### 6.10 Logger

Archivos:

```text
backend/lib/logger.h
backend/include/logger.cpp
```

Responsabilidades:

- Registrar eventos.
- Registrar errores.
- Apoyar depuracion del backend.

Archivo generado:

```text
backend/server.log
```

Este archivo es temporal y no es necesario para entrega.

---

## 7. Estructuras Del Sistema

Archivo:

```text
backend/lib/structs.h
```

### 7.1 Partition

```cpp
typedef struct _Partition {
    char part_status = '0';
    char part_type;
    char part_fit;
    int part_start = -1;
    int part_size = 0;
    char part_name[16];
} Partition;
```

Uso:

- entradas del MBR.
- particiones primarias.
- particion extendida.

Campos:

| Campo | Descripcion |
|---|---|
| `part_status` | Indica si esta activa/montada |
| `part_type` | `P`, `E`, `L` |
| `part_fit` | `B`, `F`, `W` |
| `part_start` | Byte inicial |
| `part_size` | Tamanio en bytes |
| `part_name` | Nombre maximo de 16 caracteres |

### 7.2 MBR

```cpp
typedef struct _MBR {
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    Partition mbr_Partition_1;
    Partition mbr_Partition_2;
    Partition mbr_Partition_3;
    Partition mbr_Partition_4;
} MBR;
```

El MBR se escribe al inicio del disco virtual y contiene hasta cuatro particiones.

### 7.3 EBR

```cpp
typedef struct _EBR {
    char part_status = '0';
    char part_fit;
    int part_start;
    int part_size = 0;
    int part_next = -1;
    char part_name[16];
} EBR;
```

El EBR representa particiones logicas dentro de una extendida. Usa `part_next` para enlazar el siguiente EBR.

### 7.4 Inodes

```cpp
typedef struct _Inodes {
    int i_uid = -1;
    int i_gid = -1;
    int i_size = -1;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    char i_type = -1;
    int i_perm = -1;
} Inodes;
```

Campos:

| Campo | Descripcion |
|---|---|
| `i_uid` | Usuario propietario |
| `i_gid` | Grupo propietario |
| `i_size` | Tamanio del archivo |
| `i_atime` | Ultimo acceso |
| `i_ctime` | Creacion |
| `i_mtime` | Modificacion |
| `i_block` | Apuntadores a bloques |
| `i_type` | `0` carpeta, `1` archivo |
| `i_perm` | Permisos UGO |

### 7.5 Content

```cpp
typedef struct _Content {
    char b_name[12];
    int b_inodo = -1;
} Content;
```

Representa una entrada dentro de un bloque de carpeta.

### 7.6 Folderblock

```cpp
typedef struct _Folderblock {
    Content b_content[4];
} Folderblock;
```

Cada bloque de carpeta contiene 4 entradas.

### 7.7 Fileblock

```cpp
typedef struct _Fileblock {
    char b_content[64];
} Fileblock;
```

Cada bloque de archivo almacena 64 bytes.

### 7.8 Pointerblock

```cpp
typedef struct _Pointerblock {
    int b_pointers[16] = {-1, -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1, -1};
} Pointerblock;
```

Usado para apuntadores indirectos.

### 7.9 Superblock

```cpp
typedef struct _Superblock {
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic = 0xEF53;
    int s_inode_size = sizeof(Inodes);
    int s_block_size = sizeof(Folderblock);
    int s_fist_ino = 0;
    int s_first_blo = 0;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
} Superblock;
```

Contiene la metadata principal del sistema de archivos.

### 7.10 Journaling

```cpp
typedef struct _Journaling {
    char operation[10] = "";
    char type = -1;
    char path[100] = "";
    char content[60] = "";
    time_t date;
    int size = 0;
} Journaling;
```

Usado en EXT3 para registrar operaciones.

---

## 8. Comandos Implementados

### 8.1 MKDISK

Ejemplo:

```text
mkdisk -size=20 -u=m -path=/home/archivos/Discos/Disco1.dk
```

Parametros:

| Parametro | Obligatorio | Descripcion |
|---|---|---|
| `-size` | Si | Tamanio del disco |
| `-path` | Si | Ruta del disco |
| `-u` | No | `K` o `M`; por defecto `M` |
| `-f` | No | `BF`, `FF`, `WF`; por defecto `FF` |

Errores manejados:

- size negativo o cero.
- path faltante.
- extension invalida.
- parametro desconocido.
- disco ya existente.
- ruta sin permisos.

### 8.2 RMDISK

Ejemplo:

```text
rmdisk -path=/home/archivos/Discos/Disco1.dk
```

Elimina un disco virtual.

Errores:

- path faltante.
- disco inexistente.
- extension invalida.
- no se puede eliminar.

### 8.3 FDISK

Ejemplo:

```text
fdisk -type=P -u=K -name=Part1 -size=7680 -path=/home/archivos/Discos/Disco1.dk -f=BF
```

Parametros:

| Parametro | Descripcion |
|---|---|
| `-size` | Tamanio de particion |
| `-path` | Ruta del disco |
| `-name` | Nombre de particion |
| `-u` | Unidad `B`, `K`, `M` |
| `-type` | `P`, `E`, `L` |
| `-f` | `BF`, `FF`, `WF` |
| `-delete` | `fast` o `full` |
| `-add` | Aumenta o reduce tamanio |

### 8.4 MOUNT

Ejemplo:

```text
mount -path=/home/archivos/Discos/Disco1.dk -name=Part1
```

Genera un ID en RAM.

Para carnet `202401884`, el primer mount genera:

```text
841a
```

### 8.5 UNMOUNT

Ejemplo:

```text
unmount -id=841a
```

Desmonta una particion de la tabla en RAM.

### 8.6 MOUNTED

Ejemplo:

```text
mounted
```

Lista particiones montadas.

### 8.7 MKFS

Ejemplo:

```text
mkfs -type=fast -id=841a -fs=3fs
```

Parametros:

| Parametro | Descripcion |
|---|---|
| `-id` | ID de particion montada |
| `-type` | `fast` o `full` |
| `-fs` | `2fs` o `3fs` |

### 8.8 LOGIN

Ejemplo:

```text
login -usr=root -pwd=123 -id=841a
```

### 8.9 LOGOUT

```text
logout
```

### 8.10 MKGRP

```text
mkgrp -name=devs
```

### 8.11 RMGRP

```text
rmgrp -name=devs
```

### 8.12 MKUSR

```text
mkusr -user=alex -pass=abc123 -grp=devs
```

### 8.13 RMUSR

```text
rmusr -user=alex
```

### 8.14 CHGRP

```text
chgrp -user=alex -grp=root
```

### 8.15 MKDIR

```text
mkdir -path=/docs/proyecto -p
```

### 8.16 MKFILE

```text
mkfile -path=/docs/proyecto/a.txt -size=20
mkfile -path=/docs/proyecto/b.txt -cont=/home/archivo.txt
mkfile -path=/docs/proyecto/c.txt -size=1000 -r
```

### 8.17 CAT

```text
cat -file1=/docs/proyecto/a.txt -file2=/docs/proyecto/b.txt
```

### 8.18 REP

```text
rep -id=841a -path=/home/archivos/reportes/reporte_sb.jpg -name=sb
```

Reportes:

```text
mbr
disk
inode
block
bm_inode
bm_block
tree
sb
file
ls
journaling
```

Para `file` y `ls` se usa:

```text
-path_file_ls
```

Ejemplos:

```text
rep -id=841a -path=/home/archivos/reportes/file_users.txt -name=file -path_file_ls=/users.txt
rep -id=841a -path=/home/archivos/reportes/ls_root.jpg -name=ls -path_file_ls=/
```

---

## 9. Frontend

### 9.1 index.html

Define:

- textarea de entrada.
- input de archivo.
- boton para cargar archivo.
- boton para ejecutar.
- area de salida.

### 9.2 script.js

Responsabilidades:

- leer archivo `.smia`.
- separar lineas.
- ignorar comentarios.
- enviar comandos a la API.
- mostrar respuestas.
- mostrar links de reportes.

Fragmento de envio:

```javascript
fetch(`${API_URL}/${encodeURIComponent(comando)}`)
    .then((respuesta) => respuesta.json())
    .then((datos) => {
        if (datos.status === 'success') {
            appendLine(`[OK] ${datos.message}`, 'ok');
            if (datos.file) {
                appendReportLinks(datos.file);
            }
        } else {
            appendLine(`[ERROR] ${datos.message}`, 'error');
        }
        ejecutarSiguienteComando();
    });
```

### 9.3 style.css

Define estilos para:

- layout.
- textarea.
- salida.
- botones.
- links de reportes.
- estados de exito/error.

---

## 10. Reportes Y Graphviz

Para reportes graficos se genera un archivo `.dot` temporal y se procesa con Graphviz.

Requisito:

```bash
sudo apt install graphviz
```

Verificar:

```bash
dot -V
```

El sistema elimina los `.dot` temporales despues de generar el reporte.

---

## 11. Permisos UGO

Los permisos siguen la idea:

```text
User Group Other
```

Ejemplo:

```text
664
```

Interpretacion:

| Digito | Permisos |
|---|---|
| 7 | lectura, escritura, ejecucion |
| 6 | lectura, escritura |
| 5 | lectura, ejecucion |
| 4 | lectura |
| 0 | sin permisos |

Reglas implementadas:

- root puede realizar operaciones.
- si el usuario activo es owner, se usa el primer digito.
- si pertenece al mismo grupo, se usa el segundo.
- si no, se usa el tercero.

---

## 12. Journaling EXT3

EXT3 registra operaciones en una zona posterior al superblock.

Estructura:

```cpp
typedef struct _Journaling {
    char operation[10] = "";
    char type = -1;
    char path[100] = "";
    char content[60] = "";
    time_t date;
    int size = 0;
} Journaling;
```

Operaciones registradas:

- `mkdir`
- `mkfile`
- `mkgrp`
- `rmgrp`
- `mkusr`
- `rmusr`
- `chgrp`

---

## 13. Mantenimiento

### 13.1 Agregar Un Nuevo Comando

1. Crear metodo en el modulo correspondiente.
2. Declararlo en el `.h`.
3. Implementarlo en el `.cpp`.
4. Agregar caso en `API::dispatch`.
5. Si aplica, agregar caso en `scanner::functions`.
6. Agregar validacion de parametros.
7. Agregar pruebas en script `.smia`.

Ejemplo en API:

```cpp
} else if (cmd == "NUEVO") {
    modulo.nuevo(tokens);
    return "NUEVO ejecutado";
}
```

### 13.2 Agregar Un Nuevo Reporte

1. Declarar metodo en `report.h`.
2. Implementar metodo en `report.cpp`.
3. Agregar nombre en `Report::generar`.
4. Crear `.dot` si es grafico.
5. Usar `shared.mkdirs(path)` antes de escribir.
6. Ejecutar Graphviz.
7. Eliminar `.dot` temporal.

### 13.3 Cambiar Estructuras

Si se cambia `Structs`:

1. Ejecutar `make clean`.
2. Ejecutar `make`.
3. Regenerar discos de prueba.

Motivo: los binarios de disco dependen del tamanio exacto de las estructuras.

### 13.4 Depurar Errores

Revisar:

```text
backend/server.log
```

Tambien se puede ver la salida directa de consola donde corre `./bin/filesystem`.

---

## 14. Errores Comunes

### 14.1 No Puede Crear Discos En Linux

Error:

```text
No such file or directory
```

Solucion:

```bash
sudo mkdir -p /home/archivos/Discos
sudo mkdir -p /home/archivos/reportes
sudo chown -R $USER:$USER /home/archivos
```

### 14.2 El ID No Coincide

Si el mount responde:

```text
id=841a
```

usar ese ID en:

```text
mkfs
login
rep
```

No usar IDs de ejemplos ajenos como `581a` o `651a`.

### 14.3 No Genera Reportes

Revisar:

- que el comando `rep` no este comentado con `#`.
- que el ID sea correcto.
- que Graphviz este instalado.
- que exista `/home/archivos/reportes`.

### 14.4 Puerto 8080 Ocupado

```bash
sudo lsof -i :8080
kill -9 PID
```

---

## 15. Script De Prueba Basico Con Carnet 84

Archivo:

```text
prueba_carnet_84.smia
```

Flujo:

1. Limpia discos previos.
2. Crea discos.
3. Crea particion primaria.
4. Monta particion.
5. Formatea EXT3.
6. Genera reportes `sb` y `tree`.
7. Inicia sesion root.

ID esperado:

```text
841a
```

---

## 16. Estimacion De Cobertura De Rubrica

| Parte | Maximo | Estado |
|---|---:|---|
| Aplicacion web | 5 | Cubierta |
| Discos, particiones, mount | 37 | Cubierta |
| MKFS y CAT | 7 | Cubierta |
| Usuarios y grupos | 5 | Cubierta |
| Carpetas, archivos, permisos, reportes | 36 | Cubierta |
| Documentacion y preguntas | 10 | Depende de exposicion/manual |

Rango estimado si corre correctamente en Linux:

```text
96 - 100
```

---

## 17. Recomendaciones Para Presentacion

1. Usar Linux fisico.
2. Instalar `g++`, `make` y `graphviz`.
3. Crear `/home/archivos` con permisos del usuario.
4. Ejecutar `make clean && make`.
5. Correr `./bin/filesystem`.
6. Abrir `http://localhost:8080`.
7. Cargar `prueba_carnet_84.smia` o la prueba final.
8. Mostrar reportes generados.
9. Explicar:
   - MBR.
   - EBR.
   - Superblock.
   - Bitmaps.
   - Inodos.
   - Bloques.
   - Journaling.
   - Permisos UGO.

