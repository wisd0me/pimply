/*
 * Pimply
 * Lil AZ ak47izatool@gmail.com
 */

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void destroy_signal(GtkWidget *widget, gpointer data);
void tree_row_selected(GtkWidget *widget, gpointer data);
void on_add_b(void);
void on_remove_b(void);
void on_open_b(GtkWidget *widget, GtkImage *image);
void on_apply_b();
void on_about_b(GtkWidget *widget, gpointer data);
gboolean on_key_press(GtkWidget *w, GdkEventKey *event, gpointer data);
void on_combo_changed(GtkComboBox *box, gpointer data);
void on_open_dialog_preview(GtkFileChooser *dialog, GtkWidget *preview);
gchar *pimply_open_dialog(gchar **file_name);

#endif
