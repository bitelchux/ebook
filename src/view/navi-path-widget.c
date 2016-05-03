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

#include "view/navi-path-widget.h"
#include "view/list-view.h"
#include "view/navigator.h"
#include "utils/ui-utils.h"
#include "utils/app-types.h"
#include "utils/common-utils.h"
#include "utils/config.h"
#include "utils/logger.h"

#define FM_DEF_TEXT_COLOR  43, 58, 175, 255
#define FM_NAVIFRAME_ITEM_INDEX_MAIN 1
#define FM_DEF_COLUMN_NUMBER 0
#define FM_DEF_COLUMN_SPAN 1
#define FM_DEF_ROW_SPAN 1

#define FM_CLICK_MAX_DURATION_MS 150
#define FM_CLICK_MIN_X_DISTANCE_PIX 50

static void _navi_path_widget_destroy(navi_path_widget *widget);
static void _navi_path_widget_append_label(navi_path_widget *widget, const char *label_name);

static void _navi_path_widget_label_clicked(navi_path_widget *widget, Evas_Object *label);
static void _navi_path_widget_label_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _navi_path_widget_label_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _navi_path_widget_delete_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);

navi_path_widget *navi_path_widget_add(view_data *view)
{
	RETVM_IF(!view, NULL, "View object is NULL");

	navi_path_widget *widget = calloc(1, sizeof(navi_path_widget));
	RETVM_IF(!widget, NULL, "Fail allocate memory");

	widget->table_size = 0;
	widget->view = view;

	widget->navi_path_layout = ui_utils_genlist_add(widget->view->navi_layout, _navi_path_widget_delete_cb, widget);
	if (!widget->navi_path_layout) {
		ERR("Layout is NULL");
		_navi_path_widget_destroy(widget);
		return NULL;
	}

	evas_object_size_hint_weight_set(widget->navi_path_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(widget->navi_path_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_layout_file_set(widget->navi_path_layout, ui_utils_get_resource(FM_LAYOUT_EDJ), "path_info");

	evas_object_show(widget->navi_path_layout);

	widget->navi_path_scroller = elm_scroller_add(widget->navi_path_layout);
	if (!widget->navi_path_scroller) {
		ERR("Scroller is NULL");
		evas_object_del(widget->navi_path_layout);
		return NULL;
	}

	elm_scroller_bounce_set(widget->navi_path_scroller, EINA_TRUE, EINA_FALSE);
	elm_scroller_policy_set(widget->navi_path_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

	evas_object_size_hint_weight_set(widget->navi_path_scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(widget->navi_path_scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(widget->navi_path_scroller);

	widget->navi_path_table = elm_table_add(widget->navi_path_layout);
	if (!widget->navi_path_table) {
		ERR("Table is NULL");
		evas_object_del(widget->navi_path_layout);
		return NULL;
	}

	evas_object_size_hint_weight_set(widget->navi_path_table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(widget->navi_path_table, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(widget->navi_path_table);

	elm_object_content_set(widget->navi_path_scroller, widget->navi_path_table);
	elm_object_part_content_set(widget->navi_path_layout, "info", widget->navi_path_scroller);

	elm_object_part_content_set(widget->view->navi_layout, "header_box", widget->navi_path_layout);

	return widget;
}

void navi_path_widget_content_set(navi_path_widget *widget, Eina_List *path_list)
	{
	RETM_IF(!widget, "Widget object is NULL");

	Eina_List *list = NULL;
	char *data = NULL;
	EINA_LIST_FOREACH(path_list, list, data) {
		_navi_path_widget_append_label(widget, data);
	}
}

static void _navi_path_widget_append_label(navi_path_widget *widget, const char *label_name)
{
	RETM_IF(!widget, "Widget object is NULL");
	RETM_IF(!widget->view, "View object is NULL");
	RETM_IF(!label_name, "Label name is NULL");

	Evas_Object *label = elm_label_add(widget->navi_path_layout);
	RETM_IF(!label, "Label object is NULL");

	char *label_markup = elm_entry_utf8_to_markup(label_name);
	char *lable_text = common_util_strconcat(label_markup, "/", NULL);

	elm_object_text_set(label, lable_text);

	free(lable_text);
	free(label_markup);

	evas_object_color_set(label, FM_DEF_TEXT_COLOR);
	evas_object_event_callback_add(label, EVAS_CALLBACK_MOUSE_DOWN, _navi_path_widget_label_down_cb, widget);
	evas_object_event_callback_add(label, EVAS_CALLBACK_MOUSE_UP, _navi_path_widget_label_up_cb, widget);
	evas_object_show(label);

	elm_table_pack(widget->navi_path_table,
			label,
			++widget->table_size,
			FM_DEF_COLUMN_NUMBER,
			FM_DEF_COLUMN_SPAN,
			FM_DEF_ROW_SPAN);

	widget->labels_list = eina_list_append(widget->labels_list, label);
}

static void _navi_path_widget_destroy(navi_path_widget *widget)
{
    free(widget);
}

static void _navi_path_widget_label_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	RETM_IF(!data, "Data is NULL");
	RETM_IF(!event_info, "Event info is NULL");

	navi_path_widget *widget = data;
	Evas_Event_Mouse_Down *ei = event_info;

	widget->was_down = EINA_TRUE;
	widget->down_time_ms = ei->timestamp;
	widget->down_pos_x = ei->canvas.x;
}
static void _navi_path_widget_label_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	RETM_IF(!data, "Data is NULL");
	RETM_IF(!obj, "Object is NULL");
	RETM_IF(!event_info, "Event info is NULL");

	navi_path_widget *widget = data;
	Evas_Event_Mouse_Down *event = event_info;

	if (!widget->was_down) {
		return;
	}
	widget->was_down = EINA_FALSE;

	if ((event->timestamp - widget->down_time_ms) > FM_CLICK_MAX_DURATION_MS) {
		return;
	}

	if (abs(event->canvas.x - widget->down_pos_x) > FM_CLICK_MIN_X_DISTANCE_PIX) {
		return;
	}

	_navi_path_widget_label_clicked(widget, obj);
}

static void _navi_path_widget_label_clicked(navi_path_widget *widget, Evas_Object *label)
{
	int pos = 0;
	elm_table_pack_get(label, &pos, NULL, NULL, NULL);

	int view_index = pos + FM_NAVIFRAME_ITEM_INDEX_MAIN;
	int res = navigator_goto_view_by_index(widget->view->app->navigator, view_index);
	RETM_IF(res != RESULT_TYPE_OK, "Failed to navigate to view by index '%d'", view_index);
}

static void _navi_path_widget_delete_cb(void *data, Evas *evas, Evas_Object *obj,
		void *event_info)
{
	_navi_path_widget_destroy(data);
}
