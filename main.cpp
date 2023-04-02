#include <iostream>
#include <memory>
#include <string>
#include <exception>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "user.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using user::GetUserReply;
using user::GetUserRequest;
using user::User;

#include "database/user.h"

std::string server_name{"no_name"};

class UserServiceImpl final : public User::Service
{
  Status SetUser([[maybe_unused]] ServerContext *context,
                 const user::SetUserRequest *request,
                 user::SetUserReply *reply) override
  {
    std::cout << server_name <<": set request login:" << request->login() << std::endl;
    try
    {
      database::User usr;
      usr.first_name() = request->first_name();
      usr.last_name() = request->last_name();
      usr.email() = request->email();
      usr.title() = request->title();
      usr.login() = request->login();
      usr.password() = request->password();
      usr.save_to_mysql();
      reply->set_id(usr.get_id());
      return Status::OK;
    }
    catch (std::exception &ex)
    {
      std::cout << ex.what() << std::endl;
      return Status::CANCELLED;
    }
  }

  Status GetUser([[maybe_unused]] ServerContext *context,
                 const user::GetUserRequest *request,
                 user::GetUserReply *reply) override
  {

    std::cout << server_name <<": get request id:" << request->id() << std::endl;

    try
    {
      auto user = database::User::read_by_id(request->id());
      if (user)
      {
        reply->set_first_name(user->get_first_name());
        reply->set_last_name(user->get_last_name());
        reply->set_email(user->get_email());
        reply->set_title(user->get_title());
        reply->set_id(request->id());
        reply->set_login(user->get_login());
        reply->set_password(user->get_password());

        return Status::OK;
      }
      else
        return Status::CANCELLED;
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
  if(std::getenv("SERVER_NAME")) server_name = std::getenv("SERVER_NAME");

  database::User::init();
  std::string server_address("0.0.0.0:50051");
  

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  
  UserServiceImpl service;
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

#include "config/config.h"
auto main() -> int
{
  RunServer();
  return 0;
}
