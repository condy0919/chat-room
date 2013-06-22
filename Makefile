.PHONY: main clean

CFLAGS=`pkg-config --cflags --libs gtk+-2.0`
main: gtkMain.o gui_login_window.o gui_main_window.o user.o signal_cb.o recv_thread.o
	gcc gui_login_window.o gui_main_window.o gtkMain.o user.o signal_cb.o recv_thread.o -o main $(CFLAGS) -g -Wall

gtkMain.o: gtkMain.c gui.h
	gcc gtkMain.c -c $(CFLAGS) -g -Wall

gui_login_window.o: gui_login_window.c gui.h signal_cb.h
	gcc gui_login_window.c -c $(CFLAGS) -g -Wall

gui_main_window.o: gui_main_window.c gui.h signal_cb.h
	gcc gui_main_window.c -c $(CFLAGS) -g -Wall

user.o: user.c message.h user.h
	gcc user.c -c $(CFLAGS) -g -Wall

signal_cb.o: signal_cb.c message.h signal_cb.h
	gcc signal_cb.c -c $(CFLAGS) -g -Wall

recv_thread.o: recv_thread.c message.h user.h gui.h
	gcc recv_thread.c -c $(CFLAGS) -g -Wall

clean:
	-@rm *.o
#	-rm main
