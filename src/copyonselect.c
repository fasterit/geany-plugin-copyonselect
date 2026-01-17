/*
 * Copy on Select - Geany Plugin
 * 
 * Copyright (C) 2026 Faster IT GmbH
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <gdk/gdk.h>
#include <glib.h>
#include <geanyplugin.h>

GeanyPlugin *geany_plugin;
GeanyData *geany_data;

PLUGIN_VERSION_CHECK(GEANY_API_VERSION)

PLUGIN_SET_INFO("Copy on Select",
                "Automatically copies selected text to X selection for improved middle-click paste",
                "1.0",
                "Faster IT GmbH")

/* Callback for editor notification events */
static gboolean on_editor_notify(GObject *obj, GeanyEditor *editor,
                                  SCNotification *nt, gpointer user_data)
{
    /* Silence unused parameter warnings */
    (void)obj;
    (void)user_data;
    
    /* Check if this is a selection update notification */
    if (nt->nmhdr.code == SCN_UPDATEUI && 
        (nt->updated & SC_UPDATE_SELECTION))
    {
        gint start_pos, end_pos;
        gchar *text;
        GtkClipboard *clipboard;
        
        /* Get selection start and end positions */
        start_pos = sci_get_selection_start(editor->sci);
        end_pos = sci_get_selection_end(editor->sci);
        
        /* Only copy if there is an actual selection */
        if (start_pos != end_pos)
        {
            /* Get the selected text */
            text = sci_get_contents_range(editor->sci, start_pos, end_pos);
            
            if (text != NULL)
            {
                /* Get the X primary selection clipboard */
                clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
                
                /* Set the text to the clipboard (-1 = null-terminated string) */
                gtk_clipboard_set_text(clipboard, text, -1);
                
                /* Store the clipboard data to make it available to other apps */
                gtk_clipboard_store(clipboard);
                
                /* Free the allocated text */
                g_free(text);
            }
        }
    }
    
    return FALSE;
}

/* Plugin initialization */
void plugin_init(GeanyData *data)
{
    /* Silence unused parameter warning */
    (void)data;
    
    /* Connect to the editor notification signal */
    plugin_signal_connect(geany_plugin, NULL, "editor-notify", 
                          FALSE, G_CALLBACK(on_editor_notify), NULL);
}

/* Plugin cleanup */
void plugin_cleanup(void)
{
    /* Nothing to cleanup - signals are automatically disconnected */
}
