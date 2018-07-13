CXX=g++

CXXFLAGS= -g -Wall

SRCS=$(wildcard ./servicecore/*.cpp)
OBJS=$(SRCS:.cpp=.o)

TARGET=GobangServer

.PHONY: clean

$(TARGET):$(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	rm -rf *.o

$(OBJS):%.o:%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET) 
