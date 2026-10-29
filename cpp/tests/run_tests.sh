#!/bin/bash
# Запуск: make test   (или bash tests/run_tests.sh ./lab2)
BIN=${1:-./lab2}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
pass=0; fail=0

# На macOS нет команды timeout: берём timeout, gtimeout или perl, иначе запускаем без ограничения времени
if command -v timeout >/dev/null 2>&1; then TMO=(timeout 20)
elif command -v gtimeout >/dev/null 2>&1; then TMO=(gtimeout 20)
elif command -v perl >/dev/null 2>&1; then TMO=(perl -e 'alarm 20; exec @ARGV or exit 127')
else TMO=(); fi

# check <имя> <ожидаемый rc> <ожидаемая подстрока в stdout|-> <подстрока в stderr|-> <stdin> -- <аргументы...>
check() {
    local name="$1" rc_want="$2" out_want="$3" err_want="$4" input="$5"; shift 6
    local out err rc
    out=$(printf '%s' "$input" | "${TMO[@]}" "$BIN" "$@" 2>"$TMP/err"); rc=$?
    err=$(cat "$TMP/err")
    local ok=1
    [ "$rc" = "$rc_want" ] || ok=0
    [ "$out_want" = "-" ] || [[ "$out" == *"$out_want"* ]] || ok=0
    [ "$err_want" = "-" ] || [[ "$err" == *"$err_want"* ]] || ok=0
    [[ "$out$err" != *"terminate"* && "$out$err" != *"Segmentation"* ]] || ok=0
    if [ $ok = 1 ]; then pass=$((pass+1)); else
        fail=$((fail+1)); echo "FAIL: $name (rc=$rc, ждали $rc_want)"; echo "  stdout: $out"; echo "  stderr: $err"; fi
}

S="$TMP/sets.data"

# ---- аргументы командной строки
check "help"                0 "Использование" -  ""  -- --help
check "неизвестный флаг"    1 -  "неизвестный аргумент"  "" -- --foo
check "--file без значения" 1 -  "отсутствует значение"  "" -- --file
check "--file вместо знач." 1 -  "отсутствует значение"  "" -- --file --query x
check "только --file"       1 -  "не указан --query"  "" -- --file "$S"
check "только --query"      1 -  "не указан --file"   "" -- --query "SETADD s 1"
check "task не число"       1 -  "не является целым числом"  "" -- --task abc
check "task вне диапазона"  1 -  "вне допустимого диапазона"  "" -- --task 9
check "task 2 без file"     1 -  "нужны параметры"  "" -- --task 2
check "task 5 с --file"     1 -  "относятся только к заданию 2"  "" -- --task 5 --file x

# ---- задание 2: множество
check "SETADD"              0 "OK"     - "" -- --file "$S" --query "SETADD s 1"
check "SETADD повтор"       0 "EXISTS" - "" -- --file "$S" --query "SETADD s 1"
check "SET_AT есть"         0 "TRUE"   - "" -- --file "$S" --query "SET_AT s 1"
check "SET_AT нет"          0 "FALSE"  - "" -- --file "$S" --query "SET_AT s 2"
check "SETADD второй"       0 "OK"     - "" -- --file "$S" --query "setadd s 7"
check "SETPRINT"            0 "{1, 7}" - "" -- --file "$S" --query "SETPRINT s"
check "SETDEL"              0 "OK"     - "" -- --file "$S" --query "SETDEL s 1"
check "SETDEL нет"          0 "MISSING" - "" -- --file "$S" --query "SETDEL s 1"
check "SET_AT неизв. мн."   0 "FALSE"  - "" -- --file "$S" --query "SET_AT nope 1"
check "пустой запрос"       1 -  "пустой запрос" "" -- --file "$S" --query ""
check "неизв. команда"      1 -  "неизвестная команда" "" -- --file "$S" --query "FOO s 1"
check "мало аргументов"     1 -  "недостаточно аргументов" "" -- --file "$S" --query "SETADD s"
check "много аргументов"    1 -  "лишние аргументы" "" -- --file "$S" --query "SETADD s 1 2"
check "файл-каталог"        2 -  "является каталогом" "" -- --file "$TMP" --query "SET_AT s 1"
check "нельзя записать"     2 -  "Ошибка файла" "" -- --file "$TMP/no/dir/x" --query "SETADD s 1"
out=$("$BIN" --file "$TMP/no/dir/x" --query "SETADD s 1" 2>/dev/null)
[ -z "$out" ] && pass=$((pass+1)) || { fail=$((fail+1)); echo "FAIL: при ошибке записи напечатано \"$out\" (должно быть пусто)"; }
printf 'мусор\nSET a 1 2\n\n' > "$TMP/bad.data"
check "битая строка в файле" 0 "{1, 2}" "Предупреждение" "" -- --file "$TMP/bad.data" --query "SETPRINT a"
# файл не повреждается при ошибке
before=$(cat "$S"); "$BIN" --file "$S" --query "SETADD s" >/dev/null 2>&1
[ "$(cat "$S")" = "$before" ] && pass=$((pass+1)) || { fail=$((fail+1)); echo "FAIL: файл изменился при ошибке"; }

