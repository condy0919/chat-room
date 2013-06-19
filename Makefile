.PHONY: main clean

CFLAGS=`pkg-config --cflags --libs gtk+-2.0`
main: gtkMain.o gui_login_window.o gui_main_window.o user.o signal_cb.o
	gcc gui_login_window.o gui_main_window.o gtkMain.o user.o signal_cb.o -o main $(CFLAGS) -g

gtkMain.o: gui.h
	gcc gtkMain.c -c $(CFLAGS) -g

gui_login_window.o: gui.h signal_cb.h
	gcc gui_login_window.c -c $(CFLAGS) -g

gui_main_window.o: gui.h signal_cb.h
	gcc gui_main_window.c -c $(CFLAGS) -g

user.o: message.h user.h
	gcc user.c -c $(CFLAGS) -g

signal_cb.o: message.h signal_cb.h
	gcc signal_cb.c -c $(CFLAGS) -g

clean:
	-rm *.o
	-rm main
