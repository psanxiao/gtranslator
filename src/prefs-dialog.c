/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gui.h"
#include "prefs-dialog.h"
#include "utils.h"

#include <gtk/gtkclist.h>
#include <gtk/gtktable.h>

#include <gal/e-paned/e-hpaned.h>

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

#include <libgnomeui/gnome-app-util.h>
#include <libgnomeui/gnome-dialog.h>
#include <libgnomeui/gnome-stock.h>

/*
 * Local handlers for the preferences dialog.
 */
static void select_row_function(GtkCList	*list,
	gint row, gint column, GdkEventButton	*event,
	gpointer useless);

static void clicked_function(GnomeDialog *dialog, gint button,
	gpointer interesting);

/*
 * Handles the row selections on the leftern sections list.
 */
static void select_row_function(GtkCList	*list,
	gint row, gint column, GdkEventButton	*event,
	gpointer dialog)
{
	g_return_if_fail(list!=NULL);
	g_return_if_fail(row > 0);

	/*
	 * Only react onn a left-click on the clist and also check the row
	 *  value for sanity.
	 */
	if(row < g_list_length(GTR_PREFS_DIALOG(dialog)->sections) &&
		event->button==1)
	{
		GtkWidget	*table;

		/*
		 * Arg, this shouldn't be seen as a good way to do things -- it
		 *  isn't by any means.
		 */
		table=(GtkWidget *) g_list_nth_data(GTR_PREFS_DIALOG(dialog)->widgets, row);
		g_return_if_fail(table!=NULL);

		if(GTR_PREFS_DIALOG(dialog)->content)
		{
			gtk_widget_hide(GTR_PREFS_DIALOG(dialog)->content);
		}

		GTR_PREFS_DIALOG(dialog)->content=table;

		/*
		 * And as we're already so bad we do simply pack the new widget
		 *  table into the pane -- what happened to the old one? Uhm.
		 */
		e_paned_pack2(E_PANED(GTR_PREFS_DIALOG(dialog)->pane),
			GTR_PREFS_DIALOG(dialog)->content, TRUE, FALSE);

		gtk_widget_show_all(GTR_PREFS_DIALOG(dialog)->content);
	}
}

/*
 * Handles the button clicks of the preferences dialog.
 */
static void clicked_function(GnomeDialog *dialog, gint button,
	gpointer interesting)
{
	if(!button || button==1)
	{
		if(GTR_PREFS_DIALOG(interesting)->read_all_options_function)
		{
			GTR_PREFS_DIALOG(interesting)->read_all_options_function();
		}

		/*
		 * As we did read all options, pending changes should now be
		 *  applied -- therefore we have bo change pending. Right? ,-)
		 */
		GTR_PREFS_DIALOG(interesting)->changed=FALSE;
		
		/*
		 * If "Ok" was clicked on, then we'd to close the dialog, too.
		 */
		if(!button)
		{
			gtranslator_prefs_dialog_close(GTR_PREFS_DIALOG(interesting));
		}
	}
	else if(button==3)
	{
		gnome_app_message(GNOME_APP(gtranslator_application), _("\
		The Preferences box allows you to customise gtranslator\n\
		to work in ways you find comfortable and productive."));
	}
	else
	{
		gtranslator_prefs_dialog_close(GTR_PREFS_DIALOG(interesting));
	}
}

/*
 * Create the new preferences dialog in memory.
 */
GtrPrefsDialog *gtranslator_prefs_dialog_new(GVoidFunc read_all_options_func)
{
	GtrPrefsDialog 	*dialog;
	gchar		*titles[] = { _("Preferences section") };
	
	dialog=g_new0(GtrPrefsDialog, 1);

	/*
	 * Now the "simple" widget creation routines are following.
	 */
	dialog->dialog=gnome_dialog_new(_("gtranslator -- preferences"),
		GNOME_STOCK_BUTTON_OK,
		GNOME_STOCK_BUTTON_APPLY,
		GNOME_STOCK_BUTTON_CLOSE,
		GNOME_STOCK_BUTTON_HELP,
		NULL);

	dialog->pane=e_hpaned_new();
	dialog->sections_list=gtk_clist_new_with_titles(1, titles);
	
	dialog->sections=NULL;
	dialog->widgets=NULL;
	dialog->content=NULL;

	/*
	 * We do add the pane to the main dialog and pack the clist already to
	 *  the left side of the pane.
	 */
	gtk_container_add(GTK_CONTAINER(GNOME_DIALOG(dialog->dialog)->vbox), 
		dialog->pane);

	e_paned_pack1(E_PANED(dialog->pane), dialog->sections_list,
		TRUE, FALSE);

	/*
	 * Initialization of the "internal" state variables -- we do use these 
	 *  for all of our changes determination "checks".
	 */
	dialog->changed=FALSE;
	dialog->shown=FALSE;
	dialog->changes=0;

	dialog->read_all_options_function=read_all_options_func;

	/*
	 * Disable the "Ok" and "Apply" buttons by default.
	 */
	gnome_dialog_set_sensitive(GNOME_DIALOG(GTR_PREFS_DIALOG(dialog)->dialog), 0, FALSE);
	gnome_dialog_set_sensitive(GNOME_DIALOG(GTR_PREFS_DIALOG(dialog)->dialog), 1, FALSE);

	/*
	 * Hook in the generalized callback functions for the dialog.
	 */
	gtk_signal_connect(GTK_OBJECT(GTR_PREFS_DIALOG(dialog)->sections_list),
		"select-row", GTK_SIGNAL_FUNC(select_row_function), dialog);

	gtk_signal_connect(GTK_OBJECT(GTR_PREFS_DIALOG(dialog)->dialog),
		"clicked", GTK_SIGNAL_FUNC(clicked_function), dialog);

	gtk_signal_connect(GTK_OBJECT(GTR_PREFS_DIALOG(dialog)->dialog),
		"delete_event", GTK_SIGNAL_FUNC(gtranslator_prefs_dialog_close), dialog);
	
	return dialog;
}

