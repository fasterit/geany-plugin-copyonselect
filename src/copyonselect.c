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
                "1.01",
                "Faster IT GmbH")

/* Global variable to hold clipboard text */
static gchar *clipboard_text = NULL;

/* Clipboard targets we support */
static GtkTargetEntry targets[] = {
    { "UTF8_STRING", 0, 0 },
    { "STRING", 0, 0 },
    { "TEXT", 0, 0 },
    { "COMPOUND_TEXT", 0, 0 },
    { "text/plain;charset=utf-8", 0, 0 },
    { "text/plain", 0, 0 }
};

/* Callback to provide clipboard data when requested */
static void clipboard_get_func(GtkClipboard *clipboard, GtkSelectionData *selection_data,
                                guint info, gpointer user_data)
{
    (void)clipboard;
    (void)info;
    (void)user_data;

    if (clipboard_text != NULL)
    {
        gtk_selection_data_set_text(selection_data, clipboard_text, -1);
    }
}

/* Callback when we lose clipboard ownership - but we don't free the data */
static void clipboard_clear_func(GtkClipboard *clipboard, gpointer user_data)
{
    (void)clipboard;
    (void)user_data;

    /* Do NOT free clipboard_text here - keep it available for subsequent pastes.
     * The clipboard_text is only freed in two safe scenarios:
     * 1. When a new selection is made (in on_editor_notify)
     * 2. When the plugin is unloaded (in plugin_cleanup)
     * This ensures the text persists even after we lose clipboard ownership. */
}

/* Helper function to reclaim PRIMARY clipboard ownership */
static void reclaim_clipboard_ownership(void)
{
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);

    /* Re-establish ownership with our callbacks */
    gtk_clipboard_set_with_data(clipboard, targets,
                               G_N_ELEMENTS(targets),
                               clipboard_get_func,
                               clipboard_clear_func,
                               NULL);
}

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

            if (text != NULL && *text != '\0')
            {
                /* Free the old clipboard text if any */
                if (clipboard_text != NULL)
                {
                    g_free(clipboard_text);
                }

                /* Store the new text - we keep ownership of it */
                clipboard_text = text;

                /* Claim PRIMARY clipboard ownership */
                reclaim_clipboard_ownership();
            }
        }
        else  /* start_pos == end_pos: selection was cleared */
        {
            /* Selection was cleared (e.g., by left-clicking) - but only reclaim
             * PRIMARY clipboard ownership if another application hasn't claimed it.
             * This prevents overwriting selections made in other applications. */
            if (clipboard_text != NULL)
            {
                clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);

                /* Check if the PRIMARY clipboard still contains our text.
                 * If it's different, another application has claimed it and we
                 * should NOT overwrite it with our stored text. */
                gchar *current_text = gtk_clipboard_wait_for_text(clipboard);

                if (current_text != NULL)
                {
                    /* Compare the clipboard text with our stored text */
                    gboolean is_our_text = (g_strcmp0(current_text, clipboard_text) == 0);
                    g_free(current_text);

                    /* Only reclaim if the clipboard still has our text */
                    if (is_our_text)
                    {
                        reclaim_clipboard_ownership();
                    }
                }
                else
                {
                    /* Clipboard is empty, safe to reclaim */
                    reclaim_clipboard_ownership();
                }

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
    /* Free clipboard text on cleanup */
    if (clipboard_text != NULL)
    {
        g_free(clipboard_text);
        clipboard_text = NULL;
    }
}
