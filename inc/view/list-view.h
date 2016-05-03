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

#ifndef LIST_VIEW_H_
#define LIST_VIEW_H_

#include "view/view.h"
#include <Elementary.h>

struct _app_data;

typedef enum {
	UPDATE_TYPE_VIEW = 0x0001,
	UPDATE_TYPE_GENLIST = 0x0002,
	UPDATE_TYPE_CTRL_BAR_MODE = 0x0004,
	UPDATE_TYPE_CTRL_BAR_STATE = 0x0008
} update_type;

/**
 * @brief Creates list view for concrete directory
 * @param[in]   app      A descriptor of the application.
 * @param[in]   parent   Parent object
 * @param[in]   path     The path of directory
 * @param[in]   dir_name Directory name
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_add(struct _app_data *app, Evas_Object *parent, const char *path, const char *dir_name);

/**
 * @brief Updates view
 * @param[in]   navi_item        Navi Item which needs update.
 * @param[in]   update_options   Parameter for check which widget update.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_update(Elm_Object_Item *navi_item, int update_options);

/**
 * @brief Returns counter items in view
 * @param[in]   view   View object.
 * @return Items counter.
 */
unsigned int list_view_items_count_get(view_data *view);

/**
 * @brief Creates new folder
 * @param[in]   view          View object.
 * @param[in]   folder_name   Folder name.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_create_folder(view_data *view, const char *folder_name);

/**
 * @brief Set all items select status
 * @param[in]   view    View object.
 * @param[in]   state   Selected state.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_select_all(view_data *view, Eina_Bool state);

/**
 * @brief Delete items from view
 * @param[in]   view   View object.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_delete_items(view_data *view);

/**
 * @brief Sets selected items to clipboard and sets operation type "copy"
 * @param[in]   view   View object.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_copy_items(view_data *view);

/**
 * @brief Sets selected items to clipboard and sets operation type "move"
 * @param[in]   view   View object.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_move_items(view_data *view);

/**
 * @brief Past items items from clipboard
 * @param[in]   view   View object.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int list_view_past_items(view_data *view);

#endif /* LIST_VIEW_H_ */
