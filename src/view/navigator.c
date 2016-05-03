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

#include "view/navigator.h"
#include "view/main-view.h"
#include "view/list-view.h"
#include "utils/ui-utils.h"
#include "utils/app-types.h"
#include "utils/logger.h"
#include "utils/common-utils.h"
#include "model/navi-path-storage.h"

#define FM_NAVI_ITEM_INDEX_MAIN 1
#define FM_NAVI_ITEM_INDEX_ROOT 2
#define FM_NAVI_ITEM_INDEX_PREV(navi)   eina_list_count(navi->view_list) - 1

struct _navigator {
	app_data *app;          /**< Application data pointer */
	Eina_List *view_list;   /**< List of views in stack */
};

static int _navigator_remove_items_to_index(navigator *navi, int index);

navigator *navigator_create(app_data *data)
{
	navigator *navi = calloc(1, sizeof(navigator));
	if (navi) {
		navi->app = data;
	}
	return navi;
}

void navigator_destroy(navigator *navi)
{
	if (navi) {
		eina_list_free(navi->view_list);
		free(navi);
	}
}

int navigator_add_view(navigator *navi, const char *view_title, view_data *view)
{
	RETVM_IF(!navi, RESULT_TYPE_INVALID_ARG, "Controller is NULL"); //if no item, have a problem.
	RETVM_IF(!view, RESULT_TYPE_INVALID_ARG, "View data is NULL");
	view->navi_item = elm_naviframe_item_push(view->navi, view_title, NULL, NULL, view->navi_layout, NULL);
	RETVM_IF(!view->navi_item, RESULT_TYPE_FAIL, "Failed to add view to naviframe");

	navi->view_list = eina_list_append(navi->view_list, view);
	int list_size = eina_list_count(navi->view_list);

	if (list_size != FM_NAVI_ITEM_INDEX_MAIN) {
		const char *label_name = NULL;
		if (list_size == FM_NAVI_ITEM_INDEX_ROOT) {
			label_name =  ui_utils_title_get(view->curr_path);
		} else {
			label_name = common_util_get_filename(view->curr_path);
		}

		int res = navi_path_storage_add_folder(navi->app->path_storage, label_name);
		RETVM_IF(res != RESULT_TYPE_OK , res, "Failed to add folder '%s' to storage", label_name);
	 }

	return RESULT_TYPE_OK;
}

int navigator_goto_previous_view(navigator *navi)
{
	RETVM_IF(!navi, RESULT_TYPE_INVALID_ARG, "Controller is NULL");

	return navigator_goto_view_by_index(navi, FM_NAVI_ITEM_INDEX_PREV(navi));
}

int navigator_goto_root_view(navigator *navi)
{
	RETVM_IF(!navi, RESULT_TYPE_INVALID_ARG, "Controller is NULL");

	return navigator_goto_view_by_index(navi, FM_NAVI_ITEM_INDEX_ROOT);
}

int navigator_goto_view_by_index(navigator *navi, int index)
{
	RETVM_IF(!navi, RESULT_TYPE_INVALID_ARG, "Controller is NULL");

	int res =  RESULT_TYPE_FAIL;

	navi->app->status.curr_mode = MODE_DEFAULT;

	int list_size = eina_list_count(navi->view_list);
	RETVM_IF(list_size < index, RESULT_TYPE_INVALID_ARG, "Requested view index '%d' is to big", index);

	int path_index = index - FM_NAVI_ITEM_INDEX_MAIN;
	res = navi_path_storage_remove_path_to_index(navi->app->path_storage, path_index);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to remove path_list to index", path_index);

	res = _navigator_remove_items_to_index(navi, index);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to remove views_list to index '%d'", index);

	Eina_List *last_node = eina_list_last(navi->view_list);
	RETVM_IF(!last_node, RESULT_TYPE_FAIL, "Last node is NULL");

	view_data *item = eina_list_data_get(last_node);
	RETVM_IF(!item, RESULT_TYPE_FAIL, "Navi item is NULL");

	elm_naviframe_item_pop_to(item->navi_item);
	if (item->navi_item == elm_naviframe_bottom_item_get(navi->app->navi)) {
		navi->app->status.is_mainview = EINA_TRUE;
		res = main_view_update(item->navi_item);
	} else {
		res = list_view_update(item->navi_item, UPDATE_TYPE_VIEW);
	}
	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to update view");

	return res;
}

static int _navigator_remove_items_to_index(navigator *navi, int index)
{
	RETVM_IF(!navi, RESULT_TYPE_INVALID_ARG, "Controller is NULL");

	int list_size = eina_list_count(navi->view_list);
	RETVM_IF(list_size < index, RESULT_TYPE_INVALID_ARG, "Requested view index '%d' is to big", index);

	while (list_size > index) {
		Eina_List *last_node = eina_list_last(navi->view_list);
		navi->view_list = eina_list_remove_list(navi->view_list, last_node);
		--list_size;
	}

	return RESULT_TYPE_OK;
}
