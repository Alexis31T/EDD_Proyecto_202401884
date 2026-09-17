# Manual De Usuario

Proyecto: **ExtreamFS - Sistema de Archivos EXT2/EXT3**  
Carnet: **202401884**  
ID esperado del primer montaje: **841a**

---

## 1. Introduccion

ExtreamFS es una aplicacion web que permite ejecutar comandos para simular la administracion de discos, particiones y sistemas de archivos tipo EXT2/EXT3.

Con la aplicacion se puede:

- Crear discos virtuales.
- Eliminar discos.
- Crear particiones primarias, extendidas y logicas.
- Montar particiones.
- Formatear particiones.
- Iniciar sesion.
- Crear usuarios y grupos.
- Crear carpetas y archivos.
- Leer archivos.
- Generar reportes graficos y de texto.

El sistema se usa desde un navegador web y se comunica con un backend en C++.

---

## 2. Requisitos Previos

Para usar el programa en Linux se recomienda tener instalado:

```bash
sudo apt update
sudo apt install g++ make graphviz
```

Tambien se debe preparar la carpeta donde se crearan discos y reportes:

```bash
sudo mkdir -p /home/archivos/Discos
sudo mkdir -p /home/archivos/reportes
sudo mkdir -p /home/archivos/pruebas
sudo chown -R $USER:$USER /home/archivos
```

> **Captura 1:** Terminal mostrando la instalacion de dependencias y creacion de carpetas.

---

## 3. Como Iniciar El Programa

### 3.1 Abrir Una Terminal

Ubicarse en la carpeta del backend:

```bash
cd MIA_2S2026_P1_202401884/backend
```

### 3.2 Compilar

```bash
make clean
make
```

Si la compilacion es correcta, aparecera un mensaje similar a:

```text
Compilacion completada
```

> **Captura 2:** Terminal mostrando `make clean`, `make` y compilacion exitosa.

### 3.3 Ejecutar El Backend

```bash
./bin/filesystem
```

El backend queda escuchando en el puerto:

```text
8080
```

> **Captura 3:** Terminal con el backend corriendo.

### 3.4 Abrir La Aplicacion

En el navegador abrir:

```text
http://localhost:8080
```

> **Captura 4:** Pantalla principal de ExtreamFS en el navegador.

---

## 4. Pantalla Principal

La interfaz tiene dos secciones principales:

### 4.1 Entrada De Comandos

En esta seccion se escriben o cargan comandos.

Elementos:

- **Cargar Archivo**: permite cargar un archivo `.smia` o `.txt`.
- **Limpiar**: limpia el area de entrada y salida.
- **Textarea de comandos**: area donde se escriben comandos.
- **Ejecutar**: envia los comandos al backend.

> **Captura 5:** Seccion "Entrada de Comandos".

### 4.2 Salida

En esta seccion se muestran:

- comentarios del script.
- comandos ejecutados.
- mensajes de exito.
- mensajes de error.
- links para abrir o descargar reportes.

> **Captura 6:** Seccion "Salida" con comandos ejecutados.

---

## 5. Formas De Ejecutar Comandos

### 5.1 Escribir Comandos Manualmente

Se puede escribir un comando por linea:

```text
mkdisk -size=20 -u=m -path=/home/archivos/Discos/Disco1.dk
fdisk -type=P -u=K -name=Part1 -size=7680 -path=/home/archivos/Discos/Disco1.dk -f=BF
mount -path=/home/archivos/Discos/Disco1.dk -name=Part1
```

Luego presionar:

```text
Ejecutar
```

### 5.2 Cargar Archivo De Comandos

1. Presionar **Cargar Archivo**.
2. Seleccionar un archivo `.smia` o `.txt`.
3. Verificar que el contenido aparezca en el textarea.
4. Presionar **Ejecutar**.

> **Captura 7:** Carga de un archivo `.smia`.

### 5.3 Comentarios En Scripts

Las lineas que empiezan con `#` se toman como comentarios:

