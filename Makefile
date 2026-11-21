CC = g++
CFLAGS = -Wall -Wextra -std=c++11

OBJS = wsp.o wsp_math.o \
       dijkstra.o spanning_tree.o \
	   ranking.o localsearch.o metaheuristics.o \

TEST_SRC  = tests/*.c src/*.h src/*.c
TEST_OBJS = test_all.o

all: $(OBJS) main.o
	$(CC) $(CFLAGS) -o a.out $(OBJS) main.o

ils: $(OBJS) ils.o
	$(CC) $(CFLAGS) -o ils $(OBJS) ils.o

grasp: $(OBJS) grasp.o
	$(CC) $(CFLAGS) -o grasp $(OBJS) grasp.o

tests: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o test_all.out $(TEST_OBJS)

wsp.o: src/wsp.h src/wsp.c
	$(CC) $(CFLAGS) -c src/wsp.c

wsp_math.o: src/wsp_math.h src/wsp_math.c
	$(CC) $(CFLAGS) -c src/wsp_math.c

dijkstra.o: src/dijkstra.h src/dijkstra.cpp
	$(CC) $(CFLAGS) -c src/dijkstra.cpp

spanning_tree.o: src/spanning_tree.h src/spanning_tree.c
	$(CC) $(CFLAGS) -c src/spanning_tree.c

ranking.o: src/ranking.h src/ranking.c
	$(CC) $(CFLAGS) -c src/ranking.c

localsearch.o: src/localsearch.h src/localsearch.c
	$(CC) $(CFLAGS) -c src/localsearch.c

metaheuristics.o: src/metaheuristics.h src/metaheuristics.c
	$(CC) $(CFLAGS) -c src/metaheuristics.c

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c
	
ils.o: src/ils.c
	$(CC) $(CFLAGS) -c src/ils.c

grasp.o: src/grasp.c
	$(CC) $(CFLAGS) -c src/grasp.c

test_all.o: $(TEST_SRC)
	$(CC) $(CFLAGS) -c tests/test_all.c -I src/

.PHONY: all tests clean

clean:
	rm -f *.o
	rm -f a.out ils grasp test_all.out

