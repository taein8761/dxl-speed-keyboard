CX = g++   
CXFLAGS = -g -Wall
CVFLAGS = `pkg-config opencv4 --cflags --libs`
# 백쿼트(`명령어`)는쉘에서명령어를실행한결과를의미
SRCS = main.cpp      
TARGET = Camera
$(TARGET):$(SRCS)
	$(CX) $(CXFLAGS) -o $(TARGET) $(SRCS) $(CVFLAGS)
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) 
