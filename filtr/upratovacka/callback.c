#include <gtk/gtk.h>

#include <stdlib.h>

#include "callback.h"
#include "filters.h"
#include "glob.h"


#define UPDATE_IMAGE(buf) {                                                  \
    PUSH(undo_stack, up, buf);                                               \
    gtk_image_set_from_pixbuf(GTK_IMAGE(img), buf);                          \
}                                                                            \

extern GtkWidget *window, *img;

extern char *filename, *image_format;
extern int up, rp;
extern GdkPixbuf *undo_stack[];
extern pixbuf_data p_data;


G_MODULE_EXPORT void
on_filter_edge_detect_activate(GtkWidget *widget, gpointer data)
{
    int h, s, n_chans;
    int min, max;
    int channel, x, y;
    int *filtered;
    int offset, value;
    GdkPixbuf *buf, *new;
    guchar /* *ps ,*/ *news;

    buf = PEEK(undo_stack, up);
    new = gdk_pixbuf_new(p_data.colorspace, 
                         p_data.has_alpha,
                         p_data.bits_per_sample,
                         p_data.width,
                         p_data.height);

    /* ps = gdk_pixbuf_get_pixels(buf); */
    news = gdk_pixbuf_get_pixels(new);
    h = gdk_pixbuf_get_height(buf);
    s = gdk_pixbuf_get_rowstride(buf);
    n_chans = gdk_pixbuf_get_n_channels(buf);

    filtered = roberts_cross(buf);
    
    for (channel = 0; channel < 3; channel++) {
        min = 0x0;
        max = 0xFF;

        for (y = 0; y < h; y++) {
            for (x = 0; x < s; x += n_chans) {
                value = filtered[y * s + x + channel];
                if (value > max)
                    max = value;
                if (value < min)
                    min = value;
            }
        }

        for (y = 0; y < h; y++) {
            for (x = 0; x < s; x += n_chans) {
                offset = y * s + x + channel;
                news[offset] = (filtered[offset] - min) * 0xFF / (max - min);
            }
        }
    }
    
    UPDATE_IMAGE(new);

    free(filtered);
}


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
    GdkPixbufFormat *format;
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
        format = gdk_pixbuf_get_file_info(filename, NULL, NULL);
        image_format = gdk_pixbuf_format_get_name(format);

        buf = gdk_pixbuf_new_from_file(filename, &err);
        if (err == NULL) {
            PUSH(undo_stack, up, buf);
            gtk_image_set_from_pixbuf(GTK_IMAGE(img), buf);

            p_data.colorspace = gdk_pixbuf_get_colorspace(buf);
            p_data.has_alpha = gdk_pixbuf_get_has_alpha(buf);
            p_data.bits_per_sample = gdk_pixbuf_get_bits_per_sample(buf);
            p_data.width = gdk_pixbuf_get_width(buf);
            p_data.height = gdk_pixbuf_get_height(buf);
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

    gdk_pixbuf_save(buf, filename, image_format, &err, NULL);
    if (err != NULL)
        fprintf(stderr, "%s error saving image.\n", filename);
}
