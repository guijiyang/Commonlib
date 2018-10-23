##Commonlib库编译脚本
CC = /usr/bin/gcc
CXX = /usr/bin/g++

#路径变量
#项目路径
D_BIN = bin
MAKE_BIN_DIR := $(shell mkdir -p $(D_BIN))
#目标
TARGET = $(D_BIN)/Commonlib.so
D_PATH = ./
#对象路径，放置生成的.o文件
D_OBJ = obj
VPATH = .:MessageLoop:Log:Thread:Thread/Private
SRCS = $(foreach dir, $(subst :, , $(VPATH)), $(wildcard $(dir)/*.cpp))
OBJ_PATH = $(addprefix $(D_OBJ)/, $(subst :, , $(VPATH)))
OBJS = $(addprefix $(D_OBJ)/, $(patsubst %.cpp, %.o, $(SRCS)))

#创建一个object文件的目录
MAKE_OBJECT_DIR := $(shell mkdir -p $(OBJ_PATH))
#命令，参数
RM = rm -f
CFLAGS = -Wall
CXXFLAGS = -Wall -g -c -fPIC -std=c++17 -I$(D_PATH) -I /usr/local/log4cxx/include
LDFLAGS = -shared -fPIC  -L/usr/local/log4cxx/lib
LIBS = -llog4cxx -lpthread

$(TARGET) : $(OBJS)  
	@$(CXX) -o $@ $(LDFLAGS) $(LIBS) $^

$(D_OBJ)/%.o : %.cpp $(D_OBJ)/%.cpp.d
	@$(CXX) -o $@ $(CXXFLAGS) $<

DEPS = $(OBJS:.o=.cpp.d)
$(DEPS) : $(D_OBJ)/%.cpp.d : %.cpp
	$(CXX) $< -MM -I$(D_PATH) > $(D_OBJ)/$<.d
all:
	@echo $(OBJS)
	@echo $(DEPS)

-include $(SRC_MK)

.PHONY : clean
clean : 
	$(RM) $(TARGET);
	rm -fr $(D_OBJ)/* $(D_BIN)/*
