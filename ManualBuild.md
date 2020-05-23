# Ручная сборка

1. Программа написана на C++ с использованием стандарта C++17. Соотвественно, необходим компилятор, поддерживающий данный стандарт. Лучше всех справляется [clang](https://ps-group.github.io/compilers/llvm_setup).
2. Помимо стандартной, используется несколько библиотек:
    <details>
      <summary>Boost</summary>
      
      Используется для упрощения работы со строками, опциональностью и временем.
      Установите [по ссылке](https://www.boost.org/users/download/), или установите из своего пакетного менеджера `libboost-system-dev` и `libboost-date-time-dev`
      
    </details>
    <details>
      <summary>Curl for people</summary>
      
      Используется для работы с реквестами на API.
      Необходим пакет `libcurl4-openssl-dev` и установка самой библиотеки:
        
      ```shell script
        git clone --recursive https://github.com/whoshuu/cpr.git && \
        cd cpr && \
        cmake . -DBUILD_CPR_TESTS=OFF -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && \
        sudo make install -j4
      ```
      
    </details>
    <details>
      <summary>TgBot</summary>
      
      Используется для работы с Telegram API.
      Установка:
        
      ```shell script
        git clone https://github.com/reo7sp/tgbot-cpp && \
        cd tgbot-cpp && \
        cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 && \
        make -j4 && \
        sudo make install
      ```
      
    </details>
 
3. Для сборки используется система автоматизации CMake. Тоже придётся [поставить](https://cmake.org/download/)) 
4. Далее, в папке с проектом необходимо выполнить команду `cmake . && make -j4`
5. Готово! Вы восхотительны!

# Запуск 
1. Для работы проекта используется несколько API, для которых нужны токены. Их можно добавить как файлом в корень проекта, так и в env:
    
    | API             | Название файла для добавления в корень проекта  |  Переменная env    |
    | --------------- | :---------------------------------------------: | :----------------: |
    | OpenWeatherMap  | WeatherToken.api                                | SWS_WEATHER_TOKEN  |
    | OpenCageData    | CitiesToken.api                                 | SWS_CITIES_TOKEN   |
    | Telegram        | TelegramToken.api                               | SWS_TELEGRAM_TOKEN |

2. `./SmartWeatherService`