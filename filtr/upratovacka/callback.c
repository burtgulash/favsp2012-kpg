#include <glib.h>
#include <gtk/gtk.h>

#include <assert.h>
#include <stdlib.h>

#include "callback.h"
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
static void apply_difference_of_gaussians(double first, double second);
static void apply_gaussian_blur();

enum {SOBEL, PREWITT, ROBERTS_CROSS, LAPLACE};
enum {LUMINANCE, AVERAGE};

ON_FILTER_CHOOSE(gray_scale)
ON_FILTER_CHOOSE(edge_detect)

G_MODULE_EXPORT void
on_filter_difference_of_gaussians_choose(GtkWidget *widget, gpointer data)
{
    double first, second;
    int response;

    if(!IS_EMPTY(undo_stack, up, u_start)) {
        response = gtk_dialog_run(
                   GTK_DIALOG(widgets.difference_of_gaussians_dialog));
        if (response == 1) {
            first = gtk_range_get_value(
                    GTK_RANGE(widgets.difference_of_gaussians_first));
            second = gtk_range_get_value(
                     GTK_RANGE(widgets.difference_of_gaussians_second));

            apply_difference_of_gaussians(first, second);
        }

        gtk_widget_hide(widgets.difference_of_gaussians_dialog);
    }
}


G_MODULE_EXPORT void
on_filter_gaussian_blur_choose(GtkWidget *widget, gpointer data)
{
    int response;
	double horizontal, vertical;

    if(!IS_EMPTY(undo_stack, up, u_start)) {
        response = gtk_dialog_run(
                   GTK_DIALOG(widgets.gaussian_blur_dialog));
        if (response == 1) {
            horizontal = gtk_spin_button_get_value(
                         GTK_SPIN_BUTTON(widgets.gaussian_blur_horizontal));
            vertical = gtk_spin_button_get_value(
                       GTK_SPIN_BUTTON(widgets.gaussian_blur_vertical));
g_print("%g, %g\n", horizontal, vertical);

            apply_gaussian_blur(horizontal, vertical);
        }

        gtk_widget_hide(widgets.gaussian_blur_dialog);
    }
}


/* TODO two directions. */
static void apply_gaussian_blur(double horizontal, double vertical)
{
    GdkPixbuf *buf, *new;
    guchar *ps, *news;
    int *kernel_x, *kernel_y;
	int size;

    buf = PEEK(undo_stack, up);
	new = gdk_pixbuf_copy(buf);
    ps = gdk_pixbuf_get_pixels(buf);
    news = gdk_pixbuf_get_pixels(new);

	size = 31;

	/* Make size odd number. */
	size += size & 1;
	size -= 1;


	kernel_x = g_malloc(sizeof(int) * size);
	kernel_y = g_malloc(sizeof(int) * size);
    gaussian_kernel(horizontal, size, kernel_x);
    gaussian_kernel(vertical, size, kernel_y);

	gaussian_blur(size, size, kernel_x, kernel_y, 
                  news, ps, 
                  p_data.height, 
                  p_data.rowstride,
                  p_data.n_channels);

    g_free(kernel_x);
    g_free(kernel_y);

    UPDATE_EDITED_IMAGE(new);
}


static void apply_difference_of_gaussians(double first, double second)
{
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
    int min, max;
    int channel, x, y;
    int *filtered;
    int offset, value;
    GdkPixbuf *buf, *new;
    guchar *ps, *news;

    buf = PEEK(undo_stack, up);
    new = gdk_pixbuf_new(p_data.colorspace,
                         p_data.has_alpha,
                         p_data.bits_per_sample,
                         p_data.width,
                         p_data.height);

    ps = gdk_pixbuf_get_pixels(buf);
    news = gdk_pixbuf_get_pixels(new);
    h = gdk_pixbuf_get_height(buf);
    s = gdk_pixbuf_get_rowstride(buf);
    n_chans = gdk_pixbuf_get_n_channels(buf);


    g_print("Apply edge detection filter ");
    switch (method) {
        case SOBEL:
            g_print("Sobel");
            filtered = sobel(ps, h, s, n_chans);
            break;

        case PREWITT:
            g_print("Prewitt");
            filtered = prewitt(ps, h, s, n_chans);
            break;

        case LAPLACE:
            g_print("Laplace");
            filtered = laplace(ps, h, s, n_chans);
            break;

        default:
            g_print("Roberts cross");
            filtered = roberts_cross(ps, h, s, n_chans);
    }
    g_print("\n");

    for (channel = 0; channel < MIN(3, n_chans); channel++) {
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

    g_free(filtered);
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
            PUSH(undo_stack, up, u_start, buf);
            UPDATE_IMAGE();

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
        g_printerr("%s error saving image.\n", filename);
}
