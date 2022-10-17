

#include <iostream>
#include <memory>
#include <string>
#include <exception>
#include <boost/program_options.hpp>

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
namespace po = boost::program_options;

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
      database::Author author = database::Author::read_by_id(request->id());
      reply->set_first_name(author.get_first_name());
      reply->set_last_name(author.get_last_name());
      reply->set_email(author.get_email());
      reply->set_title(author.get_title());
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

#include "config/config.h"
auto main(int argc, char *argv[]) -> int
{
  po::options_description desc{"Options"};
  desc.add_options()("help,h", "This screen")
  ("host,", po::value<std::string>()->required(), "set ip for database")
  ("port,", po::value<std::string>()->required(), "databaase port")
  ("login,", po::value<std::string>()->required(), "database login")
  ("password,", po::value<std::string>()->required(), "database password")
  ("database,", po::value<std::string>()->required(), "database name");

  po::variables_map vm;
  po::store(parse_command_line(argc, argv, desc), vm);

  if (vm.count("help"))
    std::cout << desc << '\n';

  if (vm.count("host"))
    Config::get().host() = vm["host"].as<std::string>();
  if (vm.count("port"))
    Config::get().port() = vm["port"].as<std::string>();
  if (vm.count("login"))
    Config::get().login() = vm["login"].as<std::string>();
  if (vm.count("password"))
    Config::get().password() = vm["password"].as<std::string>();
  if (vm.count("database"))
    Config::get().database() = vm["database"].as<std::string>();

  RunServer();

  return 0;
}
