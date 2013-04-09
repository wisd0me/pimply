/*
 * Pimply 
 * by Lil AZ ak47izatool@gmail.com
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "pimply.h"
#include "config.h"
#include "callbacks.h"

extern PimplyWindow pimply;

gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(pimply.tree));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    //g_print("Delete event\n");
    return(FALSE);
}

void destroy_signal(GtkWidget *widget, gpointer data) {
    //g_print("Destroy signal\n");
    gtk_main_quit();
}

void tree_row_selected(GtkWidget *widget, gpointer data) {
    GtkTreeView *treeview = data;
    GtkTreeSelection *ts = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *str;

    if (gtk_tree_selection_get_selected(ts, &model, &iter)) {
        gchar *path;
        gchar *label_text;
        GdkPixbuf *pb;
        gint width, height;

        gtk_tree_model_get(model, &iter, 0, &pb, 1, &str, 2, &path, -1);
        //g_print("selected row: %s (%s)\n", str, path);
        if (gdk_pixbuf_get_file_info(path, &width, &height) != NULL) {
            label_text = g_strdup_printf("%s (%dx%d)", str, width, height);
            gtk_label_set_label(GTK_LABEL(pimply.label), label_text);
            g_free(label_text);
        }
        else
            gtk_label_set_label(GTK_LABEL(pimply.label), str);
    }
}

void on_add_b(void) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(pimply.tree));
    GtkTreeIter iter;
    PimplyImage *image;
    gchar *file_name, *base_name;

    if (pimply_open_dialog(&file_name) != NULL) {
        //g_print("loaded: %s\n", file_name);
        if ( (image = pimply_load_image(file_name)) == NULL)
            return;

        GtkTreePath *last_node;

        g_free(file_name);
        base_name = g_path_get_basename(image->file_name);
        file_name = truncate_file_name(base_name);
        gtk_list_store_append(GTK_LIST_STORE(model), &iter);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, image->pb, 1, file_name, 2, image->file_name, -1);
        g_free(base_name);
        /* select added row */
        last_node = gtk_tree_model_get_path(model, &iter);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(pimply.tree), last_node, NULL, FALSE);
        gtk_widget_grab_focus(pimply.apply_b);
        /* update config here */
        config_write_list(model, config.current, config.mode);
        g_slice_free(PimplyImage, image);
        //pimply_image_free(image);
        gtk_tree_path_free(last_node);
    }
}

void on_remove_b(void) {
    GtkTreeSelection *ts = gtk_tree_view_get_selection(GTK_TREE_VIEW(pimply.tree));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(ts, &model, &iter)) {
        GtkTreePath *prev_node = gtk_tree_model_get_path(model, &iter);
        if (gtk_tree_path_prev(prev_node)) {
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(pimply.tree), prev_node, NULL, FALSE);
            gtk_tree_path_free(prev_node);
        }
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
        /* update config here */
        config_write_list(model, config.current, config.mode);
    }
}

void on_apply_b() {
    GtkTreeSelection *ts = gtk_tree_view_get_selection(GTK_TREE_VIEW(pimply.tree));
    GtkTreeModel *model;
    GtkTreeIter iter;
    PimplyImage *image; 
    gchar *file;

    if (gtk_tree_selection_get_selected(ts, &model, &iter)) {
        gtk_tree_model_get(model, &iter, 2, &file, -1);
        //g_print("* Applyin %s\n", file);
    }
    else return;

    if ( (image = pimply_load_image(file)) != NULL) {
        image->mode = gtk_combo_box_get_active_text(GTK_COMBO_BOX(pimply.combo));
        image->mode_n = gtk_combo_box_get_active(GTK_COMBO_BOX(pimply.combo));
        //g_print("'%s' [%d] mode selected\n", image->mode, image->mode_n);
        pimply_set_background(image);
        //g_print("config.current = %s %x\n", config.current, config.current);
        //g_free(config.current);
        g_free(config.current);
        config.current = g_strdup(image->file_name);
        config.mode = gtk_combo_box_get_active(GTK_COMBO_BOX(pimply.combo));
        config_write_list(model, image->file_name, config.mode);
        g_slice_free(PimplyImage, image);
        //gtk_widget_set_sensitive(pimply.apply_b, FALSE);
        gtk_widget_grab_focus(pimply.add_b);
    }
    return;
}

