/*
 * Pimply 
 * by Lil AZ ak47izatool@gmail.com
 */

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "pimply.h"
#include "callbacks.h"
#include "config.h"

/* TODO: global object was a misconception, and need to be handled */
PimplyWindow pimply;

/* used for "Centered" mode */
DrawParams *calculate_draw_params_center(DrawParams *draw_parm, gint width, gint height) {
    if (draw_parm == NULL)
        return(NULL);

    if (width <= pimply.screen_w) {
        draw_parm->src_x = 0;
        draw_parm->dest_x = (pimply.screen_w - width)/2;
        draw_parm->width = width;
    }
    else {
        draw_parm->src_x = (width - pimply.screen_w)/2;
        draw_parm->dest_x = 0;
        draw_parm->width = pimply.screen_w;
    }

    if (height <= pimply.screen_h) {
        draw_parm->src_y = 0;
        draw_parm->dest_y = (pimply.screen_h - height)/2;
        draw_parm->height = height;
    }
    else {
        draw_parm->src_y = (height - pimply.screen_h)/2;
        draw_parm->dest_y = 0;
        draw_parm->height = pimply.screen_h;
    }
    return(draw_parm);
}

/* used for "Scaled (keep aspect)" mode */
DrawParams *calculate_draw_params_at_scale(DrawParams *draw_parm, gint width, gint height) {
    if (draw_parm == NULL)
        return(NULL);

    if (width <= pimply.screen_w) {
        draw_parm->src_x = 0;
        draw_parm->dest_x = (pimply.screen_w - width)/2;
        draw_parm->width = width;
    }
    else {
        draw_parm->src_x = (width - pimply.screen_w)/2;
        draw_parm->dest_x = 0;
        draw_parm->width = pimply.screen_w;
    }

    if (height <= pimply.screen_h) {
        draw_parm->src_y = 0;
        draw_parm->dest_y = (pimply.screen_h - height)/2;
        draw_parm->height = height;
    }
    else {
        draw_parm->src_y = (height - pimply.screen_h)/2;
        draw_parm->dest_y = 0;
        draw_parm->height = pimply.screen_h;
    }
    return(draw_parm);
}

