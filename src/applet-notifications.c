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
#include "applet-notifications.h"
#include "applet-cryptmain.h"
#include "applet-create-new-stash.h"
#include "applet-import-stash-wizard.h"


//\___________ Define here the action to be taken when the user left-clicks on your icon or on its subdock or your desklet. The icon and the container that were clicked are available through the macros CD_APPLET_CLICKED_ICON and CD_APPLET_CLICKED_CONTAINER. CD_APPLET_CLICKED_ICON may be NULL if the user clicked in the container but out of icons.
CD_APPLET_ON_CLICK_BEGIN

	GtkWidget *pMenu = cd_cryptdock_build_action_menu();
	CD_APPLET_POPUP_MENU_ON_MY_ICON (pMenu);
CD_APPLET_ON_CLICK_END


//\___________ Same as ON_CLICK, but with middle-click.
CD_APPLET_ON_MIDDLE_CLICK_BEGIN
	
	
CD_APPLET_ON_MIDDLE_CLICK_END


//\___________ Same as ON_CLICK, but with scroll. Moreover, CD_APPLET_SCROLL_UP tels you is the user scrolled up, CD_APPLET_SCROLL_DOWN the opposite.
CD_APPLET_ON_SCROLL_BEGIN
	
	
CD_APPLET_ON_SCROLL_END

static void _cd_crypt_create_new_stash (GtkMenuItem *menu_item, gpointer data)
{
    CD_APPLET_ENTER;
    crypt_dock_create_new_stash ();
    CD_APPLET_LEAVE();
}

static void _cd_crypt_import_stash (GtkMenuItem *menu_item, gpointer data)
{
    CD_APPLET_ENTER;
    crypt_dock_import_stash_wizard ();
    CD_APPLET_LEAVE();
}

static void _cd_crypt_delete_stash_item (GtkMenuItem *menu_item, gpointer data)
{
    CD_APPLET_ENTER;
    int selection = cairo_dock_show_dialog_and_wait ( D_("Are you sure that you want delete selected stash?"),
            myIcon,
            myContainer,
            NULL,
            NULL);

    if (selection == STASH_NAME_BUTTON_OK)  {		// user select Ok check mark
        gboolean passed = crypt_delete_stash_from_list(data);
        if (passed==FALSE)  {
            crypt_show_notification_dialog (D_("Operation failed."));
        } else {
            g_ptr_array_remove (myData.pStashArray, data);
            crypt_free_stashitem (data);
            crypt_show_notification_dialog (D_("Stash removed. Note actual encfs folder still remain at the disk."));
        }
    }
    CD_APPLET_LEAVE();
}


//\___________ Define here the entries you want to add to the menu when the user right-clicks on your icon or on its subdock or your desklet. The icon and the container that were clicked are available through the macros CD_APPLET_CLICKED_ICON and CD_APPLET_CLICKED_CONTAINER. CD_APPLET_CLICKED_ICON may be NULL if the user clicked in the container but out of icons. The menu where you can add your entries is available throught the macro CD_APPLET_MY_MENU; you can add sub-menu to it if you want.
CD_APPLET_ON_BUILD_MENU_BEGIN

    GtkWidget *pSubMenu = CD_APPLET_ADD_SUB_MENU_WITH_IMAGE (D_("Delete stash ..."), CD_APPLET_MY_MENU, GTK_STOCK_REFRESH);
    for (int i=0; i < myData.pStashArray->len; i++) {
        CryptItem *pStashItem = NULL;
        pStashItem = g_ptr_array_index (myData.pStashArray, i);
        if (pStashItem) {
            if (pStashItem->bIsMounted == TRUE)   {
                CD_APPLET_ADD_IN_MENU_WITH_STOCK_AND_DATA (pStashItem->cVisibleName, GTK_STOCK_YES, _cd_crypt_delete_stash_item, pSubMenu, pStashItem);
            } else {
                CD_APPLET_ADD_IN_MENU_WITH_STOCK_AND_DATA (pStashItem->cVisibleName, GTK_STOCK_NO, _cd_crypt_delete_stash_item, pSubMenu, pStashItem);
            }
        }
    }

CD_APPLET_ADD_IN_MENU_WITH_STOCK (D_("Import stash"), GTK_STOCK_CLEAR, _cd_crypt_import_stash, CD_APPLET_MY_MENU);
CD_APPLET_ADD_IN_MENU_WITH_STOCK (D_("Create new stash"), GTK_STOCK_PASTE, _cd_crypt_create_new_stash, CD_APPLET_MY_MENU);

CD_APPLET_ON_BUILD_MENU_END
