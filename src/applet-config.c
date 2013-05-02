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

#include <string.h>
#include <cairo-dock.h>
#include <glib/gstdio.h>

#include "applet-config.h"
#include "applet-cryptmain.h"

//\_________________ Here you have to get all your parameters from the conf file. Use the macros CD_CONFIG_GET_BOOLEAN, CD_CONFIG_GET_INTEGER, CD_CONFIG_GET_STRING, etc. myConfig has been reseted to 0 at this point. This function is called at the beginning of init and reload.
CD_APPLET_GET_CONFIG_BEGIN

    CryptItem *pStashItem;
    myConfig.bEnableTimeout = CD_CONFIG_GET_BOOLEAN ("Configuration", "enableTimeout");
    myConfig.iTimeoutValue = CD_CONFIG_GET_INTEGER ("Configuration", "timePeriodic");

	gboolean bfoundStash = FALSE;
	
    myData.iMountStashCount	= 0;
	myData.pStashArray = g_ptr_array_new ();
	
	myData.cSettingsPath = g_strdup_printf ("%s/cryptkpr-dock/settings.conf", g_cCairoDockDataDir);
	GKeyFile *pFile = cairo_dock_open_key_file (myData.cSettingsPath);		
	if (pFile != NULL) 	{	
		gsize length = 0;
		gchar **pGroupList = g_key_file_get_groups (pFile, &length);

		gchar *cStashName;			
		for (int i =0; i < length; i ++)  {
			cStashName = pGroupList[i];	
		
			gchar *cCryptedFolder = g_key_file_get_string (pFile, cStashName, CRYPTKPR_SETTINGS_CRYPT_FOLDER, NULL);
			if (cCryptedFolder == NULL)  {
				continue;
			}

			gchar *cMountFolder = g_key_file_get_string (pFile, cStashName, CRYPTKPR_SETTINGS_MOUNT_FOLDER, NULL);

			if (cMountFolder == NULL)  {
				g_free (cCryptedFolder);
				continue;
			}

			pStashItem = g_new0 (CryptItem, 1);
			g_ptr_array_add (myData.pStashArray, pStashItem);
			pStashItem->iItemType = CD_CRYPT_ITEM_STASH;
			pStashItem->cVisibleName = g_strdup(cStashName);
            pStashItem->cCryptFolder = g_strdup(cCryptedFolder);
            pStashItem->cMountFolder = g_strdup(cMountFolder);
            pStashItem->bIsMounted = FALSE;

			g_free (cCryptedFolder);
			g_free (cMountFolder);

			bfoundStash = TRUE;
			
		}		
		g_strfreev (pGroupList);			
	}  
	if (bfoundStash == FALSE ){
		pStashItem = g_new0 (CryptItem, 1);
		g_ptr_array_add (myData.pStashArray, pStashItem);
		pStashItem->iItemType = CD_CRYPT_ITEM_VOID;
		pStashItem->cVisibleName = g_strdup ("No stash listed");
	}	
	
CD_APPLET_GET_CONFIG_END


//\_________________ Here you have to free all resources allocated for myConfig. This one will be reseted to 0 at the end of this function. This function is called right before you get the applet's config, and when your applet is stopped, in the end.
CD_APPLET_RESET_CONFIG_BEGIN


CD_APPLET_RESET_CONFIG_END


//\_________________ Here you have to free all resources allocated for myData. This one will be reseted to 0 at the end of this function. This function is called when your applet is stopped, in the very end.
CD_APPLET_RESET_DATA_BEGIN

	if (myData.pDialog) {
		cairo_dock_dialog_unreference (myData.pDialog);  // 
	}
	
	if (myData.cSettingsPath)  {
		g_free (myData.cSettingsPath);
	}

	if (myData.pPasswordDlg) {
		cairo_dock_dialog_unreference(myData.pPasswordDlg);
	}
	
	CryptItem *pStashItem;
	guint i;
	if (myData.pStashArray != NULL) 	{
		for (i = 0; i < myData.pStashArray->len; i ++) {
			pStashItem = g_ptr_array_index (myData.pStashArray, i);
            crypt_free_stashitem (pStashItem);
			cd_message ("%s free: (%d)", __func__, i);
		}
		g_ptr_array_free (myData.pStashArray, TRUE);	
	}
	
CD_APPLET_RESET_DATA_END
