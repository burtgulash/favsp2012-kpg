#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>


static GtkWidget *canvas, *window;
static GdkPixbuf *buf[2];
static int b = 0;
static int loaded = 0;
static int w = 0, h = 0;

static void apply_sobel()
{
    int new_b, rs, r_rs, n_channels;
    int r, c, channel;
    int g, gx, gy;
    int min, max, *tmp;
    guchar *ns, *ps, *p;

    new_b = b ^ 1;
    n_channels = gdk_pixbuf_get_n_channels(buf[b]);
    rs = gdk_pixbuf_get_rowstride(buf[b]);

    ps = gdk_pixbuf_get_pixels(buf[b]);
    ns = gdk_pixbuf_get_pixels(buf[new_b]);
    tmp = (int*) malloc(sizeof(int) * w * h * n_channels);

    for (channel = 0; channel < 3; channel++) {
        min = max = 0;

        for (r = 1; r < h - 1; r++) {
            r_rs = r * rs;
            for (c = n_channels; c < rs - n_channels; c += n_channels) {
                p = ps + r_rs + c + channel;

                gx = 2 * p[n_channels]
                   - 2 * p[-n_channels]
                   +     p[-rs + n_channels]
                   +     p[+rs - n_channels]
                   -     p[-rs - n_channels]
                   -     p[+rs - n_channels];

                gy = 2 * p[+rs]
                   - 2 * p[-rs]
                   +     p[+rs + n_channels]
                   +     p[+rs - n_channels]
                   -     p[-rs + n_channels]
                   -     p[-rs - n_channels];

                g = (int) sqrt((double) (gx * gx + gy * gy));
                tmp[r_rs + c + channel] = g;

                if (g > max)
                    max = g;
                if (g < min)
                    min = g;
            }
        }



        for (r = 1; r < h - 1; r++) {
            r_rs = r * rs;
            for (c = n_channels; c < rs - n_channels; c += n_channels) {
                ns[r_rs + c + channel] =
                    ((tmp[r_rs + c + channel] - min) * 0xFF) / (max - min);
            }
        }
    }

    free(tmp);
    b = new_b;
}


G_MODULE_EXPORT void
on_sobel_clicked(GtkWidget *widget, gpointer data)
{
    if (loaded) {
        apply_sobel();
        gtk_image_set_from_pixbuf(GTK_IMAGE(canvas), buf[b]);
    }
}


G_MODULE_EXPORT void
on_ulozit_clicked(GtkWidget *widget, gpointer data)
{
    char *filename;
    GtkResponseType response;
    GtkWidget *dialog;
    GError *err;

    err = NULL;
    dialog = gtk_file_chooser_dialog_new("Otevřít obrázek",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "zrušit", GTK_RESPONSE_CANCEL,
                                         "budiž", GTK_RESPONSE_ACCEPT,
                                         NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (err == NULL)
            gdk_pixbuf_save(buf[b], filename, "png", &err, NULL);
        else
            fprintf(stderr, "%s error saving image.\n", filename);
    }

    gtk_widget_destroy(dialog);
}


G_MODULE_EXPORT void
on_otevrit_clicked(GtkWidget *widget, gpointer data)
{
    char *filename;
    GtkResponseType response;
    GtkWidget *dialog;
    GError *err;

    err = NULL;
    dialog = gtk_file_chooser_dialog_new("Otevřít obrázek",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "zrušit", GTK_RESPONSE_CANCEL,
                                         "budiž", GTK_RESPONSE_ACCEPT,
                                         NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        buf[b] = gdk_pixbuf_new_from_file(filename, &err);

        if (err == NULL) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(canvas), buf[b]);
            assert(buf[b] != NULL);

            w = gdk_pixbuf_get_width(buf[b]);
            h = gdk_pixbuf_get_height(buf[b]);

            buf[b ^ 1] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
            assert(buf[b ^ 1] != NULL);

            loaded = 1;
        } else
            fprintf(stderr, "%s is not an image.\n", filename);
    }

    gtk_widget_destroy(dialog);
}


int main(int argc, char **argv)
{
    GtkBuilder *builder;
    GError *err;

    gtk_init(&argc, &argv);

    err = NULL;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "up.glade", &err);
    if (err != NULL) {
        fprintf(stderr, "%s: Can not load gui.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    canvas = GTK_WIDGET(gtk_builder_get_object(builder, "canvas"));

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);


    gtk_widget_show(window);
    gtk_main();


    return 0;
}
