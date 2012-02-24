#ifndef CALLBACK_H
#define CALLBACK_H

#include <gtk/gtk.h>

G_MODULE_EXPORT void
on_filter_edge_detect_activate(GtkWidget *widget, gpointer data);

G_MODULE_EXPORT void
on_about_activate(GtkWidget *widget, gpointer data);

G_MODULE_EXPORT void
on_menu_open_activate(GtkWidget *widget, gpointer data);

G_MODULE_EXPORT void
on_menu_save_as_activate(GtkWidget *widget, gpointer data);

G_MODULE_EXPORT void
on_menu_save_activate(GtkWidget *widget, gpointer data);

#endif /* !CALLBACK_H */
