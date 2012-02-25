#ifndef GLOB_H
#define GLOB_H

#define STACK_SIZE 10
#define IS_EMPTY(stack, sp) (sp <= 0)
#define PUSH(stack, sp, buf) stack[sp++] = buf
#define PEEK(stack, sp) stack[sp - 1]
#define POP(stack, sp) stack[--sp]


#define GET_WIDGET_BY_ID(builder, id)                                        \
                     GTK_WIDGET(gtk_builder_get_object(builder, id))

#define SET_SENSITIVITY_DO_BUTTON(do_button, sensitive)                      \
    gtk_widget_set_sensitive(                                                \
             GTK_WIDGET(widgets.toolbutton_##do_button), sensitive)

#define UPDATE_EDITED_IMAGE(buf) {                                           \
    PUSH(undo_stack, up, buf);                                               \
    while (!IS_EMPTY(redo_stack, rp))                                        \
        gdk_pixbuf_unref(POP(redo_stack, rp));                                 \
    SET_SENSITIVITY_DO_BUTTON(redo, FALSE);                                  \
    UPDATE_IMAGE();                                                          \
}

#define UPDATE_IMAGE()                                                       \
    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets.img), PEEK(undo_stack, up))

typedef struct {
    GdkColorspace colorspace;
    gboolean has_alpha;
    int bits_per_sample;
    int width;
    int height;
} pixbuf_data;

typedef struct {
    GtkWidget *window;
    GtkWidget *img;
    GtkWidget *aboutdialog;

    GtkWidget *toolbutton_undo;
    GtkWidget *toolbutton_redo;

    GtkWidget *filechooser_open;
    GtkWidget *filechooser_save_as;


    GtkWidget *edge_detect_combo_box;
    GtkWidget *edge_detect_dialog;

    GtkWidget *gray_scale_combo_box;
    GtkWidget *gray_scale_dialog;
} AppWidgets;

#endif /* !GLOB_H */
