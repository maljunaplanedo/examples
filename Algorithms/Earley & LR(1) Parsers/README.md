# Реализация алгоритмов парсинга КС-грамматик: Эрли и LR(1)
### Практическое задание по предмету "Формальные языки и трансляции"
### Сацкевич Марат, Б05-029

--------------------------

## Установка

- Клонируйте репозиторий `git clone git@github.com:maljunaplanedo/fl-practical-hw-2.git` и перейдите в корневую директорию
- Если PR не вмерджен, переключитесь на ветку `dev`
- Запустите скрипт `./setup.py`

## Запуск

- Запустите виртуальное окружение `source env/bin/activate`
- Установите переменную окружения `export PYTHONPATH=.`
- Запускайте необходимые скрипты

## Скрипты

- `./run_earley.sh` - проверка слов на принадлежность языку, задаваемому КС-грамматикой с помощью алгоритма Эрли
- `./run_lr.sh` - проверка слов на принадлежность языку, задаваемому LR(1) КС-грамматикой с помощью алгоритма LR(1)
- `./test_earley.sh` - запуск unit-тестов для алгоритма Эрли
- `./test_lr.sh` - запуск unit-тестов для алгоритма LR(1)

## Структура
- В директории `grammar/` реализован класс КС-грамматики, в `earley/` - алгоритм Эрли, в `lr/` - алгоритм LR(1)
- В каждой директории собственно исходный код находится в `src/`, unit-тесты в `tests/`, оболочка для работы с
алгоритмом в консоли в `main.py`. В `grammar/` присутствует только директория `src/`.