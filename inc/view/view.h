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

#ifndef VIEW_H_
#define VIEW_H_

#include <Elementary.h>
#include <efl_extension.h>
// Forward declaration:
struct _app_data;

/**
 * @brief View edit mode data
 */
typedef struct _edit_data {
	int checked_count;          /**< Counter how many items checked */
	Eina_Bool is_all_selected;  /**< Had all items are selected or not*/
} edit_data;

/**
 * @brief View data
 */
typedef struct _view {
	struct _app_data *app;      /**< A descriptor of the application. */
	Evas_Object *navi;          /**< Naviframe object */
	Evas_Object *navi_layout;   /**< Layout which pushed to naviframe */
	Elm_Object_Item *navi_item; /**< NaviItem object */
	Eina_Bool is_root;          /**< Current path is root or not */
	char *curr_path;            /**< Current directory path */
//	edit_data edit;             /**< View edit mode object */
} view_data;

#endif /* VIEW_H_ */
