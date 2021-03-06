#include <glib.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#include "glob.h"


/* Undo/Redo stacks. */
int up = 0,
    rp = 0;
int u_start = 0,
    r_start = 0;
GdkPixbuf *undo_stack[S_SIZE];
GdkPixbuf *redo_stack[S_SIZE];

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

    gtk_builder_add_from_file(builder, "upravovac.glade", &err);
    if (err != NULL) {
        g_printerr("%s: can not load gui.\n", argv[0]);
        return EXIT_FAILURE;
    }


    /* Populate global widgets. */
    widgets.window = GET_WIDGET_BY_ID(builder, "window");
    widgets.img = GET_WIDGET_BY_ID(builder, "img");
    widgets.aboutdialog = GET_WIDGET_BY_ID(builder, "aboutdialog");

    widgets.toolbutton_undo =
                    GET_WIDGET_BY_ID(builder, "toolbutton_undo");
    widgets.toolbutton_redo =
                    GET_WIDGET_BY_ID(builder, "toolbutton_redo");

    widgets.filechooser_open =
                    GET_WIDGET_BY_ID(builder, "filechooser_open");
    widgets.filechooser_save_as =
                    GET_WIDGET_BY_ID(builder, "filechooser_save_as");


    widgets.edge_detect_dialog =
                    GET_WIDGET_BY_ID(builder, "edge_detect_dialog");
    widgets.edge_detect_combo_box =
                    GET_WIDGET_BY_ID(builder, "edge_detect_combo_box");

    widgets.gray_scale_dialog =
                    GET_WIDGET_BY_ID(builder, "gray_scale_dialog");
    widgets.gray_scale_combo_box =
                    GET_WIDGET_BY_ID(builder, "gray_scale_combo_box");

    widgets.difference_of_gaussians_dialog =
                    GET_WIDGET_BY_ID(builder, "difference_of_gaussians_dialog");
    widgets.difference_of_gaussians_first =
                    GET_WIDGET_BY_ID(builder, "difference_of_gaussians_first");
    widgets.difference_of_gaussians_second =
                    GET_WIDGET_BY_ID(builder, "difference_of_gaussians_second");

    widgets.gaussian_blur_dialog =
                    GET_WIDGET_BY_ID(builder, "gaussian_blur_dialog");
    widgets.gaussian_blur_horizontal =
                    GET_WIDGET_BY_ID(builder, "gaussian_blur_horizontal");
    widgets.gaussian_blur_vertical =
                    GET_WIDGET_BY_ID(builder, "gaussian_blur_vertical");

    widgets.sharpen_dialog =
                    GET_WIDGET_BY_ID(builder, "sharpen_dialog");
    widgets.sharpen_amount =
                    GET_WIDGET_BY_ID(builder, "sharpen_amount");



    /* Connect signals. */
    gtk_builder_connect_signals(builder, &widgets);
    g_object_unref(builder);

    /* Display and run. */
    gtk_widget_show_all(widgets.window);
    gtk_main();

    return EXIT_SUCCESS;
}
