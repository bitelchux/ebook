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

#include "view/list-view.h"
//#include "view/ctrl-bar-view.h"
#include "view/genlist-widget.h"
#include "view/navi-path-widget.h"
//#include "view/navi-bar-title.h"
#include "view/navigator.h"
//#include "view/popup.h"
#include "utils/app-types.h"
#include "utils/ui-utils.h"
#include "utils/common-utils.h"
#include "utils/model-utils.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "model/fs-manager.h"
//#include "model/getclipboard.h"
#include "model/navi-path-storage.h"
#include "main-app.h"

typedef struct {
	view_data list_view;
//	title_widget *title_wgt;
	genlist_widget *genlist_wgt;
//	ctrl_bar_widget *ctrl_bar_wgt;
	navi_path_widget *navi_path_wgt;
	Eina_List *file_list;
} list_view_data;

typedef struct {
	list_view_data *list_view;
	Evas_Object *wait_popup;
} oper_callback_data;

static const char *const PROGRESS_TEXT = "Operation in progress";

static int _list_view_create_widgets(list_view_data *lv_data);
//static int _list_view_update_content(list_view_data *lv_data);
static void _list_view_destroy(list_view_data *lv_data);

static void _list_view_del_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
//static void _list_view_operation_complete(void *data, int result);
//static void _list_view_operation_move_cb(void *data, int result);
//static void _list_view_operation_copy_cb(void *data, int result);
//static void _list_view_operation_delete_cb(void *data, int result);
//static oper_callback_data *_list_view_oper_cb_data_create(list_view_data* lv_data);
//static void _list_view_oper_cb_data_destroy(oper_callback_data *data);
static Evas_Object *_list_view_progress_popup_create(list_view_data *lv_data);
static void _progress_popup_back_button_cb(void *data, Evas_Object *obj, void *event_info);

int list_view_add(app_data *app, Evas_Object *parent, const char *path, const char *dir_name)
	{
	RETVM_IF(!app, RESULT_TYPE_INVALID_ARG, "App object is NULL");
	RETVM_IF(!parent, RESULT_TYPE_INVALID_ARG, "Parent object is NULL");
	RETVM_IF(!path, RESULT_TYPE_INVALID_ARG, "Path is NULL");

	app->status.is_mainview = EINA_FALSE;

	list_view_data *data = calloc(1, sizeof(list_view_data));
	RETVM_IF(!data, RESULT_TYPE_FAIL_ALLOCATE_MEMORY, "Fail allocate memory");

	data->list_view.app = app;
	data->list_view.navi = parent;
	data->list_view.curr_path = (dir_name) ? common_util_strconcat(path, "/", dir_name, NULL)
									  : strdup(path);
	data->list_view.is_root = model_utils_is_root_path(data->list_view.curr_path);

	data->file_list = NULL;

	data->list_view.navi_layout = ui_utils_layout_add(data->list_view.navi, _list_view_del_cb, data);
	if (!data->list_view.navi_layout) {
		ERR("Fail to create Layout");
		_list_view_destroy(data);
		return RESULT_TYPE_FAIL;
	}

	elm_layout_file_set(data->list_view.navi_layout, ui_utils_get_resource(FM_LAYOUT_EDJ), "navi_layout");

	int result = fs_manager_get_file_list(data->list_view.app->manager, data->list_view.curr_path, &data->file_list);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to get file list");
		evas_object_del(data->list_view.navi_layout);
		return result;
	}

	const char *title = ui_utils_title_get(data->list_view.curr_path);
	result = navigator_add_view(data->list_view.app->navigator, title, &data->list_view);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to add view to naviframe");
		evas_object_del(data->list_view.navi_layout);
		return result;
	}

	result = _list_view_create_widgets(data);
	if (result != RESULT_TYPE_OK) {
		ERR("Fail to create widgets");
		evas_object_del(data->list_view.navi_layout);
		return result;
	}

	Eina_List *path_list = NULL;
	int res = navi_path_storage_get_path_list(app->path_storage, &path_list);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail get folders list for navigation widget");
	navi_path_widget_content_set(data->navi_path_wgt, path_list);

	elm_object_item_data_set(data->list_view.navi_item, data);

	return RESULT_TYPE_OK;
}

