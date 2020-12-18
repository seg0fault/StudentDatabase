CLIB = .
ListLib = .
DLIB = .
RLIB = .
BtrLIB = .
AvlLIB = .
GtrLIB = .
#FLAGS = -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format -fsanitize=address
#FLAGS = -Wall -Wextra -pedantic -Werror -lm -fsanitize=leak -g
FLAGS = -O3 -Wall -Wextra -pedantic -Werror -lm -g
#FLAGS = -Wall -Wextra -pedantic -Werror -lm
#FLAGS = -O3 

all: server.out
server.out: main.o command.o print_to_table.o print_to_file.o get.o 2list.o btree.o avl.o database.o record.o gtree.o
	g++ $(FLAGS) $^ -o server.out
	g++ $(FLAGS) client.cpp -o client.out
main.o: main.cpp $(CLIB)/command.h $(DLIB)/database.h
	g++ $(FLAGS) -c $<
command.o: $(CLIB)/command.cpp $(CLIB)/command.h $(CLIB)/enum.h
	g++ $(FLAGS) -c $<
print_to_table.o: $(CLIB)/print_to_table.cpp $(CLIB)/command.h $(CLIB)/enum.h
	g++ $(FLAGS) -c $<
print_to_file.o:  $(CLIB)/print_to_file.cpp $(CLIB)/command.h $(CLIB)/enum.h
	g++ $(FLAGS) -c $<
get.o: $(CLIB)/get.cpp $(CLIB)/command.h $(CLIB)/enum.h
	g++ $(FLAGS) -c $<
2list.o: $(ListLib)/2list.cpp $(ListLib)/2list.h $(ListLib)/2list_node.h $(RLIB)/record.h
	g++ $(FLAGS) -c $<
database.o: $(DLIB)/database.cpp $(DLIB)/database.h $(ListLib)/2list.h
	g++ $(FLAGS) -c $<
record.o: $(RLIB)/record.cpp $(RLIB)/record.h
	g++ $(FLAGS) -c $<
btree.o: $(BtrLIB)/btree.cpp $(BtrLIB)/btree.h $(BtrLIB)/btree_node.h
	g++ $(FLAGS) -c $<
avl.o: $(AvlLIB)/avl.cpp $(AvlLIB)/avl.h $(AvlLIB)/avl_node.h
	g++ $(FLAGS) -c $<	
gtree.o: $(GtrLIB)/gtree.cpp $(GtrLIB)/gtree.h $(GtrLIB)/gtree_node.h
	g++ $(FLAGS) -c $<
clean: server.out
	rm *.o
	clear
clear:
	rm *.o
