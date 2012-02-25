#ifndef GLOB_H
#define GLOB_H

#define STACK_SIZE 10
#define IS_EMPTY(stack, sp) (sp <= 0)
#define PUSH(stack, sp, buf) stack[sp++] = buf
#define PEEK(stack, sp) stack[sp - 1]
#define POP(stack, sp) stack[--sp]


#define GET_WIDGET_BY_ID(builder, id)                                        \
                     GTK_WIDGET(gtk_builder_get_object(builder, id))         \

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
    GtkWidget *edge_detect_combo_box;
    GtkWidget *edge_detect_dialog;
} AppWidgets;

#endif /* !GLOB_H */
