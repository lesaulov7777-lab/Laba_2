CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -MMD -MP
INCLUDES = -Iinterface -Itasks -Iarray -Isingly_list -Idoubly_list -Istack -Iqueue -Itree -Ihash -Iset -Ibst -Ibloom -Ilfu

SRCS = main.cpp \
       interface/Interface.cpp \
       tasks/task1.cpp tasks/task2.cpp tasks/task3.cpp tasks/task4.cpp \
       tasks/task5.cpp tasks/task6.cpp tasks/task7.cpp \
       array/MyArray.cpp \
       singly_list/SinglyLinkedList.cpp \
       doubly_list/DoublyLinkedList.cpp \
       stack/Stack.cpp \
       queue/Queue.cpp \
       tree/CompleteBinaryTree.cpp \
       hash/HashTable.cpp \
       bst/BST.cpp \
       bloom/Bloom.cpp \
       lfu/LFU.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = lab2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TARGET)
	bash tests/run_tests.sh ./$(TARGET)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(TARGET)

run: $(TARGET)
	./$(TARGET)

-include $(OBJS:.o=.d)

.PHONY: all clean run test
