#include <glib.h>
#include <gtk/gtk.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"
#include "glob.h"


#define ON_FILTER_CHOOSE(filter)                                             \
G_MODULE_EXPORT void                                                         \
on_filter_##filter##_choose(GtkWidget *widget, gpointer data)                \
{                                                                            \
    int response, method;                                                    \
                                                                             \
    if(!IS_EMPTY(undo_stack, up, u_start)) {                                 \
        response = gtk_dialog_run(GTK_DIALOG(widgets.filter##_dialog));      \
        if (response == 1) {                                                 \
            method = gtk_combo_box_get_active(                               \
                     GTK_COMBO_BOX(widgets.filter##_combo_box));             \
            apply_##filter(method);                                          \
        }                                                                    \
                                                                             \
        gtk_widget_hide(widgets.filter##_dialog);                            \
    }                                                                        \
}


extern char *filename, *image_format;
extern int up, rp;
extern int u_start, r_start;
extern GdkPixbuf *undo_stack[];
extern GdkPixbuf *redo_stack[];

extern pixbuf_data p_data;
extern AppWidgets widgets;



static void apply_edge_detect(int method);
static void apply_gray_scale(int method);

enum {SOBEL, PREWITT, ROBERTS_CROSS, LAPLACE};
enum {LUMINANCE, AVERAGE};

ON_FILTER_CHOOSE(gray_scale)
ON_FILTER_CHOOSE(edge_detect)


G_MODULE_EXPORT void
on_filter_sharpen_choose(GtkWidget *widget, gpointer data)
{
    int response;
    GdkPixbuf *buf, *new;
    guchar *olds, *news;
    double sharpen_amount;

    if(!IS_EMPTY(undo_stack, up, u_start)) {
        response = gtk_dialog_run(
                   GTK_DIALOG(widgets.sharpen_dialog));

        if (response == 1) {
            sharpen_amount = gtk_range_get_value(
                     GTK_RANGE(widgets.sharpen_amount));

            buf = PEEK(undo_stack, up);
            new = gdk_pixbuf_copy(buf);
            olds = gdk_pixbuf_get_pixels(buf);
            news = gdk_pixbuf_get_pixels(new);

            sharpen(sharpen_amount, news, olds,
                    p_data.height,
                    p_data.rowstride,
                    p_data.n_channels);

            UPDATE_EDITED_IMAGE(new);
        }

        gtk_widget_hide(widgets.sharpen_dialog);
    }
}


G_MODULE_EXPORT void
on_filter_difference_of_gaussians_choose(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf, *new;
    guchar *olds, *news;
    int response;
    double first, second;

    if(!IS_EMPTY(undo_stack, up, u_start)) {
        response = gtk_dialog_run(
                   GTK_DIALOG(widgets.difference_of_gaussians_dialog));

        if (response == 1) {
            first = gtk_range_get_value(
                    GTK_RANGE(widgets.difference_of_gaussians_first));
            second = gtk_range_get_value(
                     GTK_RANGE(widgets.difference_of_gaussians_second));

            buf = PEEK(undo_stack, up);
            new = gdk_pixbuf_copy(buf);
            olds = gdk_pixbuf_get_pixels(buf);
            news = gdk_pixbuf_get_pixels(new);

            difference_of_gaussians(first, second,
                                    news, olds,
                                    p_data.height,
                                    p_data.rowstride,
                                    p_data.n_channels);

            UPDATE_EDITED_IMAGE(new);
        }

        gtk_widget_hide(widgets.difference_of_gaussians_dialog);
    }
}


G_MODULE_EXPORT void
on_filter_gaussian_blur_choose(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf, *new;
    int response;
    guchar *olds, *news;
    double horizontal, vertical;

    if(!IS_EMPTY(undo_stack, up, u_start)) {
        response = gtk_dialog_run(
                   GTK_DIALOG(widgets.gaussian_blur_dialog));
        if (response == 1) {
            horizontal = gtk_spin_button_get_value(
                         GTK_SPIN_BUTTON(widgets.gaussian_blur_horizontal));
            vertical = gtk_spin_button_get_value(
                       GTK_SPIN_BUTTON(widgets.gaussian_blur_vertical));

            buf = PEEK(undo_stack, up);
            new = gdk_pixbuf_copy(buf);
            olds = gdk_pixbuf_get_pixels(buf);
            news = gdk_pixbuf_get_pixels(new);

            gaussian_blur(horizontal, vertical, news, olds,
                                  p_data.height,
                                  p_data.rowstride,
                                  p_data.n_channels);

            UPDATE_EDITED_IMAGE(new);
        }

        gtk_widget_hide(widgets.gaussian_blur_dialog);
    }
}


static void apply_gray_scale(int method)
{
    int h, s, n_chans;
    GdkPixbuf *buf, *new;
    guchar *news;

    buf = PEEK(undo_stack, up);
    new = gdk_pixbuf_copy(buf);

    news = gdk_pixbuf_get_pixels(new);
    h = gdk_pixbuf_get_height(buf);
    s = gdk_pixbuf_get_rowstride(buf);
    n_chans = gdk_pixbuf_get_n_channels(buf);


    g_print("Apply gray_scale filter ");
    switch (method) {
        case AVERAGE:
            g_print("average");
            gray_scale_avg(news, h, s, n_chans);
            break;

        default:
            g_print("luminance");
            gray_scale_luminance(news, h, s, n_chans);
    }
    g_print("\n");

    UPDATE_EDITED_IMAGE(new);
}


static void apply_edge_detect(int method)
{
    int h, s, n_chans;
    GdkPixbuf *buf, *new;
    guchar *olds, *news;

    buf = PEEK(undo_stack, up);
    new = gdk_pixbuf_copy(buf);

    olds = gdk_pixbuf_get_pixels(buf);
    news = gdk_pixbuf_get_pixels(new);

    h = p_data.height;
    s = p_data.rowstride;
    n_chans = p_data.n_channels;

    switch (method) {
        case SOBEL:
            g_print("Sobel");
            sobel(news, olds, h, s, n_chans);
            break;

        case PREWITT:
            g_print("Prewitt");
            prewitt(news, olds, h, s, n_chans);
            break;

        case LAPLACE:
            g_print("Laplace");
            laplace(news, olds, h, s, n_chans);
            break;

        default:
            g_print("Roberts cross");
            roberts_cross(news, olds, h, s, n_chans);
    }
    g_print("\n");

    UPDATE_EDITED_IMAGE(new);
}


G_MODULE_EXPORT void
on_undo_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;

    if (!IS_EMPTY(undo_stack, up, u_start) &&
        !HAS_LAST(undo_stack, up, u_start))
    {
        buf = POP(undo_stack, up);
        PUSH(redo_stack, rp, r_start, buf);
        UPDATE_IMAGE();

        SET_SENSITIVITY_DO_BUTTON(redo, TRUE);
        if (IS_EMPTY(undo_stack, up, u_start) ||
            HAS_LAST(undo_stack, up, u_start))
            SET_SENSITIVITY_DO_BUTTON(undo, FALSE);

        g_print("Undo\n");
    }
}


G_MODULE_EXPORT void
on_redo_clicked(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;

    if (!IS_EMPTY(redo_stack, rp, r_start)) {
        buf = POP(redo_stack, rp);
        PUSH(undo_stack, up, u_start, buf);
        UPDATE_IMAGE();

        SET_SENSITIVITY_DO_BUTTON(undo, TRUE);
        if (IS_EMPTY(redo_stack, rp, r_start))
            SET_SENSITIVITY_DO_BUTTON(redo, FALSE);

        g_print("Redo\n");
    }
}


G_MODULE_EXPORT void
on_about_activate(GtkWidget *widget, gpointer data)
{
    gtk_dialog_run(GTK_DIALOG(widgets.aboutdialog));
    g_print("About\n");
    gtk_widget_hide(widgets.aboutdialog);
}


G_MODULE_EXPORT void
on_menu_open_activate(GtkWidget *widget, gpointer data)
{
    int response;
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
            /* Clear stacks before opening new image. */
            while (!IS_EMPTY(undo_stack, up, u_start))
                gdk_pixbuf_unref(POP(undo_stack, up));
            while (!IS_EMPTY(redo_stack, rp, r_start))
                gdk_pixbuf_unref(POP(redo_stack, rp));

            PUSH(undo_stack, up, u_start, buf);
            UPDATE_IMAGE();

            SET_SENSITIVITY_DO_BUTTON(undo, FALSE);
            SET_SENSITIVITY_DO_BUTTON(redo, FALSE);

            p_data.colorspace = gdk_pixbuf_get_colorspace(buf);
            p_data.has_alpha = gdk_pixbuf_get_has_alpha(buf);
            p_data.bits_per_sample = gdk_pixbuf_get_bits_per_sample(buf);
            p_data.width = gdk_pixbuf_get_width(buf);
            p_data.height = gdk_pixbuf_get_height(buf);
            p_data.rowstride = gdk_pixbuf_get_rowstride(buf);
            p_data.n_channels = gdk_pixbuf_get_n_channels(buf);
        } else {
            g_printerr("%s is not an image.\n", filename);
        }
    }

    gtk_widget_hide(GTK_WIDGET(widgets.filechooser_open));
}


G_MODULE_EXPORT void
on_menu_save_activate(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *buf;
    GError *err;

    err = NULL;
    if (!IS_EMPTY(undo_stack, up, u_start)) {
        buf = PEEK(undo_stack, up);

        gdk_pixbuf_save(buf, filename, image_format, &err, NULL);
        if (err != NULL)
            g_printerr("%s error saving image.\n", filename);
    }
}


G_MODULE_EXPORT void
on_menu_save_as_activate(GtkWidget *widget, gpointer data)
{
    int response;

    response = gtk_dialog_run(GTK_DIALOG(widgets.filechooser_save_as));
    if (response == 1) {
        filename = gtk_file_chooser_get_filename(
                   GTK_FILE_CHOOSER(widgets.filechooser_save_as));
        on_menu_save_activate(NULL, NULL);
    }

    gtk_widget_hide(GTK_WIDGET(widgets.filechooser_save_as));
}
