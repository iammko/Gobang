CXX=g++

CXXFLAGS= -g -Wall -I./servicecore -I./proto -I./common -I./gameserver
CXXLFLAGS= -lprotobuf

DIRS=gameserver servicecore common

SRCS=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
SRCS2=$(wildcard ./proto/*.cc)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
OBJS2=$(patsubst %.cc,%.o,$(SRCS2))
ALLOBJS=$(OBJS) $(OBJS2)

TARGET=GobangServer

.PHONY: clean

$(TARGET):$(ALLOBJS) 
	$(CXX) $(CXXLFLAGS) $(ALLOBJS) -o $@
	rm -rf *.o

$(OBJS):%.o:%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS2):%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@for dir in $(DIRS); \
	do \
		cd $$dir; \
		echo cd $$dir; \
		for file in $(ls -ls);do \
			echo rm $$file; \
		done; \
		rm -rf *.o; \
		echo rm -rf *.o; \
		cd ../; \
		echo cd ../; \
	done
	rm -f $(TARGET)