```text
# CREACION DE DISCOS
mkdisk -size=20 -u=m -path=/home/archivos/Discos/Disco1.dk
```

En la salida apareceran como:

```text
[COMENTARIO] # CREACION DE DISCOS
```

---

## 6. Flujo Basico De Uso

El flujo recomendado es:

1. Crear disco.
2. Crear particion.
3. Montar particion.
4. Formatear particion.
5. Iniciar sesion.
6. Crear carpetas/archivos/usuarios.
7. Generar reportes.

Ejemplo:

```text
mkdisk -size=20 -u=m -path=/home/archivos/Discos/Disco1.dk
fdisk -type=P -u=K -name=Part1 -size=7680 -path=/home/archivos/Discos/Disco1.dk -f=BF
mount -path=/home/archivos/Discos/Disco1.dk -name=Part1
mkfs -type=fast -id=841a -fs=3fs
login -usr=root -pwd=123 -id=841a
```

> **Captura 8:** Flujo basico ejecutado correctamente.

---

## 7. ID De Montaje

El ID de montaje se genera usando los ultimos dos digitos del carnet.

Carnet:

```text
202401884
```

Ultimos dos digitos:

```text
84
```

Si se monta la primera particion del primer disco, el ID sera:

```text
841a
```

Ejemplos:

| ID | Significado |
|---|---|
| `841a` | Primera particion del primer disco |
| `842a` | Segunda particion del primer disco |
| `841b` | Primera particion del segundo disco |

Cuando el comando `mount` se ejecuta correctamente, la salida indica el ID:

```text
(MOUNT): se ha realizado correctamente el mount -id=841a
```

Ese mismo ID debe usarse en:

- `mkfs`
- `login`
- `rep`
- `unmount`

> **Captura 9:** Salida de `mount` mostrando el ID generado.

---

## 8. Comandos Disponibles

## 8.1 MKDISK

Crea un disco virtual.

Sintaxis:

```text
mkdisk -size=<tamano> -path=<ruta> [-u=<unidad>] [-f=<ajuste>]
```

Ejemplo:

```text
mkdisk -size=20 -u=m -path=/home/archivos/Discos/Disco1.dk
```

Parametros:

| Parametro | Descripcion |
|---|---|
| `-size` | Tamanio del disco |
| `-path` | Ruta donde se creara el disco |
| `-u` | Unidad: `k` o `m` |
| `-f` | Ajuste: `BF`, `FF`, `WF` |

Errores comunes:

- Disco ya existente.
- Ruta sin permisos.
- Size menor o igual a 0.
- Parametro desconocido.

---

## 8.2 RMDISK

Elimina un disco virtual.

Sintaxis:

```text
rmdisk -path=<ruta>
```

Ejemplo:

```text
rmdisk -path=/home/archivos/Discos/Disco1.dk
```

En la aplicacion web la confirmacion se realiza automaticamente.

---

## 8.3 FDISK

Administra particiones.

Crear particion primaria:

```text
fdisk -type=P -u=K -name=Part1 -size=7680 -path=/home/archivos/Discos/Disco1.dk -f=BF
```

Crear extendida:

```text
fdisk -type=E -u=K -name=Extendida -size=4096 -path=/home/archivos/Discos/Disco1.dk -f=WF
```

Crear logica:

```text
fdisk -type=L -u=K -name=Logica1 -size=512 -path=/home/archivos/Discos/Disco1.dk -f=FF
```

Aumentar tamanio:

```text
fdisk -add=128 -u=K -name=Part1 -path=/home/archivos/Discos/Disco1.dk
```

Reducir tamanio:

```text
fdisk -add=-64 -u=K -name=Part1 -path=/home/archivos/Discos/Disco1.dk
```

Eliminar:

```text
fdisk -delete=full -name=Part1 -path=/home/archivos/Discos/Disco1.dk
```

---

## 8.4 MOUNT

Monta una particion en memoria.

Sintaxis:

```text
mount -path=<ruta_disco> -name=<nombre_particion>
```

Ejemplo:

