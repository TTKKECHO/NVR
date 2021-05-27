#makefile test

#指定编译器

CXX = g++
CXXFLAGS = -Wall -Wno-strict-aliasing -Wno-unused-variable

#指定头文件路径
SUBDIR 	= $(shell ls ../src -R | grep /)

#替换":"为"/"
SUBDIRS	= $(subst :,/,$(SUBDIR))

#指定库路径
INCPATH	= $(subst ../,-I../,$(SUBDIRS))

#获取源代码名称
SOURCE	= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.cpp))

#.cpp替换为.o
OBJS	= $(patsubst %.cpp,%.o,$(SOURCE)) 

#中间代码文件.o文件名，不带路径
OBJFILE	= $(foreach file,$(OBJS),$(notdir $(file)))
#中间代码文件路径
#OBJPATH	= $(addperfix 

#库文件路径
LIBPATH	=../lib/
#LIBS = -Wl,-rpath=./:./HCNetSDKCom:../lib 
LIBS = -Wl,-rpath=./:./HCNetSDKCom:../lib -lhcnetsdk -lcurl
#可执行文件路径
EXE	= ../NVR.exe
#链接，生成可执行文件
$(EXE):$(OBJFILE)
	$(CXX) -L$(LIBPATH) -o $(EXE) $(OBJFILE) $(INCPATH) $(LIBS)

#编译，生成中间文件	
$(OBJFILE):%.o:$(SUBDIRS)%.cpp
	$(CXX) -c -o $@ $< $(INCPATH) -pipe -g -Wall -std=c++11

#清除生成项，便于重新编译连接
.PHONY:clean
clean:
	rm -rf $(OBJFILE)
	rm -rf $(EXE)