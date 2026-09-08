# Taller de Patrones de Comunicación Cliente-Servidor (gRPC)

Implementación de un servidor remoto (gRPC) que expone 3 procedimientos remotos
sobre datos de estudiantes de un curso, y un cliente que los invoca:

- **GetNombre**: recibe el ID de un estudiante, retorna su nombre completo.
- **GetNotas**: recibe el ID o el nombre de un estudiante, retorna el promedio
  de Taller 1 y Taller 2.
- **GetGrupo**: recibe el ID de un estudiante, retorna su grupo de trabajo.

## Estructura del proyecto

| Archivo               | Contenido                                                      |
|------------------------|------------------------------------------------------------------|
| `estudiantes.proto`    | Define el servicio y los mensajes (contrato cliente-servidor)   |
| `server.cpp`           | Implementación del servidor (procedimientos remotos + datos)    |
| `client.cpp`           | Cliente que invoca los 3 servicios                               |
| `CMakeLists.txt`       | Descarga gRPC, genera código del `.proto` y compila ambos binarios |

## 1. Requisitos

Debes compilar en un entorno **Linux**, ya sea:

- **WSL** (Windows Subsystem for Linux)
- **Ubuntu** nativo
- La **máquina virtual de la universidad**

Instala las dependencias necesarias:

```bash
sudo apt update
sudo apt install -y build-essential cmake git \
    protobuf-compiler libprotobuf-dev libgrpc++-dev protobuf-compiler-grpc
```

## 2. Compilar

Desde la carpeta donde quieras guardar el taller:

```bash
git clone <url-del-repositorio>
cd <carpeta-del-repositorio>
mkdir -p build
cd build
cmake ..
make
```

> La primera vez, `cmake ..` descarga y compila gRPC (vía `FetchContent`),
> así que puede tardar varios minutos. Las siguientes veces será mucho más rápido.

Al terminar tendrás dos ejecutables dentro de `build/`: `servidor` y `cliente`.

## 3. Ejecutar en una sola computadora (prueba rápida)

En una terminal, dentro de `build/`:

```bash
./servidor
```

Debe imprimir:

```
Servidor escuchando en 0.0.0.0:50051
```

Deja esa terminal abierta y, en **otra terminal**, dentro de `build/`:

```bash
./cliente
```

Salida esperada (con el estudiante de ejemplo ID `334`):

```
--- Probando Servicio 1: Nombre ---
Nombre para ID 334: Aguilar,María
--- Probando Servicio 2: Notas ---
Promedio de notas (334): 5
--- Probando Servicio 3: Grupo ---
Grupo para ID 334: G3
```

Puedes probar con otros IDs editando `client.cpp` (o agregando tu propia
llamada a `GetNombre`, `GetNotas` o `GetGrupo`) y recompilando con `make`.

## 4. Ejecutar entre dos computadoras (como pide el taller)

1. Ambas computadoras deben estar en la **misma red** (mismo WiFi/LAN).
2. En la **computadora servidor**, averigua su IP local:
   ```bash
   ip addr show | grep "inet "
   ```
   (busca algo como `192.168.1.50`)
3. En la computadora servidor, corre:
   ```bash
   ./servidor
   ```
4. Si hay firewall activo, abre el puerto 50051 (en Ubuntu con `ufw`):
   ```bash
   sudo ufw allow 50051/tcp
   ```
5. En la **computadora cliente** (ya compilada con los mismos pasos del punto 2),
   corre el cliente indicando la IP del servidor:
   ```bash
   ./cliente 192.168.1.50:50051
   ```
6. Deberías ver la misma salida del punto 3, ahora obtenida desde el servidor remoto.

Esta es la prueba que deben grabar en el video de entrega: mostrar el servidor
corriendo en una máquina y el cliente, en otra máquina, invocando los tres
servicios y recibiendo respuesta.

## 5. Solución de problemas comunes

| Problema | Causa probable | Solución |
|---|---|---|
| `Error: failed to connect to all addresses` | El servidor no está corriendo, o la IP/puerto está mal | Verifica que `./servidor` esté activo y que uses la IP correcta |
| El cliente no conecta entre dos PCs | Firewall bloqueando el puerto 50051 | Abre el puerto con `ufw` o el firewall que uses |
| `cmake ..` falla descargando gRPC | Sin conexión a internet o repo bloqueado | Verifica tu conexión; puede tardar, ten paciencia la primera vez |
| `Estudiante no encontrado` | El ID/nombre no existe en la base de datos de `server.cpp` | Revisa los IDs cargados en el constructor de `EstudianteServiceImpl` |

## 6. Notas de diseño

- La "base de datos" es un `std::map<std::string, Estudiante>` en memoria,
  cargado con datos de ejemplo en el constructor del servidor.
- El servidor escucha en `0.0.0.0:50051`, es decir, en todas las interfaces
  de red del equipo, por eso puede recibir conexiones desde otra computadora.
- Se usan credenciales inseguras (`InsecureChannelCredentials` /
  `InsecureServerCredentials`), sin TLS, adecuado para un entorno académico
  en red local, no para producción.
