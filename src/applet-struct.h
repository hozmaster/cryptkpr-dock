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

#ifndef  __CD_APPLET_STRUCT__
#define  __CD_APPLET_STRUCT__

#include <cairo-dock.h>

//\___________ structure containing the applet's configuration parameters.
struct _AppletConfig {
	gboolean bEnableTimeout;
	gint iTimeoutValue;
	gboolean bAllowOther;
	};

//\___________ structure containing the applet's data, like surfaces, dialogs, results of calculus, etc.
struct _AppletData {
	CairoDialog *pDialog;
	CairoDialog *pPasswordDlg;
	GtkWidget 	*pWidget;
	gchar 		*cSettingsPath;
	GPtrArray	*pStashArray;
	gint 	iMountStashCount;
	};

typedef enum _CDCryptItemTypes
{
	CD_CRYPT_ITEM_VOID = 0,
	CD_CRYPT_ITEM_STASH
} CDCryptItemTypes;

typedef struct _CryptItem  {
	gchar *cVisibleName;
	gchar *cCryptFolder;
	gchar *cMountFolder;
	gboolean bIsMounted;
    gint    iMountStash;
	gboolean bIsAvailable;
	CDCryptItemTypes 	iItemType;
} CryptItem;


#define 	STASH_NAME_BUTTON_OK 		0
#define		STASH_NAME_BUTTON_CANCEL	1

#define		CRYPTKPR_SETTINGS_FILENAME			"settings.conf"			
#define		CRYPTKPR_SETTINGS_CRYPT_FOLDER		"CryptFolder"
#define		CRYPTKPR_SETTINGS_MOUNT_FOLDER		"MountFolder"

#endif
