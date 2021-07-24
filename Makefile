all: libprocessjava.so
libprocessjava.so: src/libprocessjava.c src/conf.c src/conf.h src/log.c src/log.h
	gcc -Wall -fPIC -shared -o libprocessjava.so src/libprocessjava.c src/conf.c src/conf.h src/log.c src/log.h -ldl
	# mv libprocessjava.so /usr/local/lib64
	# echo "/usr/local/lib64/libprocessjava.so" > /etc/ld.so.preload
.PHONY clean:
	rm -f libprocessjava.so
# .SUFFIXES:.c .o 
# CC=gcc


# SRCS=log.c conf.c libprocessjava.c
# OBJS=$(SRCS:.c=.o)
# vpath %.h src
# vpath %.c src

# TARGET=libprocessjava.so

# start: $(OBJS)
# 	$(CC) -Wall -fPIC -ldl -shared -o $(TARGET) $(OBJS)
# .c.o:
# 	$(CC) -c -o $@ -c $< 

# # TARGET:
# # 	gcc -Wall -fPIC -ldl -shared -o $(TARGET)

# .PHONY:clean
# 	@echo "clean..."
# install:
# 	@echo "install libprocessjava.."
# 	echo "/usr/local/lib/libprocessjava.so" > /etc/ld.so.preload
# .SUFFIXES:.c .o
# CC=gcc
# SRCS=src/conf.c src/log.c src/libprocessjava.c
# OBJS=$(SRCS:.c=.o)
# EXEC=hello
# start: $(OBJS)
# 	$(CC) gcc -Wall -fPIC -ldl -shared -o $(EXEC) $(OBJS)
# 	@echo '----------------ok------------'
# .c.o:
# 	$(CC) -o $@ -c $< 
# clean:
# 	rm -f $(OBJS)