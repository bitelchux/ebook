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

#include "utils/ui-utils.h"
#include "utils/model-utils.h"
#include "utils/app-types.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "main-app.h"
#include "view/view.h"

#include <app.h>

Evas_Object *ui_utils_layout_add(Evas_Object *parent, Evas_Object_Event_Cb destroy_cb, void *cb_data)
{
	Evas_Object *layout = elm_layout_add(parent);
	RETVM_IF(!layout, NULL , "Layout is NULL");

	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_event_callback_add(layout, EVAS_CALLBACK_FREE, destroy_cb, cb_data);

	evas_object_show(layout);

	return layout;
}

Evas_Object *ui_utils_genlist_add(Evas_Object *parent, Evas_Object_Event_Cb destroy_cb, void *cb_data)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	RETVM_IF(!genlist, NULL, "Genlist is NULL");

	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_event_callback_add(genlist, EVAS_CALLBACK_FREE, destroy_cb, cb_data);

	evas_object_show(genlist);

	return genlist;
}

Evas_Object *ui_utils_navi_add(Evas_Object *parent, Eext_Event_Cb back_cb, void *cb_data)
{
	RETVM_IF(!parent, NULL, "Parent is NULL");

	Evas_Object *navi = elm_naviframe_add(parent);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK, back_cb, cb_data);
	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_FALSE);

	evas_object_show(navi);

	return navi;
}

Evas_Object *ui_utils_navi_layout_get(app_data *app)
{
	RETVM_IF(!app, NULL, "App object is NULL");
	RETVM_IF(!app->navi, NULL, "Naviframe object is NULL");

	Elm_Object_Item *item = elm_naviframe_top_item_get(app->navi);
	RETVM_IF(!item, NULL, "Item object is NULL");

	Evas_Object *navi_layout = elm_object_item_data_get(item);
	RETVM_IF(!navi_layout, NULL, "Navi layout object is NULL");

	return navi_layout;
}

const char *ui_utils_title_get(const char* curr_path)
{
	const char *title = NULL;
	const storage_type storage = model_utils_get_storage_type(curr_path);
	switch (storage) {
	case STORAGE_TYPE_PHONE:
		title = FM_PHONE_LABEL;
		break;
	default:
		ERR("Failed to get title");
		break;
	}

	return title;
}

static const char *_ui_utils_get_res_path()
{
	static char res_folder_path[PATH_MAX] = {'\0'};
	if (res_folder_path[0] == '\0') {
		char *res_path_buff = app_get_resource_path();
		strncpy(res_folder_path, res_path_buff, PATH_MAX-1);
		free(res_path_buff);
	}
	return res_folder_path;
}

const char *ui_utils_get_resource(const char *res_name)
{
	RETVM_IF(!res_name, NULL, "res_name is NULL");

	static char res_path[PATH_MAX] = {'\0'};
	snprintf(res_path, PATH_MAX, "%s%s", _ui_utils_get_res_path(), res_name);
	return res_path;
}
