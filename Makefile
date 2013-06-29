.PHONY: clean all

CFLAGS=`pkg-config --cflags --libs gtk+-2.0`
CC=clang

all: gtkMain.o main_window.o login_window.o recv_thread.o user.o common_list.o
	$(CC) gtkMain.o main_window.o login_window.o recv_thread.o user.o common_list.o -o chat -g $(CFLAGS)

gtkMain.o: gtkMain.c login_window.h main_window.h user.h
	$(CC) gtkMain.c -c -g $(CFLAGS)

main_window.o: main_window.c message.h user.h main_window.h
	$(CC) main_window.c -c -g $(CFLAGS)

login_window.o: login_window.c message.h user.h main_window.h login_window.h
	$(CC) login_window.c -c -g $(CFLAGS)

recv_thread.o: recv_thread.c main_window.h recv_thread.h
	$(CC) recv_thread.c -c -g $(CFLAGS)

user.o: user.c user.h file_pack.h
	$(CC) user.c -c -g $(CFLAGS)

common_list.o: common_list.c common_list.h
	$(CC) common_list.c -c -g

clean:
	-rm *.o
