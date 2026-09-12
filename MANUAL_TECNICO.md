# PROYECTO 1 - SIMULADOR DE SISTEMA DE ARCHIVOS EXT2

**Manual Técnico**

---

## INFORMACIÓN DEL PROYECTO

- **Título**: Proyecto 1 - Simulador de Sistema de Archivos EXT2
- **Estudiante**: Alexis Trujillo
- **Carnet**: 202401884
- **Curso**: MIA - 2S2026
- **Descripción**: Sistema completo para simular operaciones de filesystem EXT2 con interfaz web

---

## TABLA DE CONTENIDOS

1. [Descripción General](#descripción-general)
2. [Arquitectura del Sistema](#arquitectura-del-sistema)
3. [Estructura de Carpetas](#estructura-de-carpetas)
4. [Componentes Principales](#componentes-principales)
5. [Flujo de Ejecución](#flujo-de-ejecución)
6. [Compilación](#compilación)
7. [Uso del Sistema](#uso-del-sistema)
8. [API REST](#api-rest)
9. [Mantenimiento](#mantenimiento)
10. [Solución de Problemas](#solución-de-problemas)

---

## DESCRIPCIÓN GENERAL

Este proyecto implementa un **simulador completo de filesystem EXT2** con:

- **Backend C++**: Procesa comandos de disco, particiones, usuarios, archivos y reportes
- **Frontend HTML/CSS/JavaScript**: Interfaz web para ejecutar comandos
- **Servidor HTTP**: API REST en puerto 8080 que comunica frontend-backend
- **Sistema de Logging**: Registra todas las operaciones en `server.log`
- **Validación robusta**: Valida parámetros en cliente y servidor

### Características principales:

- Creación y eliminación de discos virtuales (MKDISK, RMDISK)
- Particionamiento (FDISK)
- Montaje de particiones (MOUNT, UNMOUNT)
- Creación de filesystems EXT2 (MKFS)
- Gestión de usuarios (LOGIN, LOGOUT, MKGRP, RMGRP, MKUSR, RMUSR)
- Operaciones de archivos (MKDIR, MKFILE, CAT, CHGRP)
- 11 tipos de reportes en formato gráfico/texto
- Interfaz web responsiva
- Descarga de reportes generados

---

## ARQUITECTURA DEL SISTEMA

Navegador (Frontend)
HTML/CSS/JavaScript - Interfaz web en puerto 8080
           |
         HTTP/JSON
           |
Servidor API (Backend C++ - API.cpp)
- Procesa comandos
- Valida parámetros
- Logging de operaciones
- Manejo de excepciones
           |
    Disco Manager
    Usuarios Manager
    Archivos Manager
           |
    Filesystem EXT2

---

## ESTRUCTURA DE CARPETAS

```
MIA_2S2026_P1_202401884/
├── backend/
│   ├── lib/              # Headers (.h)
│   │   ├── structs.h
│   │   ├── disco.h
│   │   ├── mount.h
│   │   ├── filesystem.h
│   │   ├── users.h
│   │   ├── filemanager.h
│   │   ├── report.h
│   │   ├── scanner.h
│   │   ├── shared.h
│   │   ├── api.h          # ← NUEVO
│   │   └── logger.h       # ← NUEVO
│   ├── include/          # Implementaciones (.cpp)
│   │   ├── disco.cpp
│   │   ├── mount.cpp
│   │   ├── filesystem.cpp
│   │   ├── users.cpp
│   │   ├── filemanager.cpp
│   │   ├── report.cpp
│   │   ├── scanner.cpp
│   │   ├── shared.cpp
│   │   ├── api.cpp        # ← NUEVO
│   │   └── logger.cpp     # ← NUEVO
│   ├── src/
│   │   └── main.cpp       # Entry point - inicia servidor HTTP
│   ├── bin/               # Ejecutables compilados
│   └── Makefile
├── frontend/
│   ├── index.html         # Interfaz web
│   ├── style.css          # Estilos (tema oscuro)
│   └── script.js          # Lógica del frontend
├── docs/                  # Documentación
└── .git/                  # Control de versiones
```

---

## COMPONENTES PRINCIPALES

### 1. **API REST (backend/lib/api.h, include/api.cpp)**

Servidor HTTP que procesa comandos. Clase principal que integra todo el sistema.

```cpp
class API {
public:
    void startServer(int port);           // Inicia servidor en puerto 8080
    void handleRequests();                // Loop de aceptación de conexiones
    string executeCommand(vector<string> tokens);  // Procesa comando
private:
    string createVirtualFile(string path, string content);
    string readVirtualFile(string path);
    string changeVirtualFileGroup(string path, string group);
    bool validatePath(string path);
    bool validateSize(string size);
};
```

**Comandos soportados**: MKDISK, RMDISK, FDISK, MOUNT, UNMOUNT, MKFS, LOGIN, LOGOUT, MKGRP, RMGRP, MKUSR, RMUSR, MKDIR, MKFILE, CAT, CHGRP, REP

### 2. **Logger (backend/lib/logger.h, include/logger.cpp)**

Sistema de logging singleton que registra todas las operaciones.

```cpp
class Logger {
public:
    static Logger& getInstance();  // Patrón Singleton
    void info(string message);
    void warning(string message);
    void error(string message);
    void debug(string message);
};
```

**Uso en código**:
```cpp
Logger::getInstance().info("Iniciando servidor en puerto 8080");
Logger::getInstance().error("Error: Archivo no encontrado");
```

### 3. **Frontend (frontend/script.js)**

Interfaz web que se conecta al API y ejecuta comandos.

```javascript
const API_URL = 'http://localhost:8080';

function validarComando(comando) {
    // Valida comando antes de enviar
    // Devuelve {valido: bool, mensaje: string, sugerencia: string}
}

function ejecutarSiguienteComando() {
    // Loop de ejecución secuencial de comandos
    fetch(`${API_URL}/${urlEncodedCommand}`)
        .then(respuesta => respuesta.json())
        .then(datos => {
            // Procesar respuesta JSON
            if (datos.status === 'success') {
                salida.textContent += `[✓] ${datos.message}\n`;
            }
        });
}
```

### 4. **Validación (frontend/script.js)**

Validación de comandos en cliente antes de enviar al servidor.

```javascript
const COMANDOS_VALIDOS = {
    'MKDISK': ['-size', '-u', '-path'],
    'MKDIR': ['-path'],
    'LOGIN': ['-user', '-pass', '-id'],
    // ... más comandos
};

function validarComando(comando) {
    const partes = comando.split(/\s+/);
    const cmdPrincipal = partes[0].toUpperCase();
    
    if (!COMANDOS_VALIDOS.hasOwnProperty(cmdPrincipal)) {
        return {
            valido: false,
            mensaje: `Comando desconocido: ${cmdPrincipal}`,
            sugerencia: `Comandos válidos: ${Object.keys(COMANDOS_VALIDOS).join(', ')}`
        };
    }
    return { valido: true };
}
```

---

## FLUJO DE EJECUCIÓN

### Flujo completo de un comando:

```
1. Usuario escribe comando en navegador
   └─> "MKFILE -path=/archivo.txt"

2. Frontend valida (validarComando)
   └─> Verifica comando y parámetros

3. Frontend envía HTTP GET
   └─> GET http://localhost:8080/MKFILE%20-path%3D%2Farchivo.txt

4. Backend recibe en handleRequests()
   └─> Logger.info("Comando recibido: MKFILE -path=/archivo.txt")

5. parseUrl() extrae comando de URL
   └─> "MKFILE -path=/archivo.txt"

6. split() parsea en tokens
   └─> ["MKFILE", "-path=/archivo.txt"]

7. executeCommand() procesa
   └─> Valida parámetros con validatePath()
   └─> Ejecuta createVirtualFile()
   └─> Logger.info("Ejecutando comando: MKFILE")

8. Devuelve respuesta JSON
   └─> {"status":"success","message":"Archivo creado: /archivo.txt"}

9. Frontend recibe respuesta
   └─> Muestra [✓] Archivo creado: /archivo.txt

10. Logger persiste en server.log
    └─> [2026-09-12 14:30:45] [INFO] Ejecutando comando: MKFILE
```

---

## COMPILACIÓN

### En Linux (recomendado):

```bash
cd backend
make clean      # Limpia archivos compilados previos
make all        # Compila todo
./bin/filesystem  # Ejecuta servidor
```

### Estructura del Makefile:

```makefile
CXX = g++
CXXFLAGS = -Wall -std=c++17

OBJECTS = $(BIN)/main.o $(BIN)/scanner.o $(BIN)/disco.o \
          $(BIN)/mount.o $(BIN)/shared.o $(BIN)/filesystem.o \
          $(BIN)/filemanager.o $(BIN)/report.o $(BIN)/users.o \
          $(BIN)/api.o $(BIN)/logger.o

EXECUTABLE = $(BIN)/filesystem

# Reglas de compilación para cada .cpp
$(BIN)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -I$(LIB) -o $@
```

---

## USO DEL SISTEMA

### 1. **Iniciar servidor**:
```bash
cd backend
./bin/filesystem
```

Salida esperada:
```
[2026-09-12 14:30:45] [INFO] Iniciando servidor API en puerto 8080
[2026-09-12 14:30:45] [INFO] Servidor HTTP escuchando en puerto 8080
```

### 2. **Acceder a interfaz web**:
```
http://localhost:8080
```

### 3. **Ejemplo de secuencia de comandos**:

```
# 1. Crear disco
MKDISK -size=100 -u=M -path=/home/user/disco1.mia

# 2. Crear partición
FDISK -size=50 -unit=M -type=P -fit=FF -path=/home/user/disco1.mia -name=particion1

# 3. Montar partición
MOUNT -path=/home/user/disco1.mia -name=particion1

# 4. Crear filesystem
MKFS -type=ext2 -id=particion1

# 5. Login
LOGIN -user=root -pass=123 -id=particion1

# 6. Crear directorio
MKDIR -path=/home

# 7. Crear archivo
MKFILE -path=/home/archivo.txt

# 8. Leer archivo
CAT -file=/home/archivo.txt

# 9. Generar reporte
REP -name=MBR -id=particion1 -path=/home/reporte.jpg

# 10. Logout
LOGOUT
```

---

## API REST

### Formato de solicitud:

```
GET http://localhost:8080/COMANDO -parametro=valor -parametro=valor
```

### Formato de respuesta:

```json
{
  "status": "success",
  "message": "Descripción del resultado",
  "file": "/ruta/archivo.jpg"  // Opcional, para descargas
}
```

### Ejemplo con curl:

```bash
# Ejecutar comando MKDISK
curl "http://localhost:8080/MKDISK%20-size=100%20-u=M%20-path=/tmp/disco.mia"

# Respuesta
{"status":"success","message":"Disco creado exitosamente"}
```

---

## MANTENIMIENTO

### Agregar nuevo comando:

**Paso 1**: Agregar declaración en `filemanager.h`:
```cpp
void nuevoComando(vector<string> context, Structs::Partition partition, string p);
```

**Paso 2**: Implementar en `filemanager.cpp`:
```cpp
void FileManager::nuevoComando(vector<string> context, Structs::Partition partition, string p) {
    try {
        // Implementación
        shared.response("NUEVOCOMANDO", "Ejecutado exitosamente");
    } catch(exception &e) {
        shared.handler("NUEVOCOMANDO", e.what());
    }
}
```

**Paso 3**: Agregar case en `executeCommand()` en `api.cpp`:
```cpp
} else if (cmd == "NUEVOCOMANDO") {
    if (!apiLogged) {
        return "Debe iniciar sesion primero";
    }
    string p;
    Structs::Partition partition = apiMount.getmount(apiUser.logged.id, &p);
    apiFileManager.nuevoComando(tokens, partition, p);
    return "NUEVOCOMANDO ejecutado";
```

**Paso 4**: Agregar a validación en `script.js`:
```javascript
const COMANDOS_VALIDOS = {
    'NUEVOCOMANDO': ['-parametro1', '-parametro2'],
    // ...
};
```

---

## SOLUCIÓN DE PROBLEMAS

### Problema: "Puerto 8080 en uso"
**Solución**: Cambiar puerto en `main.cpp`:
```cpp
server.startServer(8081);  // Puerto alternativo
```

### Problema: "No se crea archivo"
**Verificar**:
1. Que el usuario esté logueado (LOGIN primero)
2. Que la ruta sea válida (-path=/nombre)
3. Ver `server.log` para error específico

### Problema: "Comando no reconocido"
**Verificar**:
1. Validación en `frontend/script.js` - agregar comando a `COMANDOS_VALIDOS`
2. Agregar case en `executeCommand()` en `api.cpp`
3. Verificar parámetros correctos

### Ver logs:
```bash
tail -f backend/server.log  # Ver logs en tiempo real
```

---

## COMMITS REALIZADOS

| # | Descripción | Cambios |
|---|---|---|
| 1 | Estructura inicial | Carpetas, headers, main.cpp |
| 2 | FILE y LS reportes | Reportes nuevos |
| 3 | MKFILE, CAT, CHGRP | Nuevos comandos |
| 4 | Servidor HTTP/API | Servidor en puerto 8080 |
| 5 | Integración API+Frontend | Frontend conectado |
| 6 | Descarga de reportes | Descargas en interfaz |
| 7 | Validación y errores | Validación robusta |
| 8 | Funcionalidad real archivos | MKFILE, CAT reales |
| 9 | Testing y estabilidad | Logger y excepciones |
| 10 | Manual técnico final | Este documento |

---

## CONCLUSIONES

Este proyecto implementa un sistema completo y funcional de simulación de filesystem EXT2 con:

- Arquitectura limpia separada entre backend (C++) y frontend (HTML/JS)
- API REST bien estructurada y documentada
- Validación robusta en cliente y servidor
- Sistema de logging para debugging futuro
- Manejo de excepciones para estabilidad
- Interfaz web intuitiva y responsiva
- Código modular y fácil de mantener

El código está listo para producción y mantenimiento futuro.

---

**Fin del Manual Técnico**

Alexis Trujillo - 202401884
