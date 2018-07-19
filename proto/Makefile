
SRCS=$(wildcard *.proto)
CPPSRCS=$(patsubst %.proto,%.pb.cc,$(SRCS))
CPPHEADS=$(patsubst %.proto,%.pb.h,$(SRCS))
CPPOBJS=$(patsubst %.proto,%.pb.o,$(SRCS))

DEFINE=proto_define.h

.PHONY:clean build

build:$(SRCS)
	protoc -I=./ --cpp_out=./ $(SRCS)
	cp $(CPPSRCS) $(CPPHEADS) $(DEFINE) ../GobangClient/proto/
	cp $(CPPSRCS) $(CPPHEADS) $(DEFINE) ../GobangServer/proto/	


