// ============================================================
// server.cpp
//
// Implementa el "objeto/servidor remoto" que pide el taller.
// Expone 3 procedimientos remotos (RPC) definidos en estudiantes.proto:
//   1) GetNombre -> nombre completo del estudiante a partir de su ID
//   2) GetNotas  -> promedio de Taller 1 y Taller 2 (busca por ID o nombre)
//   3) GetGrupo  -> grupo de trabajo del estudiante a partir de su ID
//
// La "base de datos" es un std::map en memoria (db) que se llena con los
// datos de la tabla del enunciado en el constructor de EstudianteServiceImpl.
// ============================================================

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <grpcpp/grpcpp.h>
#include "estudiantes.grpc.pb.h" // Generado automáticamente por protoc a partir de estudiantes.proto

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using namespace estudiantes;

// Representa una fila de la tabla de estudiantes del taller.
struct Estudiante {
    std::string id;
    std::string nombre;
    std::string grupo;
    float taller1;
    float taller2;
};

// Implementación concreta del servicio EstudianteService definido en el .proto.
// gRPC genera la clase base "EstudianteService::Service" con métodos virtuales
// (GetNombre, GetNotas, GetGrupo) que aquí se sobreescriben (override).
class EstudianteServiceImpl final : public EstudianteService::Service {
private:
    // "Base de datos" en memoria: clave = ID del estudiante.
    std::map<std::string, Estudiante> db;

public:
    // Carga los datos de ejemplo de la tabla del taller.
    // En un caso real esto podría leerse de un archivo o de una BD real.
    EstudianteServiceImpl() {
        db["334"] = {"334", "Aguilar,María", "G3", 5.0, 5.0};
        db["444"] = {"444", "Rogriiguez, José", "G5", 4.0, 3.0};
        db["111"] = {"111", "Blanco, Alexandra", "G6", 3.0, 4.0};
        db["123"] = {"123", "Bolívar, Elizabeth", "G3", 2.5, 5.0};
        db["102"] = {"102", "Burgos, Daniel", "G6", 5.0, 5.0};
        db["400"] = {"400", "Castro, Gladys", "G5", 4.0, 4.0};
        db["241"] = {"241", "Rincón, Juan", "G4", 2.0, 4.0};
        db["231"] = {"231", "Herrera, Carolina", "G8", 2.0, 2.0};
        db["222"] = {"222", "Dominguez, Marianna", "G1", 2.9, 5.0};
        db["456"] = {"456", "Morales, Santiago", "G7", 3.4, 4.2};
        db["665"] = {"665", "Marrero, Alejandro", "G6", 3.4, 4.1};
        db["126"] = {"126", "Meneses, Enrique", "G7", 5.0, 4.7};
        db["870"] = {"870", "Rodriguez, Andres", "G8", 5.0, 4.8};
        db["45"]  = {"45", "Sanchez, Liliana", "G3", 4.0, 5.0};
        db["2"]   = {"2", "Soto, Diego", "G2", 4.0, 3.8};
        db["887"] = {"887", "Tineo, Luca", "G4", 3.0, 3.8};
        // NOTA: "665" ya existía arriba (Marrero, Alejandro); esta línea lo
        // sobreescribe con Rosales, Juan Daniel. Es un duplicado del dataset
        // original, no un error de este taller, pero conviene saber que solo
        // queda el segundo registro en memoria.
        db["665"] = {"665", "Rosales, Juan Daniel", "G4", 3.8, 4.8};
        db["889"] = {"889", "VARGAS, Teresa", "G2", 3.8, 4.7};
        db["990"] = {"990", "Vasquez Sanchez, Santiago", "G8", 4.8, 4.5};
        db["997"] = {"997", "Vera, Lilia", "G2", 4.7, 5.0};
        db["995"] = {"995", "Cabrales, Elisa", "G7", 4.5, 5.0};
    }

    // Servicio 1: dado un ID, retorna el nombre completo del estudiante.
    Status GetNombre(ServerContext* context, const IdRequest* request, NombreResponse* reply) override {
        if (db.find(request->id()) != db.end()) {
            reply->set_nombre(db[request->id()].nombre);
            return Status::OK;
        }
        return Status(grpc::NOT_FOUND, "Estudiante no encontrado");
    }

    // Servicio 2: dado un ID o un nombre, retorna el promedio de
    // Taller 1 y Taller 2. Recorre el mapa buscando coincidencia por
    // clave (ID) o por el campo nombre.
    Status GetNotas(ServerContext* context, const ConsultaRequest* request, NotasResponse* reply) override {
        std::string query = request->query();
        for (auto const& [id, est] : db) {
            if (id == query || est.nombre == query) {
                float promedio = (est.taller1 + est.taller2) / 2;
                reply->set_promedio(promedio);
                return Status::OK;
            }
        }
        return Status(grpc::NOT_FOUND, "Estudiante no encontrado");
    }

    // Servicio 3: dado un ID, retorna el grupo de trabajo del estudiante.
    Status GetGrupo(ServerContext* context, const IdRequest* request, GrupoResponse* reply) override {
        if (db.find(request->id()) != db.end()) {
            reply->set_grupo(db[request->id()].grupo);
            return Status::OK;
        }
        return Status(grpc::NOT_FOUND, "Estudiante no encontrado");
    }
};

// Levanta el servidor gRPC:
// - Escucha en el puerto 50051 en todas las interfaces de red (0.0.0.0),
//   por eso se puede probar desde otra computadora en la misma red.
// - Usa credenciales inseguras (sin TLS) porque es un taller académico.
void RunServer() {
    std::string server_address("0.0.0.0:50051");
    EstudianteServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Servidor escuchando en " << server_address << std::endl;
    server->Wait(); // Bloquea el hilo principal atendiendo peticiones indefinidamente.
}

int main() {
    RunServer();
    return 0;
}
