#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#include "glob.h"


static GtkBuilder *builder;
GtkWidget *window, *img;

char *filename;

int up = 0,
	rp = 0;
GdkPixbuf *undo_stack[STACK_SIZE];
GdkPixbuf *redo_stack[STACK_SIZE];



int main(int argc, char *argv[])
{
	GError *err;

	err = NULL;
	gtk_init(&argc, &argv);
    builder = gtk_builder_new();

    gtk_builder_add_from_file(builder, "gui.glade", &err);
	if (err != NULL) {
		fprintf(stderr, "%s: can not load gui.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    img = GTK_WIDGET(gtk_builder_get_object(builder, "image"));

    gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show(window);
	gtk_main();

	return EXIT_SUCCESS;
}
