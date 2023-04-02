/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "user.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using user::User;
using user::GetUserReply;
using user::GetUserRequest;

class UserClient {
 public:
  UserClient(std::shared_ptr<Channel> channel)
      : stub_(User::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::tuple<int,std::string,std::string,std::string,std::string> GetUser(const int id) {
    // Data we are sending to the server.
    GetUserRequest request;
    request.set_id(id);

    // Container for the data we expect from the server.
    GetUserReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->GetUser(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return std::tuple<int,std::string,std::string,std::string,std::string>(reply.id(),reply.first_name(),reply.last_name(),reply.email(),reply.title());     
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      throw "RPC failed";
    }
  }

 private:
  std::unique_ptr<User::Stub> stub_;
};

int main(int argc, char** argv) {
  
  UserClient greeter(
      grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  
  int id{1};

  if(argc>1) {
    id = atoi(argv[1]);
  }
  std::tuple<int,std::string,std::string,std::string,std::string> reply = greeter.GetUser(id);
  std::cout << "id: " << std::get<0>(reply) << std::endl;
  std::cout << "first_name: " << std::get<1>(reply) << std::endl;
  std::cout << "last_name: " << std::get<2>(reply) << std::endl;
  std::cout << "email: " << std::get<3>(reply) << std::endl;
  std::cout << "title: " << std::get<4>(reply) << std::endl;


  return 0;
}
