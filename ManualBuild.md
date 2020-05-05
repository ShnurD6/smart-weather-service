# Ручная сборка

1. Программа написана на C++ с использованием стандарта C++17. Соотвественно, необходим компилятор, поддерживающий данный стандарт. Лучше всех справляется [clang](https://ps-group.github.io/compilers/llvm_setup).
2. Помимо этого, для работы с сокетами используется библиотека boost, её тоже необходимо [установить](https://www.boost.org/users/download/).
3. Для сборки используется система автоматизации CMake. Тоже придётся [поставить](https://cmake.org/download/)) 
4. Далее, в папке с проектом необходимо выполнить команду `cmake .`
5. Затем, нужно выполить `make -j4`
6. Готово! Вы восхотительны!

# Запуск 

1. Добавить ключ API для сервиса http://api.openweathermap.org необходимо в файл WeatherToken.api в корень проекта
2. Добавить ключ API для сервиса http://api.opencagedata.com/ в файл CitiesToken.api в корень проекта
3. `./SmartWeatherService`