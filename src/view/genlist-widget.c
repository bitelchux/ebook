/*
 * Copyright 2014 - 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "view/genlist-widget.h"
//#include "view/navi-path-widget.h"
#include "view/list-view.h"
#include "utils/ui-utils.h"
#include "utils/logger.h"
#include "utils/model-utils.h"
#include "utils/config.h"
#include "utils/common-utils.h"
#include "utils/app-types.h"
#include "main-app.h"
#include "view/txt.h"
#define MIN_FILE_TYPE_ICON_SIZE ELM_SCALE_SIZE(10)

//static const char *const LABEL_TEXT_SELECT_ALL = "Select All";

static void _genlist_widget_destroy(genlist_widget *widget);
static void _genlist_widget_set_items_style(genlist_widget *widget);
static void _genlist_widget_reset_all_checkboxes(genlist_widget *widget);
static void _genlist_widget_check_state_sel_all(genlist_widget *widget);
static void _genlist_widget_item_edit_sel(genlist_widget *widget, Evas_Object *checkbox);
static void _genlist_widget_sel_all_item_sel(genlist_widget *widget);

static void _genlist_widget_delete_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _genlist_widget_item_sel_cb(void *data, Evas_Object *obj, void *event_info);
static void _genlist_widget_sel_all_checkbox_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _genlist_widget_sel_all_layout_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _genlist_widget_item_checkbox_sel_cb(void *data, Evas_Object *obj, void *event_info);

static Evas_Object *_genlist_widget_edit_icon_get(void *data, Evas_Object *obj, const char *part);
static Evas_Object *_genlist_widget_main_icon_get(const char *icon_name, Evas_Object *parent);
static Evas_Object *_genlist_widget_default_node_icon_get(void *data, Evas_Object *obj, const char *part);
static Evas_Object *_genlist_widget_default_storage_icon_get(void *data, Evas_Object *obj, const char *part);
static char *_genlist_widget_node_label_get(void *data, Evas_Object *obj, const char *part);
static char *_genlist_widget_storage_label_get(void *data, Evas_Object *obj, const char *part);

genlist_widget *genlist_widget_add(view_data *view)
{
    RETVM_IF(!view, NULL, "View object is NULL");

    genlist_widget *widget = calloc(1, sizeof(genlist_widget));
    RETVM_IF(!widget, NULL, "Fail allocate memory");

    widget->view = view;

    widget->box = elm_box_add(widget->view->navi_layout);
    if (!widget->box) {
        ERR("Fail to create box");
        _genlist_widget_destroy(widget);
        return NULL;
    }
    evas_object_size_hint_weight_set(widget->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(widget->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    widget->genlist = ui_utils_genlist_add(widget->view->navi_layout, _genlist_widget_delete_cb, widget);
    if (!widget->genlist) {
        ERR("Fail to create genlist");
        evas_object_del(widget->box);
        _genlist_widget_destroy(widget);
        return NULL;
    }

	Evas_Object *circle_genlist = eext_circle_object_genlist_add(widget->genlist, widget->view->app->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

    _genlist_widget_set_items_style(widget);

    elm_box_pack_end(widget->box, widget->genlist);
    evas_object_show(widget->box);

    elm_object_part_content_set(widget->view->navi_layout, "content", widget->box);
    evas_object_data_set(widget->genlist, "data_list", widget);

    return widget;
}

void genlist_widget_content_set(genlist_widget *widget, const Eina_List *file_list)
{
    RETM_IF(!widget, "Widget is NULL");

    const Eina_List *list = NULL;
    void *item = NULL;

    _genlist_widget_set_items_style(widget);

    EINA_LIST_FOREACH(file_list, list, item)
    {
        Elm_Object_Item *node_item = elm_genlist_item_append(widget->genlist, &itc, item, NULL,
                ELM_GENLIST_ITEM_NONE, _genlist_widget_item_sel_cb, widget);

        elm_object_item_data_set(node_item, item);
    }
}

int genlist_widget_update(genlist_widget *widget)
{
    RETVM_IF(!widget->view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

    app_data *app = widget->view->app;

    _genlist_widget_set_items_style(widget);
    _genlist_widget_reset_all_checkboxes(widget);

    if (app->status.curr_mode == MODE_EDIT) {
        evas_object_show(widget->sel_all_layout);
        elm_box_pack_start(widget->box, widget->sel_all_layout);
    } else {
        evas_object_hide(widget->sel_all_layout);
        elm_box_unpack(widget->box, widget->sel_all_layout);
    }

    return RESULT_TYPE_OK;
}

void genlist_widget_clear(genlist_widget *widget)
{
    elm_genlist_clear(widget->genlist);
}

static void _genlist_widget_destroy(genlist_widget *widget)
{
    free(widget);
}

static void _genlist_widget_set_items_style(genlist_widget *widget)
{
    app_data *app = widget->view->app;
    itc.item_style = "multiline" ;//"type1";

    itc.func.text_get = (app->status.is_mainview)
                      ? _genlist_widget_storage_label_get
                      : _genlist_widget_node_label_get;
/*
    if (app->status.curr_mode == MODE_EDIT) {
        itc.func.content_get = _genlist_widget_edit_icon_get;
    } else {
        itc.func.content_get = (app->status.is_mainview)
                               ? _genlist_widget_default_storage_icon_get
                               : _genlist_widget_default_node_icon_get;
    }
*/
    elm_genlist_realized_items_update(widget->genlist);
}

