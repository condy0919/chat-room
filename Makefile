.PHONY: main clean

CFLAGS=`pkg-config --cflags --libs gtk+-2.0`
main: gtkMain.o gui_login_window.o gui_main_window.o
	gcc gui_login_window.o gui_main_window.o gtkMain.o -o main $(CFLAGS)

gtkMain.o: gui.h
	gcc gtkMain.c -c $(CFLAGS)

gui_login_window.o: gui.h
	gcc gui_login_window.c -c $(CFLAGS)

gui_main_window.o: gui.h
	gcc gui_main_window.c -c $(CFLAGS)

clean:
	-rm *.o
	-rm main
