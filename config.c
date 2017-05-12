/*
 * Pimply
 * by wisd0me ak47izatool@gmail.com
 */

//#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "pimply.h"
#include "config.h"

extern PimplyImage img;

struct config config;

static gint filelist_realloc(gint new_size) {
    gchar **p;
    if ( (p = (gchar **) g_realloc(config.filelist, sizeof(gchar) * new_size)) == NULL) {
        g_printerr("can't allocate more memory!\n");
        return(-1);
    }
    config.filelist = p;
    return(0);
}

static void parse_line(gchar *line) {
    const gchar *name = strtok(line, "=");
    const gchar *value = strtok(NULL, "=");

    if (name == NULL || value == NULL)
        return;

    if (strcmp(name, "mode") == 0) {
        config.mode = atoi(value);
    }
    else if (strcmp(name, "current") == 0) {
        config.current = g_strndup(value, strlen(value) - 1);
    }
    else if (strcmp(name, "file") == 0) {
        config.filelist[config.nfiles] = g_strndup(value, strlen(value) - 1);
        //g_print("Added %s [%d]\n", config.filelist[config.nfiles], config.nfiles);
        config.nfiles++;
    }
}

gint config_check() {
    //gchar *path = "config";
    FILE *fd;

    config.rcfile = g_strdup_printf("%s/pimplyrc", g_get_user_config_dir());
    config.mode = 0;
    config.current = NULL;
    config.filelist = NULL;

    fd = fopen(config.rcfile, "r");
    if (fd == NULL) {
        config.opened = FALSE;
        g_printerr("config not exists, I'll try to create one.\n");

        fd = fopen(config.rcfile, "w");
        if (fd == NULL) {
            g_printerr("can't open file %s for writing\n", config.rcfile);
            exit(1);
            //return(-1);
        }
        else
            fclose(fd);
    }
    else
        fclose(fd);

    //g_print("config %s exists\n", config.rcfile);
    return(0);
}

int config_read() {
    gchar str[BUFSIZ];
    gint allocated;
    FILE *fd = fopen(config.rcfile, "r");

    if (fd == NULL) {
        config.opened = FALSE;
        g_printerr("can't open config %s for reading\n", config.rcfile);
        return(-1);
    }

    config.opened = TRUE;
    config.nfiles = 0;
    config.filelist = (gchar **) g_malloc(32 * sizeof(gchar *));
    if (config.filelist == NULL) {
        g_printerr("can't allocate memory!\n");
        return(-1);
    }
    allocated = 32;

    while(fgets(str, sizeof(str), fd) != NULL) {
        //g_print("parsing line: %s\n", str);
        parse_line(str);
        if (config.nfiles == allocated) {
            g_print("config_read(): extending file list...");
            if (filelist_realloc(allocated + 16) == -1) {
                g_printerr("config_read(): can't allocate more memory!\n");
                fclose(fd);
                return(-1);
            }
            allocated += 16;
        }
    }
    config.filelist[config.nfiles] = NULL;

    fclose(fd);
    return(0);
}

int config_write_initial(char *current, int mode) {
    gchar *line;
    FILE *fd = fopen(config.rcfile, "w");

    if (fd == NULL) {
        g_printerr("can't open config %s for writing\n", config.rcfile);
        return(-1);
    }

    line = g_strdup_printf("mode=%d\n", mode);
    fputs(line, fd);
    g_free(line);

    line = g_strdup_printf("current=%s\n", current);
    fputs(line, fd);
    g_free(line);

    fclose(fd);
    return(0);
}

int config_append_file(char *image_name) {
    gchar *line;
    FILE *fd = fopen(config.rcfile, "a");

    if (fd == NULL) {
        g_printerr("can't open config %s for writing\n", config.rcfile);
        return(-1);
    }

    line = g_strdup_printf("file=%s\n", image_name);
    fputs(line, fd);
    g_free(line);

    fclose(fd);
    return(0);
}

static gboolean config_foreach_func (GtkTreeModel *model,
                                     GtkTreePath  *path,
                                     GtkTreeIter  *iter,
                                     gpointer      user_data) 
{
    gchar *file;
 
    gtk_tree_model_get(model, iter, 2, &file, -1);
    //g_print("config_foreach_func(): %s\n", file);
    /* append to tha config */
    config_append_file(file); 
    g_free(file);
    return(FALSE); /* do not stop walking the store, call us with next row */
}

/* Add new function config_write_list(), which will read
 * gtk_list_store n write a bunch of "file=..." strings 
 * to the file.
 */
gint config_write_list(GtkTreeModel *store, gchar *current, gint mode) {
    /* Write first part of da config: mode, current, ...
     * Append whole list with config_foreach_func()
     */
    config_write_initial(current, mode);
    gtk_tree_model_foreach(store, config_foreach_func, NULL);
    return(0);
}

void config_dump() {
    gint i;
    g_print("--dumping config file--\n");
    g_print("mode = %d\n", config.mode);
    g_print("current = %s\n", config.current);

    for (i = 0; config.filelist[i] != NULL; i++)
        g_print(" file = %s\n", config.filelist[i]);

    g_print("%d files total\n", config.nfiles);
    g_print("--end of config--\n");
}