static void _genlist_widget_reset_all_checkboxes(genlist_widget *widget)
{
    Elm_Object_Item *item = elm_genlist_first_item_get(widget->genlist);
    while (item) {
        node_info *node = elm_object_item_data_get(item);
        node->is_selected = EINA_FALSE;

        Evas_Object *checkbox = elm_object_item_part_content_get(item, "elm.swallow.end");
        elm_check_state_set(checkbox, EINA_FALSE);
        item = elm_genlist_item_next_get(item);
    }

    elm_check_state_set(widget->sel_all_checkbox, EINA_FALSE);
 //   widget->view->edit.checked_count = 0;
}

static void _genlist_widget_check_state_sel_all(genlist_widget *widget)
{
    Eina_Bool check_sel_all = EINA_FALSE;
 //   if (widget->view->edit.checked_count == list_view_items_count_get(widget->view)) {
  //      check_sel_all = EINA_TRUE;
   // }
    elm_check_state_set(widget->sel_all_checkbox, check_sel_all);
}

static void _genlist_widget_item_edit_sel(genlist_widget *widget, Evas_Object *checkbox)
{
    Eina_Bool checked = elm_check_state_get(checkbox);
 //   (checked) ? ++widget->view->edit.checked_count
  //            : --widget->view->edit.checked_count;
}

static void _genlist_widget_sel_all_item_sel(genlist_widget *widget)
{
    Eina_Bool checked = elm_check_state_get(widget->sel_all_checkbox);
    Elm_Object_Item *item = elm_genlist_first_item_get(widget->genlist);
    while (item) {
        node_info *node = elm_object_item_data_get(item);

        if (node->is_selected != checked) {
            node->is_selected = checked;

 //           (checked) ? ++widget->view->edit.checked_count
  //                    : --widget->view->edit.checked_count;

            Evas_Object *checkbox = elm_object_item_part_content_get(item, "elm.swallow.end");
            elm_check_state_set(checkbox, checked);
        }

        item = elm_genlist_item_next_get(item);
    }

   // int res = list_view_select_all(widget->view, checked);
    //RETM_IF(res != RESULT_TYPE_OK, "Failed to %s all files", (checked ? "select" : "unselect"));
}

static void _genlist_widget_delete_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    _genlist_widget_destroy(data);
}

static void _genlist_widget_item_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
    RETM_IF(!data, "Data is NULL");

    genlist_widget *widget = data;
    app_data *app = widget->view->app;

    elm_genlist_item_selected_set(event_info, EINA_FALSE);

    if (app->status.curr_mode != MODE_EDIT) {
        if (app->status.is_mainview) {
            storage_info *storage = elm_object_item_data_get(event_info);
            list_view_add(app, widget->view->navi, storage->root_path, NULL);
        } else {
            node_info *pNode = elm_object_item_data_get(event_info);
            if (pNode->type == FILE_TYPE_DIR) {
                list_view_add(app, widget->view->navi, widget->view->curr_path, pNode->name);
            } else if (pNode->type == FILE_TYPE_TXT) {
            	txt_view_add(app, widget->view->navi, widget->view->curr_path, pNode->name, pNode);
            }
            else {
                model_utils_launch_file(pNode);
            }
        }
    } else {
        Evas_Object *checkbox = elm_object_item_part_content_get(event_info, "elm.swallow.end");
        Eina_Bool checked = elm_check_state_get(checkbox);
        elm_check_state_set(checkbox, !checked);

        _genlist_widget_item_edit_sel(widget, checkbox);
        _genlist_widget_check_state_sel_all(widget);

        int res = list_view_update(widget->view->navi_item, UPDATE_TYPE_CTRL_BAR_STATE);
        RETM_IF(res != RESULT_TYPE_OK, "Fail to update view");
    }
}

