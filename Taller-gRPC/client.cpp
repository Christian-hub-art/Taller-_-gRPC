// ============================================================
// client.cpp
//
// Cliente gRPC que invoca los 3 procedimientos remotos expuestos
// por server.cpp:
//   GetNombre, GetNotas, GetGrupo
//
// Uso:
//   ./cliente                       -> se conecta a localhost:50051
//   ./cliente <ip_servidor>:50051   -> se conecta a un servidor remoto
//                                       (para la prueba entre 2 computadoras)
// ============================================================

#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "estudiantes.grpc.pb.h" // Generado automáticamente por protoc a partir de estudiantes.proto

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace estudiantes;

// Envuelve el "stub" generado por gRPC (el objeto proxy que representa
// al servicio remoto) y ofrece un método por cada RPC del servicio.
class EstudianteClient {
public:
    // channel: la conexión de red hacia el servidor (host:puerto).
    EstudianteClient(std::shared_ptr<Channel> channel)
        : stub_(EstudianteService::NewStub(channel)) {}

    // Invoca el RPC GetNombre: envía el ID y muestra el nombre recibido.
    void GetNombre(const std::string& id) {
        IdRequest request;
        request.set_id(id);
        NombreResponse reply;
        ClientContext context;

        Status status = stub_->GetNombre(&context, request, &reply);
        if (status.ok()) {
            std::cout << "Nombre para ID " << id << ": " << reply.nombre() << std::endl;
        } else {
            std::cout << "Error: " << status.error_message() << std::endl;
        }
    }

    // Invoca el RPC GetNotas: envía un ID o nombre y muestra el promedio.
    void GetNotas(const std::string& query) {
        ConsultaRequest request;
        request.set_query(query);
        NotasResponse reply;
        ClientContext context;

        Status status = stub_->GetNotas(&context, request, &reply);
        if (status.ok()) {
            std::cout << "Promedio de notas (" << query << "): " << reply.promedio() << std::endl;
        } else {
            std::cout << "Error: " << status.error_message() << std::endl;
        }
    }

    // Invoca el RPC GetGrupo: envía el ID y muestra el grupo recibido.
    void GetGrupo(const std::string& id) {
        IdRequest request;
        request.set_id(id);
        GrupoResponse reply;
        ClientContext context;

        Status status = stub_->GetGrupo(&context, request, &reply);
        if (status.ok()) {
            std::cout << "Grupo para ID " << id << ": " << reply.grupo() << std::endl;
        } else {
            std::cout << "Error: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<EstudianteService::Stub> stub_;
};

int main(int argc, char** argv) {
    // Por defecto se conecta al servidor local.
    // Si pruebas entre dos computadoras, pasa la IP del servidor como argumento,
    // ej.: ./cliente 192.168.1.50:50051
    std::string target = "localhost:50051";
    if (argc > 1) {
        target = argv[1];
    }

    EstudianteClient client(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));

    std::cout << "--- Probando Servicio 1: Nombre ---" << std::endl;
    client.GetNombre("334");

    std::cout << "--- Probando Servicio 2: Notas ---" << std::endl;
    client.GetNotas("334");

    std::cout << "--- Probando Servicio 3: Grupo ---" << std::endl;
    client.GetGrupo("334");

    return 0;
}
