#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <stdlib.h>

GtkWidget* text_view_scrolling;					/* contains current text on window */		

/* create scrollable window */
GtkWidget* create_scrolling_window(void);
/* file selection window for open option */
void file_selection_open(void);
/* file selection window for save as */
void file_selection_save_as(void);
/* display text from selected file to window */
void file_open(GtkWidget* widget, gpointer user_data);
/* save current text on window to selected file */
void file_save_as(GtkWidget* widget, gpointer user_data);

int main(int argc, char *argv[]) {
 GtkWidget* window;						/* create window */
 GtkWidget* vbox;

 GtkWidget* menubar;						/* create menu with options */
 GtkWidget* filemenu;
 GtkWidget* file;
 GtkWidget* open;
 GtkWidget* save_as;
 GtkWidget* sep;
 GtkWidget* quit;

 GtkAccelGroup *accel_group = NULL;

 gtk_init(&argc, &argv);

 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);				/* set window position, size, title */
 gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
 gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
 gtk_window_set_title(GTK_WINDOW(window), "MTText");

 vbox = gtk_vbox_new(FALSE, 0);
 gtk_container_add(GTK_CONTAINER(window), vbox);

 menubar = gtk_menu_bar_new();
 filemenu = gtk_menu_new();

 accel_group = gtk_accel_group_new();
 gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

 file = gtk_menu_item_new_with_mnemonic("_File");
 open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
 save_as = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
 sep = gtk_separator_menu_item_new();
 quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);

 gtk_widget_add_accelerator(quit, "activate", accel_group, 
  GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);		/* attach menubar with options to window */
 gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
 gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_as);
 gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
 gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
 gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);
  
 GtkWidget* text_view = create_scrolling_window();			/* make window scrollable */
 gtk_box_pack_start(GTK_BOX(vbox), text_view, 1, 1, 0);

 g_signal_connect_swapped(G_OBJECT(window), "destroy",
  G_CALLBACK(gtk_main_quit), NULL);

 g_signal_connect(G_OBJECT(open), "activate",
   G_CALLBACK(file_selection_open), NULL);

 g_signal_connect(G_OBJECT(save_as), "activate",
   G_CALLBACK(file_selection_save_as), NULL);

 g_signal_connect(G_OBJECT(quit), "activate",
  G_CALLBACK(gtk_main_quit), NULL);
                                 
 gtk_widget_show_all(window);
 
 gtk_main();
  
 return 0;
}

GtkWidget* create_scrolling_window(void) {
 GtkWidget* scrolled_window;						
 GtkWidget* table;
 
 text_view_scrolling = gtk_text_view_new();
 scrolled_window = gtk_scrolled_window_new(NULL, NULL);
 table = gtk_table_new(3, 1, FALSE);

 gtk_container_add(GTK_CONTAINER(scrolled_window), text_view_scrolling);
 gtk_table_attach_defaults(GTK_TABLE(table), scrolled_window, 0, 1, 0, 1);
   
 return table; 
}

void file_selection_open(void) {
 GtkWidget* file_selector;
 
 file_selector = gtk_file_selection_new("Please select a file for editing.");
 
 g_signal_connect(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked",
  G_CALLBACK(file_open), file_selector);
 
 g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked",
  G_CALLBACK(gtk_widget_destroy), file_selector);
     
 g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->cancel_button, "clicked",
  G_CALLBACK(gtk_widget_destroy), file_selector);
 
 gtk_widget_show(file_selector); 
}

void file_selection_save_as(void) {
 GtkWidget* file_selector;
 
 file_selector = gtk_file_selection_new("Please select a file to save to.");
  
 g_signal_connect(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked",
  G_CALLBACK(file_save_as), file_selector);
  
 g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked",
  G_CALLBACK(gtk_widget_destroy), file_selector);
 
 g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->cancel_button, "clicked",
  G_CALLBACK(gtk_widget_destroy), file_selector);
  
 gtk_widget_show(file_selector);
} 

void file_open(GtkWidget* widget, gpointer user_data) {		/* get contents of selected file */
 GtkWidget* file_selector;					/* pass them to text buffer */
 const gchar* selected_filename;				
 gchar* contents;
 gsize length;					
 GError* error;
 GtkTextBuffer* buffer;						
 
 file_selector = GTK_WIDGET(user_data);
 selected_filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selector));
 g_file_get_contents(selected_filename, &contents, &length, &error);

 buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (text_view_scrolling));
 gtk_text_buffer_set_text(buffer,contents , -1);
}

void file_save_as(GtkWidget* widget, gpointer user_data) {	/* open selcted file */
 GtkWidget* file_selector;					/* fputs current text buffer to file */
 const gchar* selected_filename;                                /* close file */
 FILE* pfile;
 GtkTextBuffer* buffer;
 GtkTextIter start, end;
 gchar* text;

 file_selector = GTK_WIDGET(user_data);
 selected_filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selector));
 pfile = fopen(selected_filename, "w+");
 
 buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_scrolling));
 gtk_text_buffer_get_bounds(buffer, &start, &end);
 text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
 
 fputs(text, pfile);
 fclose(pfile);  
}






