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
    cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && \
    make -j4 && \
    make install && \
    cd .. && \
#   Install CurlForPeople lib
    git clone --recursive https://github.com/whoshuu/cpr.git && \
    cd cpr && \
    cmake . -DBUILD_CPR_TESTS=OFF -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && \
    make install -j4

ADD Sources /app/Sources
ADD CMakeLists.txt *.api /app/

WORKDIR /app/build
RUN cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && make -j4 TelegramClient

WORKDIR /app
ENTRYPOINT ./TelegramClient