void pimply_set_background(PimplyImage *image) {
    DrawParams dp;

    /* Scaled */
    if (image->mode_n == 0) {
        //g_print("Scaled mode\n");
        image->pb = gdk_pixbuf_new_from_file_at_scale(image->file_name, pimply.screen_w, pimply.screen_h, FALSE, NULL);
        gdk_pixbuf_render_pixmap_and_mask(image->pb, &image->pm, NULL, 1);
    }
    /* Scaled (keep aspect) */
    else if (image->mode_n == 1) {
        //g_print("Proportional scaled mode\n");
        GdkPixbuf *temp;
        gint w, h;

        image->gc = gdk_gc_new(pimply.desktop);
        //g_print("pimply_set_scaled_p(): loading %s\n", image->file_name);
        image->pm = gdk_pixmap_new(GDK_DRAWABLE(pimply.desktop), pimply.screen_w, pimply.screen_h, -1);
        image->pb = gdk_pixbuf_new_from_file_at_scale(image->file_name, pimply.screen_w, pimply.screen_h, TRUE, NULL);
        w = gdk_pixbuf_get_width(image->pb);
        h = gdk_pixbuf_get_height(image->pb);
        //g_print("scaled image %dx%d\n", w, h);
        calculate_draw_params_at_scale(&dp, w, h);

        temp = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, pimply.screen_w, pimply.screen_h);
        gdk_pixbuf_fill(temp, 0x000000);
        gdk_draw_pixbuf(GDK_DRAWABLE(image->pm), image->gc, temp, 0, 0, 0, 0, pimply.screen_w, pimply.screen_h, GDK_RGB_DITHER_MAX, 0, 0);
        g_object_unref(temp);
        gdk_draw_pixbuf(GDK_DRAWABLE(image->pm), image->gc, image->pb, dp.src_x, dp.src_y, dp.dest_x, dp.dest_y, w, h, GDK_RGB_DITHER_MAX, 0, 0);
        //g_print("img(%d,%d) : w %d, h %d | screen(%d,%d)\n", dp.src_x, dp.src_y, dp.width, dp.height, dp.dest_x, dp.dest_y);
    }
    /* Tiled */
    else if (image->mode_n == 2) {
        //g_print("Tiled mode\n");
        image->pb = gdk_pixbuf_new_from_file(image->file_name, NULL);
        gdk_pixbuf_render_pixmap_and_mask(image->pb, &image->pm, NULL, 1);
    }
    /* Centered */
    else if (image->mode_n == 3) {
        //g_print("Centered mode\n");
        GdkPixbuf *temp;

        image->pm = gdk_pixmap_new(GDK_DRAWABLE(pimply.desktop), pimply.screen_w, pimply.screen_h, -1);
        image->pb = gdk_pixbuf_new_from_file(image->file_name, NULL);
        temp = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, pimply.screen_w, pimply.screen_h);
        gdk_pixbuf_fill(temp, 0x000000);

        calculate_draw_params_center(&dp, image->width, image->height);
        /* g_print("\n\nCalculated DP:\nsrc_x = %d, src_y = %d, dest_x = %d, dest_y = %d, width = %d, height = %d\n",
           dp.src_x, dp.src_y, dp.dest_x, dp.dest_y, dp.width, dp.height); */
        gdk_draw_pixbuf(GDK_DRAWABLE(image->pm), image->gc, temp, 0, 0, 0, 0, pimply.screen_w, pimply.screen_h, GDK_RGB_DITHER_MAX, 0, 0);
        gdk_draw_pixbuf(GDK_DRAWABLE(image->pm), image->gc, image->pb, dp.src_x, dp.src_y, dp.dest_x, dp.dest_y, dp.width, dp.height, GDK_RGB_DITHER_MAX, 0, 0);
        g_object_unref(temp);
    }
    gdk_window_set_back_pixmap(pimply.desktop, image->pm, FALSE);

    /* Folowing code was STOLEN!!1 from qiv :\ */
    Display *display = GDK_DISPLAY();
    Window xwin = RootWindow(display, DefaultScreen(display));
    Atom prop_root = XInternAtom(display, "_XROOTPMAP_ID", False),
        prop_esetroot = XInternAtom(display, "ESETROOT_PMAP_ID", False),
        xa_pixmap = XInternAtom(display, "PIXMAP", True), prop_type;
    unsigned char *data_root, *data_esetroot;
    int prop_format;
    unsigned long prop_length, after;
    Pixmap pixmap_id = GDK_WINDOW_XWINDOW(image->pm);

    data_root = data_esetroot = NULL;
    /* test whether an existing client should be killed */
    XGetWindowProperty(display, xwin,
                       prop_root, 0, 1, False, AnyPropertyType,
                       &prop_type, &prop_format, &prop_length, &after,
                       &data_root);
    if (prop_type == xa_pixmap) {
        XGetWindowProperty(display, xwin,
                           prop_esetroot, 0, 1, False, AnyPropertyType,
                           &prop_type, &prop_format, &prop_length, &after,
                           &data_esetroot);
        /* If data structures match the client can be safely killed. In case of
           data structure mismatch just ignore the client since it possibly could
           be the window manager. Memory should not be reclaimed in this case. */
        if (data_root && data_esetroot && prop_type == xa_pixmap &&
            *((Pixmap*) data_root) == *((Pixmap*) data_esetroot))
            /* Do NOT kill any clients. It seems that GDK is already taking care
               of killing the pixmap if set with gdk_window_set_back_pixmap.
               XKillClient(display, *((Pixmap*)data_root)) */;
    }

    /* really change the property now */
    XDeleteProperty(display, xwin, prop_root);
    XChangeProperty(display, xwin, prop_root, xa_pixmap, 32,
                    PropModeReplace, (unsigned char*)&pixmap_id, 1);
    XDeleteProperty(display, xwin, prop_esetroot);
    XChangeProperty(display, xwin, prop_esetroot, xa_pixmap, 32,
                    PropModeReplace, (unsigned char*)&pixmap_id, 1);
    XSetCloseDownMode(display, RetainPermanent);
    if (data_root) XFree(data_root);
    if (data_esetroot) XFree (data_esetroot);

    gdk_window_clear(pimply.desktop);
    gdk_flush();

    g_object_unref(image->pb);
    g_object_unref(image->pm);
}

