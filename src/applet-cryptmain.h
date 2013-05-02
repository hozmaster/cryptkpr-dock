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


#ifndef __APPLET_CRYPTMAIN__
#define __APPLET_CRYPTMAIN__


#include <cairo-dock.h>
#include "applet-struct.h"

// Free reserved memory in from stash item
void crypt_free_stashitem (CryptItem* item);
// Add new stash item to menu
void cd_cryptdock_add_new_stash_item (const gchar *pStashName);

void crypt_show_notification_dialog (const char *);
// Remove existing stash from the settings file
gboolean crypt_delete_stash_from_list (CryptItem *newStash);
// Save stash settings to the settings file.
gboolean crypt_save_stash_to_settings (CryptItem *newStash);

// Update count of mount stash.
void        crypt_update_stash_count(void);

GtkWidget *cd_cryptdock_build_action_menu (void);

#endif
