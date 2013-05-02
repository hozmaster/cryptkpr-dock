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

#include <stdlib.h>
#include <string.h>

#include "applet-struct.h"
#include "applet-import-stash-wizard.h"
#include "encfs_wrapper.h"
#include "applet-cryptmain.h"

static void _cd_cryptdock_activate_item (GtkMenuItem *pMenuItem, CryptItem *pStashItem )
{
    CD_APPLET_ENTER;
    if ( pStashItem && pStashItem->bIsMounted == TRUE)  {
        if (pStashItem->cMountFolder != 0 && strlen(pStashItem->cMountFolder) !=0)  {
            int result = encfs_stash_unmount (pStashItem->cMountFolder);
            if (result)  {
                crypt_show_notification_dialog(D_("Unmount failed"));
                cd_message ("%s, unmount failed with %d ", __func__, result);
            } else {
                crypt_show_notification_dialog(D_("Stash unmount succesfully !"));
            }
        }
        pStashItem->bIsMounted = FALSE;		// temp should replaced
        myData.iMountStashCount --;
        crypt_update_stash_count ();
        }
    else if ( pStashItem ) {
        if ( (pStashItem->cCryptFolder != 0 && strlen(pStashItem->cCryptFolder)!=0) &&
            (pStashItem->cMountFolder != 0 && strlen(pStashItem->cMountFolder) !=0) )  {
            GtkWidget *editbox = gtk_entry_new();
            gtk_entry_set_visibility (GTK_ENTRY(editbox), FALSE);
            int selection = cairo_dock_show_dialog_and_wait ( D_("Enter password: "),
                    myIcon,
                    myContainer,
                    NULL,
                    editbox);

            if (selection == 0) {

                char *password = g_strdup(gtk_entry_get_text(GTK_ENTRY(editbox)));
                char *output = NULL;

                int timeout = 0;
                if (myConfig.bEnableTimeout == TRUE)  {
                    timeout = myConfig.iTimeoutValue;
                }
                int result = encfs_stash_mount (pStashItem->cCryptFolder, pStashItem->cMountFolder, password, timeout, FALSE, &output);
                if  (result != 0)  {
                    crypt_show_notification_dialog (D_("Please make sure that required folders exist."));
                    if (output && strlen(output))   {
                        cd_message ("%s mount failed, output len %d ", __func__, strlen(output));
                    }
                } else  {
                    pStashItem->bIsMounted = TRUE;
                    myData.iMountStashCount ++;
                    crypt_update_stash_count ();
                    crypt_show_notification_dialog (D_("Stash mount succesfully"));
                }


                g_free (password);
                g_free (output);
            } else {
                crypt_show_notification_dialog (D_("Cancelled."));
            }

            gtk_widget_destroy (editbox);
        } else  {
            cd_message ("%s required parameters missing", __func__);
        }
    }
    CD_APPLET_LEAVE();
}

// Free, let them free.
void crypt_free_stashitem(CryptItem* item)
{
    g_free (item->cVisibleName);
    g_free (item->cMountFolder);
    g_free (item->cCryptFolder);
    g_free (item);
}

void crypt_show_notification_dialog (const char *cMessage)
{
    CairoDialogAttribute attr;
    memset (&attr, 0, sizeof (CairoDialogAttribute));
    attr.cText = cMessage;
    attr.bUseMarkup = TRUE;
    const gchar *cButtonsImage[3] = {"ok", NULL, NULL};
    attr.cButtonsImage = cButtonsImage;
    attr.pUserData = myApplet;
    attr.pFreeDataFunc = NULL;
    attr.iTimeLength = 4500;
    cairo_dock_build_dialog (&attr, myIcon, myContainer);
}

void crypt_update_stash_count ()
{
    if (myData.iMountStashCount > 0)  {
        CD_APPLET_SET_QUICK_INFO_ON_MY_ICON_PRINTF ("%d", myData.iMountStashCount);
    } else  {
        CD_APPLET_SET_QUICK_INFO_ON_MY_ICON (NULL);
    }
}

gboolean crypt_delete_stash_from_list (CryptItem *stash)
{

    GError *error = NULL;
    if (stash==NULL)  {
        return FALSE;
    }

    if (stash->bIsMounted == TRUE /*|| crypt_is_mounted(stash->cMountFolder)==TRUE*/) {
        return FALSE;
    }

    GKeyFile* pKeyFile = cairo_dock_open_key_file (myData.cSettingsPath);
    g_return_val_if_fail (pKeyFile != NULL, FALSE);

    if (g_key_file_has_group (pKeyFile, stash->cVisibleName))
    {
        g_key_file_remove_group(pKeyFile, stash->cVisibleName, &error);
    }
    cairo_dock_write_keys_to_file (pKeyFile,myData.cSettingsPath);
    g_key_file_free (pKeyFile);
    return TRUE;
}

gboolean crypt_save_stash_to_settings (CryptItem *newStash)
{
    if (newStash == NULL) {
        return FALSE;
    }
    GKeyFile* pKeyFile = cairo_dock_open_key_file (myData.cSettingsPath);
    g_return_val_if_fail (pKeyFile != NULL, FALSE);

    if (g_key_file_has_group (pKeyFile, newStash->cVisibleName))
    {
        cairo_dock_show_temporary_dialog_with_icon (D_("The given stash exist already."), myIcon, myContainer, 5000, "same icon");
        g_key_file_free (pKeyFile);
        return FALSE;
    } else  {
        g_key_file_set_string (pKeyFile, newStash->cVisibleName, CRYPTKPR_SETTINGS_CRYPT_FOLDER, newStash->cCryptFolder);
        g_key_file_set_string (pKeyFile, newStash->cVisibleName, CRYPTKPR_SETTINGS_MOUNT_FOLDER, newStash->cMountFolder);
    }

    cairo_dock_write_keys_to_file (pKeyFile,myData.cSettingsPath);
    g_key_file_free (pKeyFile);
    return TRUE;
}

gboolean crypt_on_button_press (GtkWidget* widget,
  GdkEventButton * event, GdkWindowEdge edge)
{
  cd_message ("%s ", __func__);
  if (event->type == GDK_BUTTON_PRESS) {
    if (event->button == 1) {
    }
  }
  return FALSE;
}

GtkWidget *cd_cryptdock_build_action_menu ()
{
    GtkWidget *pMenu = gtk_menu_new ();
    GtkWidget *pMenuItem;
    cd_message ("%s ", __func__);
    g_signal_connect(G_OBJECT(pMenu), "button-press-event",
        G_CALLBACK(crypt_on_button_press), NULL);

    CD_APPLET_ADD_IN_MENU (D_ ("Stashes:"), _cd_cryptdock_activate_item, pMenu);
    CD_APPLET_ADD_SEPARATOR_IN_MENU (pMenu);

    for (int i=0; i < myData.pStashArray->len; i++) {
        CryptItem *pStashItem = NULL;
        pStashItem = g_ptr_array_index (myData.pStashArray, i);
        if (pStashItem) {
			// todo check is mount active or not.
            if (pStashItem->bIsMounted == TRUE )   {
                CD_APPLET_ADD_IN_MENU_WITH_STOCK_AND_DATA (pStashItem->cVisibleName, GTK_STOCK_YES, _cd_cryptdock_activate_item, pMenu, pStashItem);
             } else {
                CD_APPLET_ADD_IN_MENU_WITH_STOCK_AND_DATA (pStashItem->cVisibleName, GTK_STOCK_NO, _cd_cryptdock_activate_item, pMenu, pStashItem);
             }
        }
    }
    return pMenu;
}
