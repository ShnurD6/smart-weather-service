FROM clangbuiltlinux/ubuntu:latest

RUN apt-get update && \
    apt-get install -y \
        libcurl4-openssl-dev \
        libboost-system-dev \
        cmake && \
    git clone https://github.com/reo7sp/tgbot-cpp && \
    cd tgbot-cpp && \
    cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && \
    make -j4 && \
    make install

ADD Sources /app/Sources
ADD CMakeLists.txt CitiesToken.api WeatherToken.api TelegramToken.api /app/

WORKDIR /app/build
RUN cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && make -j4

WORKDIR /app
ENTRYPOINT ./SmartWeatherService