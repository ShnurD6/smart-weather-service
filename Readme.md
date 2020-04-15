# Умный сервис прогноза погоды
**Средний уровень сложности**

- Для работы сервиса используется язык программирования C++
- Интерфейс приложения -- консоль (мне очень жаль, я последние 3 часа, рашающие три часа, которые я мог бы потратить на корректную обработку ошибок\etc потратил на то, что пытался прикрутить Tg-бота, а я очень расчитывал на это, делал архитектуру для того, чтобы можно было удобно отправлять локацию из ТГ и подтягивать данные о местоположении. Увы. У меня не заработала ни одна из тех C++ TgApi библиотек, которые я нашёл. Все просто зависали на открытии сокета. Видимо, у меня какие-то проблемы с компом. Или со знаниями. Короче, обидно. Понимаю, что учитывая мою степень обработки ошибок и то, что это консоль -- шансов у меня не очень много, но я всё равно буду рад любому фидбеку.)
- Формат ответа - текст. Данные, полученные и API (температура и её ощущение) подставляются в текстовый шаблон. В итоге, в зависимости от данных, получается что-то вроде 

`Ваш прогноз погоды ☄:`

`Сейчас температура составляет -8℃, но ощущается как -3℃`

- Всё, кроме JsonParser написано мной в рамках работы над этим проктом

---
Процесс работы:
- Данные (координаты или название города) запрашиваются у пользователя.
- Если это название города - запрос сначала улетает в на апи https://opencagedata.com/, которое говорит координаты по названию города.
- Если это координаты, или у нас уже есть координаты из предыдущего шага - мы отправляем их на http://api.openweathermap.org, оно говорит нам погоду
- Погоду используем для формирования ответа

---
Как это запустить:
1) Добавить ключ API для сервиса http://api.openweathermap.org в файл Weathertoken.api в корень проекта
2) Добавить ключ API для сервиса http://api.opencagedata.com/ в файл Citiestoken.api в корень проекта
3) `cmake .`
4) `make -j4`
5) `./SmartWeatherService`

_p.s. Если нужны токены -- напишите в телеграм @shnurd6, я скину)_