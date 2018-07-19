CXX=g++

CXXFLAGS= -g -Wall -I./proto
CXXLFLAGS= -lprotobuf 

SRCS1=$(wildcard *.cpp) 
SRCS2=$(wildcard ./proto/*.cc)

OBJS1=$(SRCS1:.cpp=.o)
OBJS2=$(SRCS2:.cc=.o)

TARGET=GobangClient

.PHONY: clean

$(TARGET):$(OBJS1) $(OBJS2) 
	$(CXX) $(OBJS1) $(OBJS2) -o $@ $(CXXLFLAGS)
	rm -rf *.o

$(OBJS1):%.o:%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS2):%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGET) 
