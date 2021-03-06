/*
 * (C) 2001     Fatih Demir <kabalak@kabalak.net>
 *     2012     Ignacio Casal Quinteiro <icq@gnome.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Fatih Demir <kabalak@kabalak.net>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifndef GTR_TRANSLATION_MEMORY_UTILS_H
#define GTR_TRANSLATION_MEMORY_UTILS_H

#include <gtk/gtk.h>
#include <gio/gio.h>

void    gtr_scan_dir    (GFile         *dir,
                         GSList       **list,
                         const gchar   *po_name);

#endif
