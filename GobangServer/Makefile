CXX=g++

CXXFLAGS= -g -Wall

DIRS=gameserver proto servicecore

SRCS=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

TARGET=GobangServer

.PHONY: clean

$(TARGET):$(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	rm -rf *.o

$(OBJS):%.o:%.cpp 
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
