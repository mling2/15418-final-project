APP_NAMES=ford_fulkerson

OBJS=ford_fulkerson.o

CXX = g++ -std=c++11
CXXFLAGS = -I. -O3 -Wall

default: $(APP_NAMES)

$(APP_NAMES): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAMES)
