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

  void GetUser(const int id) {
    GetUserRequest request;
    request.set_id(id);


    GetUserReply reply;
    ClientContext context;

    Status status = stub_->GetUser(&context, request, &reply);

    if (status.ok()) {
      std::cout << "id:" << reply.id() << std::endl;
      std::cout << "first_name:" << reply.first_name() << std::endl;
      std::cout << "last_name:" << reply.last_name() << std::endl;
      std::cout << "email:" << reply.email() << std::endl;
      std::cout << "title:" << reply.title() << std::endl;
      std::cout << "login:" << reply.login() << std::endl;
      std::cout << "password:" <<reply.password() << std::endl;
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
  
  UserClient user_stub(
      grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  
  int id{1};

  if(argc>1) {
    id = atoi(argv[1]);
  }
  
  user_stub.GetUser(id);

  return 0;
}