void on_about_b(GtkWidget *widget, gpointer data) {
    GtkWidget *about;
    //const gchar *authors[] = {"Lil AZ <ak47izatool@gmail.com>", NULL};

    about = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), NAME);
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), VERSION);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "Written by Lil AZ <ak47izatool@gmail.com>");
    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), "Distributed under the terms of GPLv2 n all dat...");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), "Pimply - GTK2 wallpaper changer");
    //gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), authors);
    //gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about), "Lil AZ\nRichard M. Stallman");

    g_signal_connect_swapped(G_OBJECT(about), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(about));
    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy(about);
}

void on_combo_changed(GtkComboBox *box, gpointer data) {
    /*
    if (gtk_combo_box_get_active(box) != img.mode_n && img.opened)
        gtk_widget_set_sensitive(pimply.apply_b, TRUE);
    */
    return;
}

gboolean on_key_press(GtkWidget *w, GdkEventKey *event, gpointer data) {
    if (event->type == GDK_KEY_PRESS) {
        if (event->keyval == GDK_Escape) {
            //g_print("key press event activated\n");
            /* How couldn't I found dis myself :-\ */
            gtk_widget_destroy(pimply.w);
        }
    }

    return(FALSE);
}

gchar *pimply_open_dialog(gchar **file_name) {
    GtkWidget *open_dialog;
    GtkWidget *preview;
    GtkFileFilter *filter;
    gchar *tmp = NULL;

    open_dialog = gtk_file_chooser_dialog_new("Add File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                              GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                              NULL);
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Supported image formats");
    gtk_file_filter_add_pixbuf_formats(filter);
    /*
      gtk_file_filter_add_pattern(filter, "*.png");
      gtk_file_filter_add_pattern(filter, "*.jpg");
      gtk_file_filter_add_pattern(filter, "*.gif");
      gtk_file_filter_add_pattern(filter, "*.bmp");
    */
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), filter);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_dialog), pimply.last_folder);
    //g_print("last folder: '%s'\n", pimply.last_folder);

    preview = gtk_image_new();
    gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(open_dialog), preview);
    //gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(open_dialog), TRUE);
    g_signal_connect(G_OBJECT(open_dialog), "update-preview", G_CALLBACK(on_open_dialog_preview), preview);

    *file_name = NULL;
    if (gtk_dialog_run(GTK_DIALOG(open_dialog)) == GTK_RESPONSE_ACCEPT) {
        *file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_dialog));
        if ( (tmp = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(open_dialog))) != NULL) {
            pimply.last_folder = tmp;
        }
    }

    g_object_unref(preview);
    gtk_widget_destroy(open_dialog);
    return(*file_name);
}

void on_open_dialog_preview(GtkFileChooser *dialog, GtkWidget *preview) {
    gchar *file_name = gtk_file_chooser_get_filename(dialog);
    gint width, height;

    /* check if it's file via stat() or some shit,
     * n if it's not - clear preview widget.
     */  

    if (file_name == NULL || gdk_pixbuf_get_file_info(file_name, &width, &height) == NULL) {
        //g_print("error loading file '%s' for preview\n", file_name);
        return;
    }
    else {
        GdkPixbuf *pb;
        //g_print("previewing %s (%dx%d)\n", file_name, width, height);

        if (width <= pimply.gtk_preview_width && height <= pimply.gtk_preview_height)
            pb = gdk_pixbuf_new_from_file(file_name, NULL);
        else
            pb = gdk_pixbuf_new_from_file_at_scale(file_name, pimply.gtk_preview_width, pimply.gtk_preview_height, TRUE, NULL);

        gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pb);
        g_object_unref(pb);
        g_free(file_name);
    }
}
