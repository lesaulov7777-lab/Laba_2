# Лаба 2, вариант: 1.6  3.2  4.2  5.8  6.1.2  6.2.8  7.2
# make        - собрать cpp/lab2
# make test   - прогнать тесты
# make run    - запустить интерактивное меню

.PHONY: all cpp test run clean

all: cpp

cpp:
	$(MAKE) -C cpp

test:
	$(MAKE) -C cpp test

run:
	$(MAKE) -C cpp run

clean:
	$(MAKE) -C cpp clean