```text
mount -path=/home/archivos/Discos/Disco1.dk -name=Part1
```

Salida esperada:

```text
(MOUNT): se ha realizado correctamente el mount -id=841a
```

---

## 8.5 MOUNTED

Lista las particiones montadas.

```text
mounted
```

---

## 8.6 UNMOUNT

Desmonta una particion.

```text
unmount -id=841a
```

---

## 8.7 MKFS

Formatea una particion montada.

EXT3:

```text
mkfs -type=fast -id=841a -fs=3fs
```

EXT2:

```text
mkfs -type=fast -id=841a -fs=2fs
```

---

## 8.8 LOGIN

Inicia sesion.

Usuario inicial:

```text
login -usr=root -pwd=123 -id=841a
```

---

## 8.9 LOGOUT

Cierra la sesion activa.

```text
logout
```

---

## 8.10 MKGRP

Crea un grupo.

```text
mkgrp -name=devs
```

Requiere sesion root.

---

## 8.11 RMGRP

Elimina un grupo.

```text
rmgrp -name=devs
```

Requiere sesion root.

---

## 8.12 MKUSR

Crea un usuario.

```text
mkusr -user=alex -pass=abc123 -grp=devs
```

Requiere:

- sesion root.
- grupo existente.
- usuario no duplicado.

---

## 8.13 RMUSR

Elimina un usuario.

```text
rmusr -user=alex
```

---

## 8.14 CHGRP

Cambia el grupo de un usuario.

```text
chgrp -user=alex -grp=root
```

---

## 8.15 MKDIR

Crea carpetas.

Crear carpeta:

```text
mkdir -path=/docs
```

Crear padres automaticamente:

```text
mkdir -path=/docs/proyecto/fase1 -p
```

---

## 8.16 MKFILE

Crea archivos.

Archivo con tamanio:

```text
mkfile -path=/docs/proyecto/a.txt -size=20
```

Archivo con contenido desde otro archivo:

```text
mkfile -path=/docs/proyecto/a.txt -cont=/home/archivo.txt
```

Crear padres automaticamente:

```text
mkfile -path=/docs/proyecto/a.txt -size=20 -r
```

---

## 8.17 CAT

Muestra contenido de archivos.

```text
cat -file1=/docs/proyecto/a.txt
```

Varios archivos:

```text
cat -file1=/docs/proyecto/a.txt -file2=/docs/proyecto/b.txt
```

---

## 8.18 REP

Genera reportes.

Sintaxis general:

```text
rep -id=<id> -path=<ruta_salida> -name=<reporte>
```

Ejemplo:

```text
rep -id=841a -path=/home/archivos/reportes/reporte1_sb.jpg -name=sb
```

Reportes disponibles:

| Nombre | Descripcion |
|---|---|
| `mbr` | Reporte del MBR |
| `disk` | Distribucion del disco |
| `inode` | Tabla de inodos |
| `block` | Bloques |
| `bm_inode` | Bitmap de inodos |
| `bm_block` | Bitmap de bloques |
| `tree` | Arbol del sistema de archivos |
| `sb` | Superblock |
| `file` | Contenido de archivo |
| `ls` | Listado de carpeta |
| `journaling` | Journaling EXT3 |

Reporte `file`:

```text
rep -id=841a -path=/home/archivos/reportes/file_users.txt -name=file -path_file_ls=/users.txt
```

Reporte `ls`:

```text
rep -id=841a -path=/home/archivos/reportes/ls_root.jpg -name=ls -path_file_ls=/
```

Cuando el reporte se genera correctamente, aparecen botones:

- **Abrir reporte**
- **Descargar**

> **Captura 10:** Links de abrir/descargar reporte.

---

## 9. Script De Prueba Con Carnet 84

El archivo:

```text
prueba_carnet_84.smia
```

contiene una prueba lista para:

- crear discos.
- crear una particion.
- montar con ID `841a`.
- formatear EXT3.
- generar reportes `sb` y `tree`.
- iniciar sesion root.

Para usarlo:

