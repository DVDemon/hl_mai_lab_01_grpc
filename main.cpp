

#include <iostream>
#include <memory>
#include <string>
#include <exception>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "author.grpc.pb.h"
#endif

using author::Author;
using author::AuthorReply;
using author::AuthorRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

#include "database/author.h"
// Logic and data behind the server's behavior.
class AuthorServiceImpl final : public Author::Service
{
  Status GetAuthor([[maybe_unused]] ServerContext *context,
                   const AuthorRequest *request,
                   AuthorReply *reply) override
  {

    std::cout << "request id:" << request->id() << std::endl;

    try
    {
      database::Author authot = database::Author::read_by_id(request->id());
      reply->set_first_name("Ivan");
      reply->set_last_name("Ivanov");
      reply->set_email("ivanov@yandex.ru");
      reply->set_title("mr");
      reply->set_id(request->id());

      return Status::OK;
    }
    catch (std::exception &ex)
    {
      std::cout << ex.what() << std::endl;
      return Status::CANCELLED;
    }
  }

}; 

void RunServer()
{
  std::string server_address("0.0.0.0:50051");
  AuthorServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

auto main() -> int
{
  RunServer();

  return 0;
}
