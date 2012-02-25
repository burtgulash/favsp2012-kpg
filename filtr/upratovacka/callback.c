#include <gtk/gtk.h>

#include <assert.h>
#include <stdlib.h>

#include "callback.h"
#include "filters.h"
#include "glob.h"


enum {SOBEL, PREWITT, ROBERTS_CROSS};


extern char *filename, *image_format;
extern int up, rp;
extern GdkPixbuf *undo_stack[];
extern GdkPixbuf *redo_stack[];

extern pixbuf_data p_data;
extern AppWidgets widgets;


static void apply_edge_detect(int method);


G_MODULE_EXPORT void
on_undo_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;

    if (!IS_EMPTY(undo_stack, up)) {
        buf = POP(undo_stack, up);
        PUSH(redo_stack, rp, buf);
        UPDATE_IMAGE();

        SET_SENSITIVITY_DO_BUTTON(redo, TRUE);
        if (IS_EMPTY(undo_stack, up))
            SET_SENSITIVITY_DO_BUTTON(undo, FALSE);
    }
}


G_MODULE_EXPORT void
on_redo_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;

    if (!IS_EMPTY(redo_stack, rp)) {
        buf = POP(redo_stack, rp);
        PUSH(undo_stack, up, buf);
        UPDATE_IMAGE();

        SET_SENSITIVITY_DO_BUTTON(undo, TRUE);
        if (IS_EMPTY(redo_stack, rp))
            SET_SENSITIVITY_DO_BUTTON(redo, FALSE);
    }
}


static void apply_edge_detect(int method)
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

    switch (method) {
        case SOBEL:
            filtered = sobel(buf);
            break;

        case PREWITT:
            filtered = prewitt(buf);
            break;

        default:
            filtered = roberts_cross(buf);
    }

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

    UPDATE_EDITED_IMAGE(new);

    free(filtered);
}


G_MODULE_EXPORT void
on_filter_edge_detect_choose(GtkWidget *widget, gpointer data)
{
    int response, method;

    if(!IS_EMPTY(undo_stack, up)) {
        response = gtk_dialog_run(GTK_DIALOG(widgets.edge_detect_dialog));
        if (response == 1) {
            method = gtk_combo_box_get_active(
                     GTK_COMBO_BOX(widgets.edge_detect_combo_box));
            apply_edge_detect(method);
        }

        gtk_widget_hide(widgets.edge_detect_dialog);
    }
}


G_MODULE_EXPORT void
on_about_activate(GtkWidget *widget, gpointer data)
{
    gtk_dialog_run(GTK_DIALOG(widgets.aboutdialog));
    gtk_widget_hide(widgets.aboutdialog);
}


G_MODULE_EXPORT void
on_menu_open_activate(GtkWidget *widget, gpointer data)
{
    GtkResponseType response;
    GdkPixbuf *buf;
    GdkPixbufFormat *format;
    GError *err;

    err = NULL;
    response = gtk_dialog_run(GTK_DIALOG(widgets.filechooser_open));

    if (response == 1) {
        filename = gtk_file_chooser_get_filename(
                    GTK_FILE_CHOOSER(widgets.filechooser_open));
        format = gdk_pixbuf_get_file_info(filename, NULL, NULL);
        image_format = gdk_pixbuf_format_get_name(format);

        buf = gdk_pixbuf_new_from_file(filename, &err);
        if (err == NULL) {
            PUSH(undo_stack, up, buf);
            gtk_image_set_from_pixbuf(GTK_IMAGE(widgets.img), buf);

            SET_SENSITIVITY_DO_BUTTON(undo, TRUE);

            p_data.colorspace = gdk_pixbuf_get_colorspace(buf);
            p_data.has_alpha = gdk_pixbuf_get_has_alpha(buf);
            p_data.bits_per_sample = gdk_pixbuf_get_bits_per_sample(buf);
            p_data.width = gdk_pixbuf_get_width(buf);
            p_data.height = gdk_pixbuf_get_height(buf);
        } else {
            fprintf(stderr, "%s is not an image.\n", filename);
        }
    }

    gtk_widget_hide(GTK_WIDGET(widgets.filechooser_open));
}


G_MODULE_EXPORT void
on_menu_save_as_activate(GtkWidget *widget, gpointer data)
{
    GtkResponseType response;

    response = gtk_dialog_run(GTK_DIALOG(widgets.filechooser_save_as));
    if (response == 1) {
        filename = gtk_file_chooser_get_filename(
                   GTK_FILE_CHOOSER(widgets.filechooser_save_as));

        /* Reuse callback for saving */
        on_menu_save_activate(NULL, NULL);
    }

    gtk_widget_hide(GTK_WIDGET(widgets.filechooser_save_as));
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