1. Abrir la aplicacion.
2. Presionar **Cargar Archivo**.
3. Seleccionar `prueba_carnet_84.smia`.
4. Presionar **Ejecutar**.
5. Revisar la salida.
6. Abrir los reportes generados.

> **Captura 11:** Archivo `prueba_carnet_84.smia` cargado.

---

## 10. Reportes Generados

Los reportes se guardan normalmente en:

```text
/home/archivos/reportes
```

Ejemplos:

```text
/home/archivos/reportes/reporte1_sb.jpg
/home/archivos/reportes/reporte1_tree.jpg
```

Para revisar desde terminal:

```bash
ls -lh /home/archivos/reportes
```

> **Captura 12:** Carpeta `/home/archivos/reportes` con reportes generados.

---

## 11. Errores Comunes Y Soluciones

### 11.1 No Se Puede Crear El Disco

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

### 11.2 ID Invalido

Error:

```text
el primer identificador no es valido
```

Causa:

Se uso un ID incorrecto, por ejemplo `581a` o `651a`.

Solucion:

Usar el ID que devuelve `mount`. Para este carnet normalmente:

```text
841a
```

### 11.3 No Se Generan Reportes

Revisar:

- que el comando `rep` no tenga `#` al inicio.
- que el ID sea correcto.
- que Graphviz este instalado.
- que exista `/home/archivos/reportes`.

### 11.4 Puerto 8080 Ocupado

Buscar proceso:

```bash
sudo lsof -i :8080
```

Finalizar:

```bash
kill -9 PID
```

### 11.5 Frontend No Carga

Verificar:

- backend corriendo.
- abrir `http://localhost:8080`.
- ejecutar desde carpeta `backend`.

### 11.6 Error De Conexion En Frontend

Si aparece:

```text
[ERROR CONEXION]
```

Significa que el backend no esta corriendo o no esta disponible en `localhost:8080`.

---

## 12. Recomendaciones De Uso

- Reiniciar el backend antes de una demostracion importante.
- Usar scripts ya probados.
- Revisar siempre el ID que devuelve `mount`.
- Crear carpetas de Linux antes de ejecutar.
- No comentar los comandos `rep` si se quieren generar reportes.
- Instalar Graphviz antes de generar reportes graficos.
- Mantener una terminal abierta con el backend corriendo.

---

## 13. Flujo Recomendado Para Demo

1. Abrir terminal.
2. Crear carpetas de trabajo.
3. Compilar con `make clean && make`.
4. Ejecutar `./bin/filesystem`.
5. Abrir `http://localhost:8080`.
6. Cargar `prueba_carnet_84.smia`.
7. Ejecutar comandos.
8. Mostrar salida correcta.
9. Abrir reporte `sb`.
10. Abrir reporte `tree`.
11. Mostrar archivos en `/home/archivos/reportes`.

> **Captura 13:** Demo completa ejecutada correctamente.

---

## 14. Glosario Basico

| Termino | Significado |
|---|---|
| Disco virtual | Archivo `.dk`, `.dsk` o `.mia` que simula un disco |
| Particion | Seccion dentro del disco |
| Mount | Montaje de particion en memoria |
| ID | Identificador generado al montar |
| EXT2 | Sistema de archivos sin journaling |
| EXT3 | Sistema de archivos con journaling |
| Inodo | Estructura que representa archivo o carpeta |
| Bloque | Unidad donde se guarda contenido |
| Bitmap | Mapa de espacios libres/ocupados |
| Reporte | Archivo generado para visualizar informacion |
| Graphviz | Herramienta usada para convertir `.dot` a imagen |

---

## 15. Cierre

Con este manual el usuario puede:

- instalar dependencias.
- iniciar el backend.
- abrir el frontend.
- cargar scripts.
- ejecutar comandos.
- interpretar IDs.
- generar reportes.
- resolver errores frecuentes.

Para una ejecucion correcta en Linux, lo mas importante es:

```text
crear /home/archivos
usar ID 841a
instalar graphviz
no comentar los comandos rep
```

