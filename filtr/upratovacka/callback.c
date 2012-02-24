#include <gtk/gtk.h>

#include "callback.h"
#include "glob.h"


extern GtkWidget *window, *img;

extern char *filename;
extern int up, rp;
extern GdkPixbuf *undo_stack[];


G_MODULE_EXPORT void
on_about_activate(GtkWidget *widget, gpointer data)
{
    gtk_dialog_run(GTK_DIALOG(widget));
    gtk_widget_hide(widget);
}


G_MODULE_EXPORT void
on_menu_open_activate(GtkWidget *widget, gpointer data)
{
    GtkResponseType response;
    GtkWidget *dialog;
    GdkPixbuf *buf;
    GError *err;

    err = NULL;
    dialog = gtk_file_chooser_dialog_new("Otevřít obrázek",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        buf = gdk_pixbuf_new_from_file(filename, &err);
        if (err == NULL) {
            PUSH(undo_stack, up, buf);
            gtk_image_set_from_pixbuf(GTK_IMAGE(img), buf);

            /* TODO width, height get here */
        } else {
            fprintf(stderr, "%s is not an image.\n", filename);
        }
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}


G_MODULE_EXPORT void
on_menu_save_as_activate(GtkWidget *widget, gpointer data)
{
    GtkResponseType response;
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Uložit jako",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_SAVE_AS, GTK_RESPONSE_ACCEPT,
                                         NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		/* Reuse callback for saving */
		on_menu_save_activate(NULL, NULL);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}


G_MODULE_EXPORT void
on_menu_save_activate(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;
	GError *err;

	err = NULL;
    /* TODO if not empty */
    buf = PEEK(undo_stack, up);

    gdk_pixbuf_save(buf, filename, "png", &err, NULL);
    if (err != NULL)
        fprintf(stderr, "%s error saving image.\n", filename);
}