static void _genlist_widget_sel_all_layout_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    RETM_IF(!data, "Data is NULL");

    genlist_widget *widget = data;
    Eina_Bool checked = elm_check_state_get(widget->sel_all_checkbox);
    elm_check_state_set(widget->sel_all_checkbox, !checked);
    _genlist_widget_sel_all_checkbox_change_cb(data, widget->sel_all_checkbox, NULL);
}

static void _genlist_widget_sel_all_checkbox_change_cb(void *data, Evas_Object *obj, void *event_info)
{
    RETM_IF(!data, "Data is NULL");

    genlist_widget *widget = data;
    _genlist_widget_sel_all_item_sel(widget);

  //  int res = list_view_update(widget->view->navi_item, UPDATE_TYPE_CTRL_BAR_STATE);
  //  RETM_IF(res != RESULT_TYPE_OK, "Fail to update view");
}

static void _genlist_widget_item_checkbox_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
    RETM_IF(!data, "Data is NULL");

    genlist_widget *widget = data;
    _genlist_widget_item_edit_sel(widget, obj);
    _genlist_widget_check_state_sel_all(widget);

   // int res = list_view_update(widget->view->navi_item, UPDATE_TYPE_CTRL_BAR_STATE);
   // RETM_IF(res != RESULT_TYPE_OK, "Fail to update view");
}

static Evas_Object *_genlist_widget_edit_icon_get(void *data, Evas_Object *obj, const char *part)
{
    RETVM_IF(!data, NULL, "Data is NULL");
    Evas_Object *eo = NULL;

    genlist_widget *widget = evas_object_data_get(obj, "data_list");
    node_info *pNode = data;

    if (!strcmp(part, "elm.swallow.icon")) {
        eo = _genlist_widget_default_node_icon_get(data, obj, part);
        RETVM_IF(!eo, NULL, "Failed to create icon");
    } else if (!strcmp(part, "elm.swallow.end")) {
        eo = elm_check_add(obj);
        elm_check_state_set(eo, pNode->is_selected);
        elm_check_state_pointer_set(eo, &pNode->is_selected);
        elm_object_focus_set(eo, EINA_FALSE);
        evas_object_propagate_events_set(eo, EINA_FALSE);
        evas_object_smart_callback_add(eo, "changed", _genlist_widget_item_checkbox_sel_cb, widget);
    }

    return eo;
}

static Evas_Object *_genlist_widget_main_icon_get(const char *icon_name, Evas_Object *parent)
{
    Evas_Object *icon = elm_image_add(parent);
    elm_object_focus_set(icon, EINA_FALSE);
    elm_image_file_set(icon, ui_utils_get_resource(icon_name), NULL);
    evas_object_size_hint_min_set(icon, MIN_FILE_TYPE_ICON_SIZE, MIN_FILE_TYPE_ICON_SIZE);
    evas_object_show(icon);
    return icon;
}

static Evas_Object *_genlist_widget_default_node_icon_get(void *data, Evas_Object *obj, const char *part)
{
    RETVM_IF(!data, NULL, "Data is NULL");
    node_info *pNode = data;
    Evas_Object *eo = NULL;
    const char *icon_name = model_utils_get_default_icon_name(pNode->type);

    if (!strcmp(part, "elm.swallow.icon")) {
        eo = _genlist_widget_main_icon_get(icon_name, obj);
        RETVM_IF(!eo, NULL, "Failed to create icon");
    }

    return eo;
}

static Evas_Object *_genlist_widget_default_storage_icon_get(void *data, Evas_Object *obj, const char *part)
{
    RETVM_IF(!data, NULL, "Data is NULL");
    Evas_Object *eo = NULL;

    if (!strcmp(part, "elm.swallow.icon")) {
        eo = _genlist_widget_main_icon_get(FM_ICON_FOLDER, obj);
        RETVM_IF(!eo, NULL, "Failed to create icon");
    }

    return eo;
}

static char *_genlist_widget_node_label_get(void *data, Evas_Object *obj, const char *part)
{
    RETVM_IF(!data, NULL, "Data is NULL");
    node_info *pNode = data;

    if (!strcmp(part, "elm.text")) {
        return elm_entry_utf8_to_markup(pNode->name);
    }

    return NULL;
}

static char *_genlist_widget_storage_label_get(void *data, Evas_Object *obj, const char *part)
{
    RETVM_IF(!data, NULL, "Data is NULL");
    storage_info *pStorage = data;

    if (!strcmp(part, "elm.text")) {
        return elm_entry_utf8_to_markup(pStorage->root_name);
    }

    return NULL;
}

