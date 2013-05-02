/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gdk/gdkx.h>
#include <stdio.h>
#include <cairo-dock.h>
#include "applet-cryptmain.h"
#include "applet-struct.h"

static gboolean _cryptdock_stash_check_encfs_validity (const char *encfsf)
{
    gboolean retval = FALSE;
    if (encfsf==NULL)
        return retval;
    char buf[1024];
    const char* const conf[] = {"%s/.encfs6.xml", "%s/.encfs5"};
    unsigned int confs = sizeof conf / sizeof conf[0];
    for (int i = 0; i < confs; i++ )  {
        snprintf(buf, sizeof (buf), conf[i], encfsf);
        if (g_file_test(buf, G_FILE_TEST_IS_REGULAR))  {
            retval=TRUE;
            break;
        }
    }
    return retval;
}

void crypt_dock_import_stash_wizard()
{
    gboolean disgardStash = FALSE;
    CryptItem *pStashItem;
    cd_message ("%s enter to the function", __func__);
    pStashItem = g_new0 (CryptItem, 1);
    for (int i=0; i < 3; i++) {
        switch (i)  {
            case 0:
                {
                GtkWidget* pFileChooserDialog = gtk_file_chooser_dialog_new (
                    _("Select an existing EncFS encrypted folder (eg ~/.crypt)"),
                    GTK_WINDOW (myContainer->pWidget),
                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                    GTK_STOCK_OK,
                    GTK_RESPONSE_OK,
                    GTK_STOCK_CANCEL,
                    GTK_RESPONSE_CANCEL,
                    NULL);

                gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (pFileChooserDialog), g_get_home_dir());
                gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (pFileChooserDialog), FALSE);

                gtk_widget_show (pFileChooserDialog);
                int answer = gtk_dialog_run (GTK_DIALOG (pFileChooserDialog));
                gboolean valid = FALSE;
                if (answer == GTK_RESPONSE_OK)	{
                    pStashItem->cCryptFolder = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileChooserDialog));
                    valid = _cryptdock_stash_check_encfs_validity(pStashItem->cCryptFolder);
                }
                else  {
                    disgardStash = TRUE;
                }

                gtk_widget_destroy (pFileChooserDialog);
                if (valid == FALSE )  {
                        cairo_dock_show_dialog_and_wait ( D_("The selected folder is not an EncFS encrypted folder"),
                            myIcon,
                            myContainer,
                            NULL,
                            NULL);
                        disgardStash = TRUE;
                    }
                }
                break;
            case 1:
                {
                char *blah=NULL;
                char *last_slash;
                // Choose the name and location at which you want the EncFS folder to be mounted
                GtkWidget* pFileChooserDialog = gtk_file_chooser_dialog_new (
                    _("Choose the name and location at which you want the EncFS folder to be mounted"),
                    GTK_WINDOW (myContainer->pWidget),
                    GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
                    GTK_STOCK_OK,
                    GTK_RESPONSE_OK,
                    GTK_STOCK_CANCEL,
                    GTK_RESPONSE_CANCEL,
                    NULL);

                blah = g_strdup(pStashItem->cCryptFolder);
                last_slash = strrchr (blah, '/');
                if (last_slash)
                    *last_slash = '\0';

                gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (pFileChooserDialog), blah);
                gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (pFileChooserDialog), FALSE);

                gtk_widget_show (pFileChooserDialog);
                int answer = gtk_dialog_run (GTK_DIALOG (pFileChooserDialog));
                if (answer == GTK_RESPONSE_OK)	{
                    pStashItem->cMountFolder = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileChooserDialog));
                } else {
                    disgardStash = TRUE;
                    cairo_dock_show_dialog_and_wait ( D_("You must enter a name"),
                        myIcon,
                        myContainer,
                        NULL,
                        NULL);
                }
                gtk_widget_destroy (pFileChooserDialog);

                }
                break;
            case 2:
                {
                GtkWidget *editbox = gtk_entry_new();
                int selection = cairo_dock_show_dialog_and_wait ( D_("Enter name of stash (visible in list)"),
                        myIcon,
                        myContainer,
                        NULL,
                        editbox);

                if (selection == STASH_NAME_BUTTON_OK)  {		// user select Ok check mark
                    pStashItem->cVisibleName = g_strdup(gtk_entry_get_text(GTK_ENTRY(editbox)));
                } 	else  {
                    disgardStash = TRUE;
                }
                gtk_widget_destroy (editbox);
                }
                break;

            }

            if (disgardStash==TRUE) {
                break;
            }
        }

    if (disgardStash == FALSE && crypt_save_stash_to_settings (pStashItem) == TRUE)  {
        g_ptr_array_add (myData.pStashArray, pStashItem);
    } else {
        crypt_show_notification_dialog (D_("Unable to perform operation"));
        crypt_free_stashitem (pStashItem);
    }

    return;
}
