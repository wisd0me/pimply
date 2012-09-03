/*
 * Pimply
 * by Lil AZ ak47izatool@gmail.com
 */

#ifndef _CONFIG_H
#define _CONFIG_H

struct config {
    gint mode;
    gint nfiles;
    gchar **filelist;
    gchar *current;
    gchar *rcfile;
    gboolean opened;
} config;

gint config_check();
gint config_read();
gchar **config_read_list();
gint config_write_initial(gchar *current, gint mode);
gint config_write_list(GtkTreeModel *store, gchar *current, gint mode);
void config_dump();

#endif
