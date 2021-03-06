/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-alternate-language-plugin.h"
#include "gtr-alternate-language-panel.h"
#include "gtr-notebook.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"
#include "gtr-tab.h"
#include "gtr-tab-activatable.h"

#include <glib/gi18n.h>

#define TAB_DATA_KEY    "GtrAlternateLangPluginTabData"
#define MENU_PATH "/MainMenu/ViewMenu/ViewOps_2"

struct _GtrAlternateLangPluginPrivate
{
  GtrWindow *window;
  GtrTab *tab;

  GtkActionGroup *action_group;
  guint ui_id;
};

enum
{
  PROP_0,
  PROP_WINDOW,
  PROP_TAB
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);
static void gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrAlternateLangPlugin,
                                gtr_alternate_lang_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init) \
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_TAB_ACTIVATABLE,
                                                               gtr_tab_activatable_iface_init)    \
                                                                                                  \
                                _gtr_alternate_lang_panel_register_type (type_module);            \
)

static void
on_alternate_lang_activated (GtkAction *action,
                             GtrWindow * window)
{
  GtrTab *tab;
  GtkWidget *alternatelang;

  tab = gtr_window_get_active_tab (window);
  alternatelang = g_object_get_data (G_OBJECT (tab), TAB_DATA_KEY);

  gtr_tab_show_widget (GTR_TAB (tab), alternatelang);
}

static const GtkActionEntry action_entries[] = {
  {"AlternateLang", NULL, N_("_Alternate Language"), "<control>K",
   N_("Show the Alternate Language panel"),
   G_CALLBACK (on_alternate_lang_activated)},
};

static void
gtr_alternate_lang_plugin_init (GtrAlternateLangPlugin *plugin)
{
  plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin,
                                              GTR_TYPE_ALTERNATE_LANG_PLUGIN,
                                              GtrAlternateLangPluginPrivate);
}

static void
gtr_alternate_lang_plugin_dispose (GObject *object)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

  g_clear_object (&priv->window);
  g_clear_object (&priv->tab);
  g_clear_object (&priv->action_group);

  G_OBJECT_CLASS (gtr_alternate_lang_plugin_parent_class)->dispose (object);
}

static void
gtr_alternate_lang_plugin_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        priv->window = GTR_WINDOW (g_value_dup_object (value));
        break;

      case PROP_TAB:
        priv->tab = GTR_TAB (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_alternate_lang_plugin_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        g_value_set_object (value, priv->window);
        break;

      case PROP_TAB:
        g_value_set_object (value, priv->tab);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
update_ui (GtrAlternateLangPlugin *plugin)
{
  GtrTab *tab;
  GtkAction *action;

  tab = gtr_window_get_active_tab (plugin->priv->window);

  action = gtk_action_group_get_action (plugin->priv->action_group, "AlternateLang");
  gtk_action_set_sensitive (action, (tab != NULL));
}

static void
gtr_alternate_lang_plugin_window_activate (GtrWindowActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtkUIManager *manager;

  manager = gtr_window_get_ui_manager (priv->window);

  priv->action_group = gtk_action_group_new ("GtrAlternateLangPluginActions");
  gtk_action_group_set_translation_domain (priv->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_actions (priv->action_group, action_entries,
                                G_N_ELEMENTS (action_entries), priv->window);

  gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);

  priv->ui_id = gtk_ui_manager_new_merge_id (manager);

  gtk_ui_manager_add_ui (manager,
                         priv->ui_id,
                         MENU_PATH,
                         "AlternateLang",
                         "AlternateLang", GTK_UI_MANAGER_MENUITEM, FALSE);
}

static void
gtr_alternate_lang_plugin_window_deactivate (GtrWindowActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtkUIManager *manager;

  /* Remove menuitem */
  manager = gtr_window_get_ui_manager (priv->window);

  gtk_ui_manager_remove_ui (manager, priv->ui_id);
  gtk_ui_manager_remove_action_group (manager, priv->action_group);
}

static void
gtr_alternate_lang_plugin_window_update_state (GtrWindowActivatable *activatable)
{
  update_ui (GTR_ALTERNATE_LANG_PLUGIN (activatable));
}

static void
gtr_alternate_lang_plugin_tab_activate (GtrTabActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtkWidget *alternatelang;

  alternatelang = gtr_alternate_lang_panel_new (priv->tab);
  gtk_widget_show (alternatelang);

  gtr_tab_add_widget (GTR_TAB (priv->tab),
                      alternatelang,
                      "GtrAlternateLangPluginPanel",
                      _("Alternate Language"),
                      NULL,
                      GTR_TAB_PLACEMENT_RIGHT);

  g_object_set_data (G_OBJECT (priv->tab), TAB_DATA_KEY, alternatelang);
}

static void
gtr_alternate_lang_plugin_tab_deactivate (GtrTabActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtkWidget *alternatelang;

  alternatelang = g_object_get_data (G_OBJECT (priv->tab), TAB_DATA_KEY);
  g_return_if_fail (alternatelang != NULL);

  gtr_tab_remove_widget (priv->tab, alternatelang);
  g_object_set_data (G_OBJECT (priv->tab), TAB_DATA_KEY, NULL);
}

static void
gtr_alternate_lang_plugin_class_init (GtrAlternateLangPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_alternate_lang_plugin_dispose;
  object_class->set_property = gtr_alternate_lang_plugin_set_property;
  object_class->get_property = gtr_alternate_lang_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");
  g_object_class_override_property (object_class, PROP_TAB, "tab");

  g_type_class_add_private (klass, sizeof (GtrAlternateLangPluginPrivate));
}

static void
gtr_alternate_lang_plugin_class_finalize (GtrAlternateLangPluginClass *klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_alternate_lang_plugin_window_activate;
  iface->deactivate = gtr_alternate_lang_plugin_window_deactivate;
  iface->update_state = gtr_alternate_lang_plugin_window_update_state;
}

static void
gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface)
{
  iface->activate = gtr_alternate_lang_plugin_tab_activate;
  iface->deactivate = gtr_alternate_lang_plugin_tab_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_alternate_lang_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_ALTERNATE_LANG_PLUGIN);
  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_TAB_ACTIVATABLE,
                                              GTR_TYPE_ALTERNATE_LANG_PLUGIN);
}
