CXX=g++

CXXFLAGS= -g

HEADS=$(wildcard *.h)
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

TARGET=GobangClient

.PHONY: clean

$(TARGET):$(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	rm -rf *.o

$(OBJS):%.o:%.cpp $(HEADS) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET) 
