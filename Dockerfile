FROM ubuntu:20.04
RUN apt update
RUN apt install -y make cmake g++-multilib
WORKDIR /plugin
CMD cd ./build/linux && cmake ../.. && cd ../.. && cmake --build ./build/linux