/*
 * Appends the given page to the preferences dialog.
 */
void gtranslator_prefs_dialog_append_page(GtrPrefsDialog *dialog,
	gchar *section_name, GtkWidget *table)
{
	gchar	*section_name_array[] = { section_name };
	
	g_return_if_fail(dialog!=NULL);
	g_return_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL);
	g_return_if_fail(section_name!=NULL);
	g_return_if_fail(table!=NULL);

	gtk_clist_append(GTK_CLIST(GTR_PREFS_DIALOG(dialog)->sections_list),
		section_name_array);

	dialog->sections=g_list_append(dialog->sections, section_name);
	dialog->widgets=g_list_append(dialog->widgets, table);

	if(dialog->shown && g_list_length(dialog->sections) >= 1)
	{
		dialog->content=table;
		
		e_paned_pack2(E_PANED(GTR_PREFS_DIALOG(dialog)->pane),
			GTR_PREFS_DIALOG(dialog)->content, TRUE, FALSE);

		gtk_widget_show_all(GTR_PREFS_DIALOG(dialog)->content);
	}
}

/*
 * Show/hide the preferences dialog safely.
 */
void gtranslator_prefs_dialog_show(GtrPrefsDialog *dialog)
{
	g_return_if_fail(dialog!=NULL);
	g_return_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL);

	if(!GTR_PREFS_DIALOG(dialog)->shown)
	{
		gtk_widget_show_all(GTK_WIDGET(GTR_PREFS_DIALOG(dialog)->dialog));
		GTR_PREFS_DIALOG(dialog)->shown=TRUE;
	}
}

void gtranslator_prefs_dialog_hide(GtrPrefsDialog *dialog)
{
	g_return_if_fail(dialog!=NULL);
	g_return_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL);

	if(GTR_PREFS_DIALOG(dialog)->shown)
	{
		gtk_widget_hide(GTK_WIDGET(GTR_PREFS_DIALOG(dialog)->dialog));
		GTR_PREFS_DIALOG(dialog)->shown=FALSE;
	}
}

/*
 *  Set the changed status and update the button activities accordinly .-)
 */
void gtranslator_prefs_dialog_changed(GtrPrefsDialog *dialog)
{
	g_return_if_fail(dialog!=NULL);
	g_return_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL);

	GTR_PREFS_DIALOG(dialog)->changed=TRUE;
	GTR_PREFS_DIALOG(dialog)->changes++;

	gnome_dialog_set_sensitive(GNOME_DIALOG(GTR_PREFS_DIALOG(dialog)->dialog), 0, TRUE);
	gnome_dialog_set_sensitive(GNOME_DIALOG(GTR_PREFS_DIALOG(dialog)->dialog), 1, TRUE);
}

/*
 * Return whether any change has been performed -- return the corresponding
 *  variable to be more honest ,-)
 */
gboolean gtranslator_prefs_dialog_is_changed(GtrPrefsDialog *dialog)
{
	g_return_val_if_fail(dialog!=NULL, FALSE);
	g_return_val_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL, FALSE);

	return GTR_PREFS_DIALOG(dialog)->changed;
}

/*
 * Close/free the preferences dialog correspondingly.
 */
void gtranslator_prefs_dialog_close(GtrPrefsDialog *dialog)
{
	g_return_if_fail(dialog!=NULL);
	g_return_if_fail(GTR_PREFS_DIALOG(dialog)->dialog!=NULL);

	/*
	 * Check if any change is pending.
	 */
	if(GTR_PREFS_DIALOG(dialog)->changed)
	{
		gchar		*warning_str=NULL;
		GtkWidget	*warning_dialog=NULL;
		GtkWidget	*warning_label=NULL;
		gint		 retval=-1;
		
		/*
		 * Build up a quite informative question dialog with the count 
		 *  of changes which are still pending.
		 */
		warning_str=g_strdup_printf(_("%i preferences changes have been made.\n\
Do you want to discard all of these changes?"), 
			GTR_PREFS_DIALOG(dialog)->changes);
		
		warning_label=gtk_label_new(warning_str);
		warning_dialog=gnome_dialog_new(_("gtranslator -- preferences"),
			GNOME_STOCK_BUTTON_YES,
			GNOME_STOCK_BUTTON_NO,
			GNOME_STOCK_BUTTON_CANCEL,
			NULL);

		gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(warning_dialog)->vbox),
			warning_label, TRUE, TRUE, 0);

		retval=gnome_dialog_run_and_close(GNOME_DIALOG(warning_dialog));
		GTR_FREE(warning_str);
		
		if(retval!=0)
		{
			return;
		}
	}

	gnome_dialog_close(GNOME_DIALOG(dialog->dialog));

	gtranslator_utils_free_list(GTR_PREFS_DIALOG(dialog)->sections, FALSE);
	gtranslator_utils_free_list(GTR_PREFS_DIALOG(dialog)->widgets, FALSE);
	GTR_FREE(dialog);
}