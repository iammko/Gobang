CXX=g++

CXXFLAGS= -g -Wall
CXXLFLAGS= -lprotobuf

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

TARGET=GobangClient

.PHONY: clean

$(TARGET):$(OBJS) 
	$(CXX) $(CXXLFLAGS) $(OBJS) -o $@
	rm -rf *.o

$(OBJS):%.o:%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET) 
