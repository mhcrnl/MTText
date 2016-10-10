Must have the gtk development library installed (libgtk2.0-dev).

To compile: 
gcc MTText.c -o MTText `pkg-config --cflags --libs gtk+-2.0`
