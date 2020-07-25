FROM clangbuiltlinux/ubuntu:latest

RUN apt-get update && \
    apt-get install -y \
        libcurl4-openssl-dev \
        libboost-system-dev \
        libboost-date-time-dev \
        cmake && \
#   Install TgBot
    git clone https://github.com/reo7sp/tgbot-cpp && \
    cd tgbot-cpp && \
    cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 && \
    make -j4 && \
    make install && \
    cd .. && \
#   Install CurlForPeople lib
    git clone --recursive https://github.com/whoshuu/cpr.git && \
    cd cpr && \
    cmake . -DBUILD_CPR_TESTS=OFF -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 && \
    make install -j4 && \
#   Clone project
    git clone --recursive https://github.com/ShnurD6/smart-weather-service.git /smart-weather-service

WORKDIR /smart-weather-service/build
RUN cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 && make -j4 TelegramClient

WORKDIR /app
ENTRYPOINT ./TelegramClient