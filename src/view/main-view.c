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

#include "view/main-view.h"
//#include "view/ctrl-bar-view.h"
#include "view/genlist-widget.h"
#include "view/navigator.h"
#include "view/view.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "utils/ui-utils.h"
#include "utils/common-utils.h"
#include "utils/app-types.h"
#include "model/fs-manager.h"
#include "main-app.h"

#define FM_MAIN_VIEW_TITLE "Storages"

typedef struct {
	view_data view;
	genlist_widget *genlist_widget;
//	ctrl_bar_widget *ctrl_widget;
	Eina_List *storage_list;
} main_view_data;

static int _main_view_fill(main_view_data *data);
static int _main_view_create_widgets(main_view_data *data);
static void _main_view_destroy(main_view_data *data);

static void _main_view_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

int main_view_add(app_data *app, Evas_Object *parent)
{
	RETVM_IF(!app, RESULT_TYPE_INVALID_ARG, "App object is NULL");
	RETVM_IF(!parent, RESULT_TYPE_INVALID_ARG, "Parent object is NULL");

	app->status.is_mainview = EINA_TRUE;

	main_view_data *data = calloc(1, sizeof(main_view_data));
	RETVM_IF(!data, RESULT_TYPE_FAIL_ALLOCATE_MEMORY, "Fail allocate memory");

	data->view.app = app;
	data->view.navi = parent;

	data->storage_list = NULL;
	data->view.is_root = EINA_FALSE;

	data->view.navi_layout = ui_utils_layout_add(data->view.navi, _main_view_del_cb, data);
	if (!data->view.navi_layout) {
		ERR("Fail to create layout");
		_main_view_destroy(data);
		return RESULT_TYPE_FAIL;
	}

	elm_layout_file_set(data->view.navi_layout, ui_utils_get_resource(FM_LAYOUT_EDJ), "navi_layout");

	int result = navigator_add_view(data->view.app->navigator, FM_MAIN_VIEW_TITLE, &data->view);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to add view to naviframe");
		evas_object_del(data->view.navi_layout);
		return result;
	}

	result = _main_view_create_widgets(data);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to create widgets");
		evas_object_del(data->view.navi_layout);
		return result;
	}

	result = _main_view_fill(data);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to initialize main view");
		evas_object_del(data->view.navi_layout);
		return result;
	}

	elm_object_item_data_set(data->view.navi_item, data);

	return RESULT_TYPE_OK;
}

int main_view_update(Elm_Object_Item *navi_item)
{
	RETVM_IF(!navi_item, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	main_view_data *data = elm_object_item_data_get(navi_item);
	RETVM_IF(!data, RESULT_TYPE_INVALID_ARG, "Data is NULL");
	RETVM_IF(!data->genlist_widget, RESULT_TYPE_INVALID_ARG, "Genlist widget is NULL");

	common_util_clear_storage_list(&data->storage_list);
	genlist_widget_clear(data->genlist_widget);

	return _main_view_fill(data);
}

static int _main_view_create_widgets(main_view_data *data)
{
	data->genlist_widget = genlist_widget_add((view_data *)data);
	RETVM_IF(!data->genlist_widget, RESULT_TYPE_INVALID_ARG, "Fail to create genlist widget");

//	data->ctrl_widget = ctrl_bar_widget_add((view_data *)data);
//	RETVM_IF(!data->ctrl_widget, RESULT_TYPE_INVALID_ARG, "Fail to create ctrl bar widget");

	return RESULT_TYPE_OK;
}

static int _main_view_fill(main_view_data *data)
{
	int res = fs_manager_get_storage_list(data->view.app->manager, &data->storage_list);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to get storage list");

	genlist_widget_content_set(data->genlist_widget, data->storage_list);
	return RESULT_TYPE_OK;
}

static void _main_view_destroy(main_view_data *data)
{
	if (data) {
		common_util_clear_storage_list(&data->storage_list);
		free(data);
	}
}

static void _main_view_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_main_view_destroy(data);
}
