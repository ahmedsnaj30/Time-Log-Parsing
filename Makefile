CC = g++
CFLAGS = -g -Wall -Werror -std=c++14 -pedantic
DEPS = 
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lboost_unit_test_framework -lboost_regex -lboost_date_time
LFLAGS = cpplint --filter=-runtime/references,-build/header_guard --extensions=cpp,h
LDEPS = main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<
	
all: ps7

ps7: main.o
	$(CC) $(CLFAGS) -o $@ $^ $(LIBS)

lint:
	$(LFLAGS) $(LDEPS)

clean:
	rm -f *.o ps7