/* This is more correct function */
PimplyImage *pimply_load_image(gchar *file) {
    PimplyImage *image = g_slice_new(PimplyImage);

    if (image == NULL || file == NULL)
        return(NULL);

    image->file_name = g_strdup(file);
    //g_print("load_image_true(): file_name %s\n", image->file_name);
    if (gdk_pixbuf_get_file_info(image->file_name, &image->width, &image->height) == NULL) {
        g_print("error opening image '%s'\n", image->file_name);
        g_free(image->file_name);
        g_slice_free(PimplyImage, image);
        image = NULL;
    }
    else {
        image->opened = TRUE;
        if (image->width <= pimply.gtk_img_width && image->height <= pimply.gtk_img_height)
            image->pb = gdk_pixbuf_new_from_file(image->file_name, NULL);
        else
            image->pb = gdk_pixbuf_new_from_file_at_scale(image->file_name, pimply.gtk_img_width, pimply.gtk_img_height, TRUE, NULL);
    }

    return(image);
}

void pimply_image_free(PimplyImage *image) {
    if (image == NULL)
        return;

    /*
    if (G_IS_OBJECT(image->pb))
        g_object_unref(image->pb);
    if (G_IS_OBJECT(image->pm))
        g_object_unref(image->pm);
    if (G_IS_OBJECT(image->file_name))
        g_free(image->file_name);
    */
    if (image->pb != NULL)
        g_object_unref(image->pb);
    if (image->pm != NULL)
        g_object_unref(image->pm);
    if (image->file_name != NULL)
        g_free(image->file_name);

    g_slice_free(PimplyImage, image);
}

/* it's only used for display */
gchar *truncate_file_name(gchar *name) {
    gint sz = strlen(name);

    if (sz <= pimply.name_limit)
        return(name);

    gint pos;
    //if (sz > pimply.name_limit && (sz - pimply.name_limit) > 3)
    pos = pimply.name_limit - 3;
    name[pos] = '.';
    name[pos+1] = '.';
    name[pos+2] = '.';
    name[pos+3] = '\0';
    //g_print("truncated name: %s\n", name);
    return(name);
}

/* images must be finished with NULL */
GtkTreeModel *create_model(gchar **images) {
    GtkListStore *store;
    GtkTreeIter iter;
    gint i;
    gchar *name, *basename;
    PimplyImage *image;

    /* Third column should be a PimplyImage structure! */
    store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

    for (i = 0; images[i] != NULL; i++) {
        /* skip bad or not existent files */
        if ( (image = pimply_load_image(images[i])) == NULL)
            continue;
        basename = g_path_get_basename(images[i]);
        name = truncate_file_name(basename);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, image->pb, 1, name, 2, images[i], -1);
        g_free(basename);
        g_free(image->file_name);
        g_object_unref(image->pb);
        g_slice_free(PimplyImage, image); /* under question */
    }

    return(GTK_TREE_MODEL(store));
}

void add_columns(GtkTreeView *tree) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    //GtkTreeModel *model = gtk_tree_view_get_model(tree);

    /* first column with image */
    renderer = gtk_cell_renderer_pixbuf_new();
    //column = gtk_tree_view_column_new_with_attributes("Image", renderer, "spacing", 8, NULL);
    column = gtk_tree_view_column_new_with_attributes("Image", renderer, "pixbuf", 0, NULL); /* Damn, I'm just fuckin shocked */
    //gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column), GTK_TREE_VIEW_COLUMN_FIXED);
    //gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 96);
    gtk_tree_view_append_column(tree, column);

    /* second column with file name */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("File name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(tree, column);
}

static gboolean tree_find_current (GtkTreeModel *model,
                                   GtkTreePath  *path,
                                   GtkTreeIter  *iter,
                                   gpointer      user_data) 
{
    gchar *file;
 
    gtk_tree_model_get(model, iter, 2, &file, -1);
    //g_print("config_foreach_func(): %s\n", file);
    if (g_strcmp0(file, config.current) == 0) {
        //g_print("Current %s found!!1 Now I should only select it!\n", file);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(pimply.tree), gtk_tree_model_get_path(model, iter), NULL, FALSE);
        return(TRUE);
    }
    g_free(file);
    return(FALSE); /* do not stop walking the store, call us with next row */
}

void pimply_init() {
    pimply.command_line = FALSE;
    pimply.title = NAME;
    pimply.width = 350;
    pimply.height = 400;

    if (config.current) pimply.last_folder = g_path_get_dirname(config.current);
    else pimply.last_folder = getenv("HOME");

    pimply.desktop = gdk_get_default_root_window();
    pimply.screen_w = gdk_screen_width();
    pimply.screen_h = gdk_screen_height();

    pimply.name_limit = 35;
    pimply.gtk_img_width = 96;
    pimply.gtk_img_height = 96;
    pimply.gtk_preview_height = 128;
    pimply.gtk_preview_width = 128;


    /*
    img.file_name = NULL;
    img.mode = NULL;
    img.mode_n = 0;
    img.opened = FALSE;
    img.gc = gdk_gc_new(pimply.desktop);
    */
}

