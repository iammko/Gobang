
SRCS=$(wildcard *.proto)
CPPSRCS=$(patsubst %.proto,%.pb.cc,$(SRCS))
CPPHEADS=$(patsubst %.proto,%.pb.h,$(SRCS))
CPPOBJS=$(patsubst %.proto,%.pb.o,$(SRCS))



.PHONY:clean build

build:$(SRCS)
	protoc -I=./ --cpp_out=./ $(SRCS)
	cp $(CPPSRCS) $(CPPHEADS) ../GobangClient/proto/
	cp $(CPPSRCS) $(CPPHEADS) ../GobangServer/proto/	


