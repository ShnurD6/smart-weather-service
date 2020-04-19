FROM clangbuiltlinux/ubuntu:latest

RUN apt-get update && \
    apt-get install -y \
    libboost-dev \
    cmake

ADD Sources /app/Sources
ADD CMakeLists.txt CitiesToken.api WeatherToken.api /app/

WORKDIR /app/build
RUN cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && make -j4

WORKDIR /app
ENTRYPOINT ./SmartWeatherService