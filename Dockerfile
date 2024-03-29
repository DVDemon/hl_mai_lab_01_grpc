FROM ubuntu
ENV TZ=Europe/Moscow
RUN apt-get clean && apt-get update -y 
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata git sudo build-essential autoconf libtool libssl-dev zlib1g-dev librdkafka-dev pkg-config cmake gcc git g++ mysql-client libmysqlclient-dev libboost-all-dev libssl-dev && apt-get clean
RUN git clone -b master https://github.com/pocoproject/poco.git && cd poco && mkdir cmake-build && cd cmake-build && cmake .. && make && sudo make install && cd .. && cd ..
RUN git clone --recurse-submodules -b v1.43.0 https://github.com/grpc/grpc && cd grpc && mkdir cmake_build && cd cmake_build && cmake -DgRPC_INSTALL=ON  -DgRPC_BUILD_TESTS=OFF .. && make && sudo make install && cd
RUN git clone https://github.com/DVDemon/hl_mai_lab_01_grpc.git && sudo ldconfig &&     cd hl_mai_lab_01_grpc && mkdir build && cd build && cmake  .. && make
ENTRYPOINT [ "hl_mai_lab_01_grpc/build/server" ]