# ---- задание 1: DFS
check "t1 граф"      0 "DFS-граф (стек): A B D C" - $'GRAPH\nA B\nA C\nB D\nSTART A\n' -- --task 1
check "t1 дерево"    0 "DFS-дерево (стек): 1 2 4 5 3" - $'TREE\n1 2 3 4 5\n' -- --task 1
check "t1 цикл"      0 "DFS-граф (стек): A B C" - $'GRAPH\nA B\nB C\nC A\n' -- --task 1
check "t1 пусто"     1 - "нет данных" "" -- --task 1
check "t1 нет START" 1 - "отсутствует в графе" $'GRAPH\nA B\nSTART Z\n' -- --task 1
check "t1 данные до секции" 1 - "до объявления" $'A B\n' -- --task 1
check "t1 плохое число" 1 - "не является целым" $'TREE\n1 x 3\n' -- --task 1
check "t1 3 вершины в ребре" 1 - "ребро задаётся" $'GRAPH\nA B C\n' -- --task 1

# ---- задание 3: схожие подмножества (пример из ТЗ: разница 3)
check "t3 пример"    0 "Разница сумм: 3" - "5 8 1 14 7" -- --task 3
check "t3 повторы"   0 "Разница сумм: 3" - "3 3 3 3" -- --task 3
check "t3 ноль"      0 "Разница сумм: 0" - "3 4 5 6" -- --task 3
check "t3 один"      0 "Разница сумм: 4" - "4" -- --task 3
check "t3 пусто"     1 - "пустое множество" "" -- --task 3
check "t3 нуль в данных" 1 - "вне допустимого диапазона" "5 0 3" -- --task 3
check "t3 минус"     1 - "вне допустимого диапазона" "5 -3" -- --task 3
check "t3 не число"  1 - "не является целым" "5 abc" -- --task 3
check "t3 огромная сумма" 1 - "слишком большая задача" "1000000000 999999999 123456789 1000000000 900000000 800000000" -- --task 3

# ---- задание 4: подмассив с суммой (пример из ТЗ: 4 подмассива)
check "t4 пример"    0 "Найдено подмассивов: 4" - "4 -7 1 5 -4 0 -3 2 4 1 5" -- --task 4
check "t4 нет"       0 "нет" - "1 2 3 100" -- --task 4
check "t4 мало"      1 - "ожидалось" "5" -- --task 4
check "t4 не число"  1 - "не является целым" "1 2 abc 3" -- --task 4
check "t4 переполн." 1 - "вне допустимого диапазона" "99999999999999999999 5" -- --task 4

# ---- задание 5: глубины (пример из ТЗ: 1 2 3 4 2 3 4 4 3)
check "t5 пример"    0 "1 2 3 4 2 3 4 4 3" - "7 3 2 1 9 5 4 6 8 0" -- --task 5
check "t5 повторы"   0 "1 2" - "5 5 3 3 0" -- --task 5
check "t5 без нуля"  0 "1 2" "Предупреждение" "5 3" -- --task 5
check "t5 сразу 0"   0 "ни одного" - "0" -- --task 5
check "t5 пусто"     1 - "пустой ввод" "" -- --task 5
check "t5 не число"  1 - "не является целым" "5 q 0" -- --task 5

