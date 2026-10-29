#!/bin/bash
# Показывает по очереди вывод программы для каждого рисунка отчёта (Рисунок 1 ... Рисунок 12).
# Перед каждым рисунком экран очищается, команда выводится на экран (как в отчёте Морозова).
# Запуск из папки cpp:   bash tests/make_screenshots.sh
# Сделайте скриншот терминала и нажмите Enter, чтобы перейти к следующему рисунку.
# Только вывод без пауз:  bash tests/make_screenshots.sh --no-pause

cd "$(dirname "$0")/.." || exit 1
PAUSE=1
[ "$1" = "--no-pause" ] && PAUSE=0

[ -x ./lab2 ] || make || { echo "Не удалось собрать lab2"; exit 1; }
rm -f sets.data

run() {                      # печатает команду как приглашение и выполняет её
    printf '$ %s\n' "$1"
    eval "$1"
}

figure() {                   # figure <номер> <заголовок> <команда>...
    local n="$1" title="$2"; shift 2
    [ $PAUSE = 1 ] && clear
    echo "=== Рисунок $n. $title ==="
    echo
    for c in "$@"; do run "$c"; done
    echo
    if [ $PAUSE = 1 ]; then read -r -p ">>> Рисунок $n: сделайте скриншот и нажмите Enter..." _; fi
}

figure 1 "Обход графа и дерева в глубину (задание 1)" \
  "./lab2 --task 1 <<< \$'GRAPH\nA B\nA C\nB D\nC D\nSTART A\nTREE\n1 2 3 4 5 6 7'"

rm -f sets.data
figure 2 "Работа с множеством (задание 2)" \
  "./lab2 --file sets.data --query 'SETADD myset apple'" \
  "./lab2 --file sets.data --query 'SETADD myset banana'" \
  "./lab2 --file sets.data --query 'SETADD myset cherry'" \
  "./lab2 --file sets.data --query 'SET_AT myset apple'" \
  "./lab2 --file sets.data --query 'SET_AT myset orange'" \
  "./lab2 --file sets.data --query 'SETDEL myset apple'" \
  "./lab2 --file sets.data --query 'SET_AT myset apple'" \
  "./lab2 --file sets.data --query 'SETPRINT myset'" \
  "cat sets.data"

figure 3 "Схожие подмножества (задание 3, вариант 2)" \
  "./lab2 --task 3 <<< '5 8 1 14 7'" \
  "./lab2 --task 3 <<< '4 10 5 1 3 7 7 4'"

figure 4 "Поиск подмассивов с заданной суммой (задание 4, вариант 2)" \
  "./lab2 --task 4 <<< '4 -7 1 5 -4 0 -3 2 4 1 5'" \
  "./lab2 --task 4 <<< '1 2 3 100'"

figure 5 "Глубина добавляемых элементов (задание 5, вариант 8)" \
  "./lab2 --task 5 <<< '7 3 2 1 9 5 4 6 8 0'" \
  "./lab2 --task 5 <<< '5 5 3 3 8 0'"

figure 6 "Метод цепочек и хеширование кукушки (задание 6.1, вариант 2)" \
  "./lab2 --task 6 <<< \$'CHAIN apple=1 banana=2 cherry=3\nCUCKOO apple=1 banana=2 cherry=3'"

figure 7 "Эмпирический анализ хеширования (задание 6.1)" \
  "./lab2 --task 6 <<< 'BENCH 100000'" \
  "./lab2 --task 6 <<< 'BENCH 1000000'"

figure 8 "Фильтр Блума в хеш-таблице (задание 6.2, вариант 8)" \
  "./lab2 --task 6 <<< \$'ADD apple 10\nADD banana 20\nHAS apple\nHAS orange\nADD apple 11\nHAS apple\nDEL apple\nHAS apple'"

figure 9 "LFU-кэш (задание 7, вариант 2)" \
  "./lab2 --task 7 <<< '3 10 SET 5 7 SET 4 6 SET 3 5 SET 2 4 SET 1 3 GET 1 GET 2 GET 3 GET 4 GET 5'" \
  "./lab2 --task 7 <<< '2 7 SET 1 10 SET 2 20 GET 1 GET 1 GET 2 SET 3 30 GET 2'"

figure 10 "Обработка ошибочных запросов и входных данных" \
  "./lab2 --task 4 <<< '1 2 abc 3'; echo \"код возврата: \$?\"" \
  "./lab2 --task 7 <<< '0 1 GET 1'; echo \"код возврата: \$?\"" \
  "./lab2 --file sets.data --query 'SETADD myset'; echo \"код возврата: \$?\"" \
  "./lab2 --file /no/such/dir/x.data --query 'SETADD s 1'; echo \"код возврата: \$?\"" \
  "./lab2 --task 9; echo \"код возврата: \$?\""

figure 11 "Интерактивное меню" \
  "./lab2 <<< \$'abc\n5\n7 3 2 1 9 5 4 6 8 0\n\n0'"

figure 12 "Автоматические тесты" \
  "make test 2>&1 | tail -n 3"

rm -f sets.data
