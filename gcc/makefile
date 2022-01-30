
#$(函数名  函数的参数)
SRC=$(wildcard *.c)  #wildcard提取当前目录下的所有.c文件的名称，SRC是这些所有.c文件的字符串列表
OBJS=$(patsubst %.c,%.o,$(SRC)) #patsubst字符串转换函数，将SRC中的所有.c字符串变为.o字符串

#代表使用的编译器，例如本次使用的是gcc，使用其它编译器时，只需更改此选项即可
CC = gcc
FLAG = -c
CFLAGS=-std=c99

parameter_o=-o
target=simpledpptest
LIB=

DEFS = -DEBUG       #宏定义
INCLUDE=
 
#“$^”代表依赖文件，“$@”代表目标文件
$(target):$(OBJS)
	$(CC) $(CFLAGS) $^ $(parameter_o) $@ $(LIB) 

.PHONY:
clean:
	rm -rf $(OBJS) $(target)
