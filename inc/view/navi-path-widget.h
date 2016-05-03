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

#ifndef _NAVI_PATH_WIDGET_H_
#define _NAVI_PATH_WIDGET_H_

#include "view/view.h"
#include <Elementary.h>

/**
 * @brief Navi path widget data
 */
typedef struct _navi_path_widget {
	view_data *view;                    /**< Parent view */
	Evas_Object *navi_path_layout;      /**< Widget main layout */
	Evas_Object *navi_path_table;       /**< Widget table object */
	Evas_Object *navi_path_scroller;    /**< Widget scroller object */
	Eina_List *labels_list;             /**< List of labels */
	Eina_Bool was_down;                 /**< State if mouse down event was */
	int down_pos_x;                     /**< Coordinate x where mouse down event was */
	unsigned int down_time_ms;          /**< Time when mouse down event was */
	unsigned int table_size;            /**< Table size */
} navi_path_widget;

/**
 *@brief Creates navi path widget
 *@param[in]   view     View object
 *@return Navi path widget on success or NULL.
 */
navi_path_widget *navi_path_widget_add(view_data *view);

/**
 *@brief Set data to view
 *@param[in]   widget       Navi path widget object
 *@param[in]   path_list    Path list to view
 */
void navi_path_widget_content_set(navi_path_widget *widget, Eina_List *path_list);


#endif /* _NAVI_PATH_WIDGET_H_ */