# ---- задание 6: хеш-таблицы
check "t6 chain"     0 "get c = 3" - "CHAIN a=1 b=2 c=3" -- --task 6
check "t6 chain без ="  0 "get c = c" - "CHAIN a b c" -- --task 6
check "t6 chain перезапись" 0 "get a = 9" - "CHAIN a=1 a=9" -- --task 6
check "t6 cuckoo"    0 "get b = 2" - "CUCKOO a=1 b=2 c=3" -- --task 6
check "t6 cuckoo перезапись" 0 "get a = 9" - "CUCKOO a=1 a=9" -- --task 6
check "t6 bloom"     0 "FALSE (фильтр Блума отсёк" - $'ADD x 1\nHAS zzz\nHAS x\nDEL x\nHAS x\n' -- --task 6
check "t6 bloom TRUE" 0 "TRUE (значение: 42)" - $'ADD x 42\nHAS x\n' -- --task 6
check "t6 bloom обновление" 0 "UPDATED" - $'ADD x 1\nADD x 2\nHAS x\n' -- --task 6
check "t6 bloom новое значение" 0 "TRUE (значение: 2)" - $'ADD x 1\nADD x 2\nHAS x\n' -- --task 6
check "t6 bench"     0 "Эмпирический анализ" - "BENCH 2000" -- --task 6
check "t6 bench без ошибок" 0 "Кукушка" - "BENCH 5000" -- --task 6
check "t6 bench 0"   1 - "вне допустимого" "BENCH 0" -- --task 6
check "t6 bench abc" 1 - "не является целым" "BENCH abc" -- --task 6
check "t6 неизв."    1 - "неизвестная команда" $'FOO\nADD x 1\n' -- --task 6
check "t6 ADD без ключа" 1 - "требует ключ" "ADD" -- --task 6
check "t6 ADD без значения" 1 - "ключ и значение" "ADD x" -- --task 6
check "t6 продолжает после ошибки" 1 "OK" "строка 1" $'ADD\nADD q 1\n' -- --task 6

# ---- задание 7: LFU (примеры из ТЗ)
check "t7 пример 1"  0 "3 4 5 -1 -1" - "3 10 SET 5 7 SET 4 6 SET 3 5 SET 2 4 SET 1 3 GET 1 GET 2 GET 3 GET 4 GET 5" -- --task 7
check "t7 пример 2"  0 "10 10 20 -1" - "2 7 SET 1 10 SET 2 20 GET 1 GET 1 GET 2 SET 3 30 GET 2" -- --task 7
check "t7 пусто"     1 - "ожидалось" "" -- --task 7
check "t7 cap=0"     1 - "вне допустимого" "0 1 GET 1" -- --task 7
check "t7 мало запр." 1 - "получено 1" "2 3 SET 1 2" -- --task 7
check "t7 плохой запр." 1 - "неизвестный запрос" "2 1 PUT 1 2" -- --task 7
check "t7 SET без знач." 1 - "два аргумента" "2 2 SET 1 GET 1" -- --task 7
check "t7 нет GET"   0 "запросов GET не было" - "2 1 SET 1 2" -- --task 7
check "t7 лишнее"    0 "2" "проигнорированы" "2 2 SET 1 2 GET 1 GET 9" -- --task 7

# ---- меню (ввод из канала): не падает на мусоре и EOF
check "меню выход"   0 "Лабораторная работа" - $'0\n' --
check "меню мусор"   0 "Выберите пункт" "пункт меню" $'abc\n99\n-1\n\n0\n' --
check "меню EOF"     0 "Выберите пункт" - "" --
check "меню t5"      0 "1 2 3 4 2 3 4 4 3" - $'5\n7 3 2 1 9 5 4 6 8 0\n\n0\n' --
check "меню t5 ошибка и дальше" 0 "1 2" "Ошибка ввода" $'5\nx 0\n\n5\n5 3 0\n\n0\n' --
check "меню множества" 0 "TRUE" - $'2\n'"$TMP/menu.data"$'\nSETADD a 1\nSET_AT a 1\nбред\n\n0\n' --
check "меню обрыв посреди ввода" 0 "Выберите пункт" - $'5\n7 3' --

echo "Пройдено: $pass, провалено: $fail"
[ $fail = 0 ]
