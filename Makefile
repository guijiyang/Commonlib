##Commonlib库编译脚本
CC = /usr/bin/gcc
CXX = /usr/bin/g++

#项目路径
D_BIN = bin

#编译生成目标
TARGET = $(D_BIN)/Commonlib.so
D_PATH = ./
#对象路径，放置生成的.o文件
D_OBJ = obj
VPATH = .:MessageLoop:Log:Thread:Thread/Private
SRCS = $(foreach dir, $(subst :, , $(VPATH)), $(wildcard $(dir)/*.cpp))
OBJ_PATH = $(addprefix $(D_OBJ)/, $(subst :, , $(VPATH)))
OBJS = $(addprefix $(D_OBJ)/, $(patsubst %.cpp, %.o, $(SRCS)))

#安装路径
INSTALL_DIR = /usr/local/Commonlib
#头文件安装路径
HEADER_PATH = ./:MessageLoop:MessageLoop/Private:Log:Thread:Thread/Private:Pattern
HEADERS =  $(subst :, , $(HEADER_PATH))
HEADERS_INSTALL_PATH = $(addprefix $(INSTALL_DIR)/include/, $(HEADERS))
SHAREDLIB_INSTALL_PATH = $(INSTALL_DIR)/lib
#创建一个object文件的目录和bin目录
MAKE_BIN_DIR := $(shell mkdir -p $(D_BIN))
MAKE_OBJECT_DIR := $(shell mkdir -p $(OBJ_PATH))
# all:
# 	@echo $(HEADERS_INSTALL_PATH)

#命令，参数
# RM = rm -f
CFLAGS = -Wall
CXXFLAGS = -Wall -g -c -fPIC -std=c++17 -I$(D_PATH) -I /usr/local/log4cxx/include
LDFLAGS = -shared -fPIC  -L/usr/local/log4cxx/lib
LIBS = -llog4cxx -lpthread

$(TARGET) : $(OBJS)  
	@$(CXX) -o $@ $(LDFLAGS) $^ $(LIBS) 

$(D_OBJ)/%.o : %.cpp $(D_OBJ)/%.cpp.d
	@$(CXX) -o $@ $(CXXFLAGS) $<

DEPS = $(OBJS:.o=.cpp.d)
$(DEPS) : $(D_OBJ)/%.cpp.d : %.cpp
	$(CXX) $< -MM -I$(D_PATH) > $(D_OBJ)/$<.d

.PHONY : clean install
clean : 
	$(RM) $(TARGET);
	rm -fr $(D_OBJ) $(D_BIN)

install:
	@ sudo mkdir -p $(INSTALL_DIR) $(HEADERS_INSTALL_PATH) $(SHAREDLIB_INSTALL_PATH); \
	for headpath in $(HEADERS); do \
		sudo install -m 644 $$headpath/*.h $(INSTALL_DIR)/include/$$headpath; \
	done; \
	sudo install -m 644 $(TARGET) $(SHAREDLIB_INSTALL_PATH)