int list_view_update(Elm_Object_Item *navi_item, int update_options)
{
	RETVM_IF(!navi_item, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *data = elm_object_item_data_get(navi_item);
	RETVM_IF(!data, RESULT_TYPE_INVALID_ARG, "Data is NULL");
	RETVM_IF(!data->genlist_wgt, RESULT_TYPE_INVALID_ARG, "Genlist widget is NULL");
//	RETVM_IF(!data->ctrl_bar_wgt, RESULT_TYPE_INVALID_ARG, "Ctrl bar widget is NULL");
/*
	if (update_options & UPDATE_TYPE_VIEW) {
		RETVM_IF(_list_view_update_content(data) != RESULT_TYPE_OK,
				RESULT_TYPE_FAIL, "Fail to update view content");
	}

	if (update_options & UPDATE_TYPE_GENLIST) {
		RETVM_IF(genlist_widget_update(data->genlist_wgt) != RESULT_TYPE_OK,
				RESULT_TYPE_FAIL, "Fail to update genlist widget");
	}

	if (update_options & UPDATE_TYPE_CTRL_BAR_MODE) {
		RETVM_IF(ctrl_bar_widget_mode_update(data->ctrl_bar_wgt) != RESULT_TYPE_OK,
				RESULT_TYPE_FAIL, "Fail to update ctrl bar");
	}

	if (update_options & UPDATE_TYPE_CTRL_BAR_STATE) {
		RETVM_IF(ctrl_bar_widget_state_update(data->ctrl_bar_wgt) != RESULT_TYPE_OK,
				RESULT_TYPE_FAIL, "Fail to update ctrl bar");
	}
*/
	return RESULT_TYPE_OK;
}

unsigned int list_view_items_count_get(view_data *view)
{
	list_view_data *data = (list_view_data *)view;

	return eina_list_count(data->file_list);
}
/*
int list_view_create_folder(view_data *view, const char *folder_name)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	char *new_folder = common_util_strconcat(view->curr_path, "/", folder_name, NULL);

	int res = fs_manager_create_folder(view->app->manager, new_folder);

	free(new_folder);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Model create folder Fail");

	return RESULT_TYPE_OK;
}
*/
static void _progress_popup_back_button_cb(void *data, Evas_Object *obj, void *event_info)
{
}

Evas_Object *_list_view_progress_popup_create(list_view_data *lv_data)
{
	RETVM_IF(!lv_data, NULL, "View obj is NULL");

	Evas_Object *popup = elm_popup_add(lv_data->list_view.navi);
	RETVM_IF(!popup, NULL, "Fail to create popup");

	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

	Evas_Object *layout = ui_utils_layout_add(popup, NULL, NULL);
	if (!layout) {
		ERR("Fail to create layout");
		evas_object_del(popup);
		return NULL;
	}
	elm_layout_file_set(layout, ui_utils_get_resource(FM_LAYOUT_EDJ), "processing_view");

	Evas_Object *progressbar = elm_progressbar_add(layout);
	if (!progressbar) {
		ERR("Fail to create process bar");
		evas_object_del(popup);
		return NULL;
	}

	elm_object_style_set(progressbar, "process_medium");
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);

	elm_object_part_content_set(layout, "processing", progressbar);
	elm_object_part_text_set(layout, "elm.text", PROGRESS_TEXT);

	evas_object_show(layout);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _progress_popup_back_button_cb, NULL);
	elm_object_content_set(popup, layout);

	evas_object_show(popup);

	return popup;
}
/*
static oper_callback_data *_list_view_oper_cb_data_create(list_view_data* lv_data)
{
	RETVM_IF(!lv_data, NULL, "View data is NULL");

	oper_callback_data *cb_data = calloc(1, sizeof(oper_callback_data));
	RETVM_IF(!cb_data, NULL, "Failed to allocate memory for callback data");

	Evas_Object *progress_popup = _list_view_progress_popup_create(lv_data);
	if (!progress_popup) {
		ERR("Failed to create progress popup");
		_list_view_oper_cb_data_destroy(cb_data);
		return NULL;
	}

	cb_data->list_view = lv_data;
	cb_data->wait_popup = progress_popup;

	return cb_data;
}

static void _list_view_oper_cb_data_destroy(oper_callback_data *data)
{
    if (data) {
        evas_object_del(data->wait_popup);
        free(data);
    }
}
/*
int list_view_delete_items(view_data *view)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *list_data = (list_view_data *)view;

	oper_callback_data *cb_data = _list_view_oper_cb_data_create(list_data);
	RETVM_IF(!cb_data, RESULT_TYPE_FAIL, "Failed to create callback data");

	int res = fs_manager_delete_files(list_data->list_view.app->manager,
			list_data->file_list,
			_list_view_operation_delete_cb,
			cb_data);

	if (res != RESULT_TYPE_OK) {
		ERR("Failed to delete selected files");
		_list_view_oper_cb_data_destroy(cb_data);
		return res;
	}

	return res;
}

int list_view_copy_items(view_data *view)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *list_data = (list_view_data *)view;
	app_data *app = list_data->list_view.app;

	int res = clipboard_add_data(app->clipboard, list_data->file_list);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to copy files");

	res = clipboard_set_operation(app->clipboard, OPERATION_TYPE_COPY);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to set operation");

	return RESULT_TYPE_OK;
}

int list_view_move_items(view_data *view)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *list_data = (list_view_data *)view;
	app_data *app = list_data->list_view.app;

	int res = clipboard_add_data(app->clipboard, list_data->file_list);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to copy files");

	res = clipboard_set_operation(app->clipboard, OPERATION_TYPE_MOVE);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to set operation");

	return RESULT_TYPE_OK;
}

int list_view_past_items(view_data *view)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *list_data = (list_view_data *)view;
	app_data *app = list_data->list_view.app;

	Eina_List *clipboard_list = NULL;
	int res = clipboard_get_data(app->clipboard, &clipboard_list);
	RETVM_IF(res != RESULT_TYPE_OK || !clipboard_list, res, "Fail to get files from clipboard");

	operation_type operation = OPERATION_TYPE_NONE;
	res = clipboard_get_operation(app->clipboard, &operation);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Fail to get operation type");

	oper_callback_data *cb_data = _list_view_oper_cb_data_create(list_data);
	RETVM_IF(!cb_data, RESULT_TYPE_FAIL, "Failed to create callback data");

	if (operation == OPERATION_TYPE_COPY) {
		res = fs_manager_copy_files(app->manager,
				clipboard_list,
				list_data->list_view.curr_path,
				_list_view_operation_copy_cb,
				cb_data);
	} else if (operation == OPERATION_TYPE_MOVE) {
		res = fs_manager_move_files(app->manager,
				clipboard_list,
				list_data->list_view.curr_path,
				_list_view_operation_move_cb,
				cb_data);
	} else {
		ERR("Wrong operation type");
		res = RESULT_TYPE_FAIL;
	}

	if (res != RESULT_TYPE_OK) {
		ERR("Failed to paste selected files");
		_list_view_oper_cb_data_destroy(cb_data);
		return res;
	}

	return res;
}

int list_view_select_all(view_data *view, Eina_Bool state)
{
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View object is NULL");

	list_view_data *data = (list_view_data *)view;

	Eina_List *list = NULL;
	void *item = NULL;

	EINA_LIST_FOREACH(data->file_list, list, item) {
		node_info *node = item;
		node->is_selected = state;
	}
	return RESULT_TYPE_OK;
}
*/
static int _list_view_create_widgets(list_view_data *lv_data)
{
	lv_data->navi_path_wgt = navi_path_widget_add((view_data *)lv_data); //path print
	RETVM_IF(!lv_data->navi_path_wgt, RESULT_TYPE_FAIL, "Fail to create navigation widget");

	lv_data->genlist_wgt = genlist_widget_add((view_data *)lv_data);
	RETVM_IF(!lv_data->genlist_wgt, RESULT_TYPE_FAIL, "Fail to create genlist widget");
	genlist_widget_content_set(lv_data->genlist_wgt, lv_data->file_list);

//	lv_data->ctrl_bar_wgt = ctrl_bar_widget_add((view_data *)lv_data); // after phone, no print
//	RETVM_IF(!lv_data->ctrl_bar_wgt, RESULT_TYPE_FAIL, "Fail to create ctrl bar widget");

	if (!lv_data->list_view.is_root) {
	//	lv_data->title_wgt = navi_bar_title_widget_add((view_data *)lv_data);
		// control bar create
	//	RETVM_IF(!lv_data->title_wgt, RESULT_TYPE_FAIL, "Fail to create title widget");
	}

	return RESULT_TYPE_OK;
}
/*
static int _list_view_update_content(list_view_data *lv_data)
{
	common_util_clear_file_list(&lv_data->file_list);

//	int result = fs_manager_get_file_list(lv_data->list_view.app->manager, lv_data->list_view.curr_path, &lv_data->file_list);
//	RETVM_IF(result != RESULT_TYPE_OK, result, "Fail to get file list");

	genlist_widget_clear(lv_data->genlist_wgt);
	genlist_widget_content_set(lv_data->genlist_wgt, lv_data->file_list);

	return RESULT_TYPE_OK;
}*/
/*
static void _list_view_operation_move_cb(void *data, int result)
{
	_list_view_operation_complete(data, result);
}
static void _list_view_operation_copy_cb(void *data, int result)
{
	_list_view_operation_complete(data, result);
}
static void _list_view_operation_delete_cb(void *data, int result)
{
	RETM_IF(!data, "Data is NULL");

	oper_callback_data *cb_data = (oper_callback_data *)data;
	list_view_data *list_data = cb_data->list_view;

	_list_view_oper_cb_data_destroy(cb_data);

	if (result != RESULT_TYPE_OK) {
		RETM_IF(popup_create((view_data *)list_data, POPUP_TYPE_ERROR), "Fail to create error popup");
	} else {
		list_data->list_view.app->status.curr_mode = MODE_DEFAULT;
		int res = list_view_update(list_data->list_view.navi_item,
				UPDATE_TYPE_GENLIST |
				UPDATE_TYPE_CTRL_BAR_MODE |
				UPDATE_TYPE_VIEW);

		RETM_IF(res != RESULT_TYPE_OK, "Fail to update view");
	}
}

static void _list_view_operation_complete(void *data, int result)
	{
	RETM_IF(!data, "Data is NULL");

	oper_callback_data *cb_data = (oper_callback_data *)data;
	list_view_data *list_data = cb_data->list_view;

	_list_view_oper_cb_data_destroy(cb_data);

	if (result != RESULT_TYPE_OK) {
		ERR("File operation failed");

		operation_type operation = OPERATION_TYPE_NONE;
		int res = clipboard_get_operation(list_data->list_view.app->clipboard, &operation);

		if (res != RESULT_TYPE_OK) {
			ERR("Fail to get operation type");
			res = popup_create((view_data *)list_data, POPUP_TYPE_ERROR);
		} else {
			switch (operation) {
			case OPERATION_TYPE_MOVE:
				if (result == RESULT_TYPE_OPERATION_INVALID_DEST) {
					res = popup_create((view_data *)list_data, POPUP_TYPE_MOVE_RECURSIVE_FAIL);
				} else {
					res = popup_create((view_data *)list_data, POPUP_TYPE_FAIL_TO_MOVE);
				}
				break;
			case OPERATION_TYPE_COPY:
				if (result == RESULT_TYPE_OPERATION_INVALID_DEST) {
					res = popup_create((view_data *)list_data, POPUP_TYPE_COPY_RECURSIVE_FAIL);
				} else {
					res = popup_create((view_data *)list_data, POPUP_TYPE_FAIL_TO_COPY);
				}
				break;
			default:
				res = popup_create((view_data *)list_data, POPUP_TYPE_ERROR);
				break;
			}
		}

		if (res != RESULT_TYPE_OK) {
			ERR("Fail to create error popup");
		}
	} else {
		list_data->list_view.app->status.curr_mode = MODE_DEFAULT;
		int res = list_view_update(list_data->list_view.navi_item,
				UPDATE_TYPE_GENLIST |
				UPDATE_TYPE_CTRL_BAR_MODE |
				UPDATE_TYPE_VIEW);

		if (res != RESULT_TYPE_OK) {
			ERR("Fail to update view");
		}
	}

	if (result != RESULT_TYPE_OPERATION_INVALID_DEST) {
		result = clipboard_clear(list_data->list_view.app->clipboard);
		RETM_IF(result != RESULT_TYPE_OK, "Fail to clear clipboard");
	}
}
*/
static void _list_view_destroy(list_view_data *lv_data)
{
	if (lv_data) {
		app_data *app = lv_data->list_view.app;

		app->status.curr_mode = MODE_DEFAULT;

		common_util_clear_file_list(&lv_data->file_list);

		free(lv_data->list_view.curr_path);
		free(lv_data);
	}
}

static void _list_view_del_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	_list_view_destroy(data);
}