/* Don't forget to add horizontal(and vertical?) flip CheckButton (combobox?)!!!oneone */
void pimply_create_window() {
    /* window */
    pimply.w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(pimply.w), pimply.width, pimply.height);
    //gtk_window_set_resizable(GTK_WINDOW(pimply.w), FALSE);
    gtk_window_set_title(GTK_WINDOW(pimply.w), pimply.title);
    gtk_container_set_border_width(GTK_CONTAINER(pimply.w), 4);
    g_signal_connect(G_OBJECT(pimply.w), "delete_event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(G_OBJECT(pimply.w), "destroy", G_CALLBACK(destroy_signal), NULL);
    /* gtk_widget_add_events(pimply.w, GDK_BUTTON_PRESS_MASK); I don't know if it's needed or not */
    g_signal_connect(G_OBJECT(pimply.w), "key-press-event", G_CALLBACK(on_key_press), NULL);
    
    //pimply.image = gtk_image_new();

    /* add scrolled feature */
    pimply.sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(pimply.sw), GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pimply.sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    /* buttons */
    pimply.quit_b = gtk_button_new_from_stock(GTK_STOCK_QUIT);
    g_signal_connect_swapped(G_OBJECT(pimply.quit_b), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(pimply.w));
    g_object_set(G_OBJECT(pimply.quit_b), "width-request", 24, NULL);

    pimply.about_b = gtk_button_new_from_stock(GTK_STOCK_ABOUT);
    g_signal_connect(G_OBJECT(pimply.about_b), "clicked", G_CALLBACK(on_about_b), NULL);
    g_object_set(G_OBJECT(pimply.about_b), "width-request", 24, NULL);

    pimply.apply_b = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    g_signal_connect(G_OBJECT(pimply.apply_b), "clicked", G_CALLBACK(on_apply_b), NULL);
    //gtk_widget_set_sensitive(pimply.apply_b, FALSE);
    g_object_set(G_OBJECT(pimply.apply_b), "width-request", 24, NULL);

    pimply.add_b = gtk_button_new_from_stock(GTK_STOCK_ADD);
    g_signal_connect(G_OBJECT(pimply.add_b), "clicked", G_CALLBACK(on_add_b), NULL);
    g_object_set(G_OBJECT(pimply.add_b), "width-request", 24, "height-request", 8, NULL);

    pimply.remove_b = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
    g_signal_connect(G_OBJECT(pimply.remove_b), "clicked", G_CALLBACK(on_remove_b), NULL);
    g_object_set(G_OBJECT(pimply.remove_b), "width-request", 24, "height-request", 8, NULL);

    pimply.label = gtk_label_new("Wussup");

    pimply.combo = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(pimply.combo), "Scaled");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pimply.combo), "Scaled (keep aspect)");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pimply.combo), "Tiled");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pimply.combo), "Centered");
    gtk_combo_box_set_active(GTK_COMBO_BOX(pimply.combo), config.mode);
    //g_print("combo active %d\n", gtk_combo_box_get_active(GTK_COMBO_BOX(pimply.combo)));
    //g_signal_connect(G_OBJECT(pimply.combo), "changed", G_CALLBACK(on_combo_changed), NULL);
    //g_object_set(G_OBJECT(pimply.combo), "width-request", 64, "height-request", 24, NULL);

    /* containers */
    pimply.hbox = gtk_hbutton_box_new();
    //g_object_set(G_OBJECT(pimply.hbox), "resize-mode", GTK_RESIZE_QUEUE, "border-width", 0, NULL);
    gtk_box_set_spacing(GTK_BOX(pimply.hbox), 2);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(pimply.hbox), GTK_BUTTONBOX_CENTER);
    gtk_box_pack_start(GTK_BOX(pimply.hbox), pimply.add_b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pimply.hbox), pimply.remove_b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pimply.hbox), pimply.apply_b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pimply.hbox), pimply.about_b, FALSE, FALSE, 0);
    //gtk_box_pack_start(GTK_BOX(pimply.hbox), pimply.quit_b, FALSE, FALSE, 0);

    /* TreeView list */
    pimply.model = create_model(config.filelist);
    pimply.tree = gtk_tree_view_new_with_model(pimply.model);
    gtk_tree_model_foreach(pimply.model, tree_find_current, NULL);
    g_object_unref(pimply.model);
    add_columns(GTK_TREE_VIEW(pimply.tree));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(pimply.tree), FALSE);
    g_signal_connect(G_OBJECT(pimply.tree), "cursor-changed", G_CALLBACK(tree_row_selected), pimply.tree);

    gtk_container_add(GTK_CONTAINER(pimply.sw), pimply.tree);

    pimply.vbox = gtk_vbox_new(FALSE, 4);
    gtk_container_add(GTK_CONTAINER(pimply.vbox), pimply.sw);
    gtk_container_add(GTK_CONTAINER(pimply.vbox), pimply.label);
    gtk_box_set_child_packing(GTK_BOX(pimply.vbox), pimply.label, FALSE, FALSE, 2, GTK_PACK_START);
    gtk_container_add(GTK_CONTAINER(pimply.vbox), pimply.combo);
    gtk_box_set_child_packing(GTK_BOX(pimply.vbox), pimply.combo, FALSE, FALSE, 2, GTK_PACK_START);
    gtk_container_add(GTK_CONTAINER(pimply.vbox), pimply.hbox);
    gtk_box_set_child_packing(GTK_BOX(pimply.vbox), pimply.hbox, FALSE, FALSE, 2, GTK_PACK_START);

    gtk_container_add(GTK_CONTAINER(pimply.w), pimply.vbox);
    gtk_widget_grab_focus(pimply.add_b);
}

