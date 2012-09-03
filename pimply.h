/*
 * Pimply
 * by Lil AZ ak47izatool@gmail.com
 */

#ifndef _PIMPLY_H
#define _PIMPLY_H

#include <gtk/gtk.h>

#define VERSION "0.2-rc2"
#define NAME "Pimply"

/* main window */
typedef struct pimply_window {
    gboolean command_line;
    gint screen_w;
    gint screen_h;
    GdkWindow *desktop;

    gchar *title;
    gchar *last_folder;
    gint width;
    gint height;
    gint name_limit;

    /* width n height of GtkImage widget */
    gint gtk_img_width;
    gint gtk_img_height;
    gint gtk_preview_height;
    gint gtk_preview_width;

    GtkWidget *w;
    GtkWidget *sw;
    GtkWidget *tree;
    GtkTreeModel *model;
    GtkWidget *label;
    GtkWidget *combo;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *quit_b;
    GtkWidget *apply_b;
    GtkWidget *about_b;
    GtkWidget *add_b;
    GtkWidget *remove_b;
} PimplyWindow;

typedef struct pimply_image {
    gboolean opened;
    gchar *file_name;
    gchar *mode;
    gint mode_n;

    GdkGC *gc;
    GdkDrawable draw;
    GdkPixbuf *pb;
    GdkPixmap *pm;
    gint width;
    gint height;
} PimplyImage;

typedef struct draw_params {
    gint src_x, src_y;
    gint dest_x, dest_y;
    gint width, height;
} DrawParams;

PimplyImage *pimply_load_image(gchar *file_name);
void pimply_image_free(PimplyImage *image);
void pimply_set_background(PimplyImage *image);
gchar *truncate_file_name(gchar *name);

#endif
