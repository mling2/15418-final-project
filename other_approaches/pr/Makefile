APP_NAMES=pr

OBJS=pr.o

CXX = g++ -std=c++11
CXXFLAGS = -I. -O3 -Wall -fopenmp -Wno-unknown-pragmas

default: $(APP_NAMES)

$(APP_NAMES): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) main.cpp

%.o: %.cpp main.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAMES)
