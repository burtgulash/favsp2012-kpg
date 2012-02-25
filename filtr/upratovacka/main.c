#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#include "glob.h"


/* Undo/Redo stacks. */
int up = 0,
    rp = 0;
GdkPixbuf *undo_stack[STACK_SIZE];
GdkPixbuf *redo_stack[STACK_SIZE];

/* Global parameters */
char *filename, *image_format;
pixbuf_data p_data;
AppWidgets widgets;



int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GError *err;

    err = NULL;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();

    gtk_builder_add_from_file(builder, "upratovac.glade", &err);
    if (err != NULL) {
        fprintf(stderr, "%s: can not load gui.\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    /* Populate global widgets. */
    widgets.window = GET_WIDGET_BY_ID(builder, "window");
    widgets.img = GET_WIDGET_BY_ID(builder, "img");
    widgets.aboutdialog = GET_WIDGET_BY_ID(builder, "aboutdialog");

    widgets.edge_detect_dialog =
                    GET_WIDGET_BY_ID(builder, "edge_detect_dialog");
    widgets.edge_detect_combo_box =
                    GET_WIDGET_BY_ID(builder, "edge_detect_combo_box");

    widgets.toolbutton_undo =
                    GET_WIDGET_BY_ID(builder, "toolbutton_undo");
    widgets.toolbutton_redo =
                    GET_WIDGET_BY_ID(builder, "toolbutton_redo");

    widgets.filechooser_open =
                    GET_WIDGET_BY_ID(builder, "filechooser_open");
    widgets.filechooser_save_as =
                    GET_WIDGET_BY_ID(builder, "filechooser_save_as");


    /* Connect signals. */
    gtk_builder_connect_signals(builder, &widgets);
    g_object_unref(builder);

    /* Display and run. */
    gtk_widget_show(widgets.window);
    gtk_main();

    return EXIT_SUCCESS;
}