void pimply_restore() {
    if (config.opened && config.current) {
        gchar *label_text, *basename;
        gint width, height;

        //g_print("--- pimply_init() ---\n");
        if (gdk_pixbuf_get_file_info(config.current, &width, &height) == NULL) {
            g_print("error opening image '%s'\n", config.current);
            return;
        }
        basename = truncate_file_name(g_path_get_basename(config.current));
        gtk_combo_box_set_active(GTK_COMBO_BOX(pimply.combo), config.mode);
        //g_print("image: %s size: %dx%d; basename: %s\n", config.current, width, height, basename);
        label_text = g_strdup_printf("%s (%dx%d)", basename, width, height);
        gtk_label_set_label(GTK_LABEL(pimply.label), label_text);
        g_free(basename);
        g_free(label_text);
        //g_print("--- pimply_init() end ---\n");
    }
}

void usage(char *name) {
    g_print("usage: %s [OPTION]\n\n", name);
    g_print("  --help, -h\tdisplay this message and exit\n");
    g_print("  --version, -v\tversion and author information\n");
    g_print("  --restore, -r\trestore previously set wallpaper\n");
    g_print("  --dump, -d\tdump config to stdout\n");
}

void parseargs(int argc, char **argv) {
    int opt_index = 0;
    int c;
    struct option long_opts[] = {
        {"help",        no_argument, 0, 'h'},
        {"version",     no_argument, 0, 'v'},
        {"restore",     no_argument, 0, 'r'},
        {"dump",        no_argument, 0, 'd'},
        {NULL, 0, NULL, 0}
    };

    while(1) {
        c = getopt_long(argc, argv, "hvrd", long_opts, &opt_index);
        /* Detect the end of options */
        if (c == -1)
            break;

        switch(c) {
        case 0:
            g_print("%s given\n", long_opts[opt_index].name);
            break;
        case 'h':
            usage(argv[0]);
            break;
        case 'v':
            g_print("%s %s by AZ\n", NAME, VERSION);
            g_print("Feel free to send feedback to ak47izatool@gmail.com\n");
            break;
        case 'r':
            if (!config.opened || !config.current) {
                g_print("Can't restore: config not present or incorrect!\n");
                exit(1);
            }
            if (config.mode <= -1 || config.mode > 3) {
                g_print("Can't restore: config is incorrect!\n");
                exit(1);
            }

            PimplyImage image;
            image.file_name = config.current;
            image.mode_n = config.mode;
            if (gdk_pixbuf_get_file_info(image.file_name, &image.width, &image.height) == NULL) {
                g_print("can't load image %s!\n", image.file_name);
                exit(1);
            }

            pimply_set_background(&image);
            break;
        case 'd':
            config_dump();
            break;
        }
    }
    exit(0);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    if (config_check() != -1)
        config_read();

    pimply_init();

    if (argc > 1)
        parseargs(argc, argv);

    pimply_create_window();
    if (config.opened) pimply_restore();

    gtk_widget_show_all(pimply.w);
    gtk_main();

    return(0);
}
