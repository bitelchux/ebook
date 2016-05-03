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

#ifndef GENLIST_H_
#define GENLIST_H_

#include "view/view.h"
#include <Elementary.h>
#include <efl_extension.h>
Elm_Genlist_Item_Class itc;

/**
 * @brief Genlist widget data
 */
typedef struct {
	view_data *view;                /**< Parent view */
	Evas_Object *genlist;           /**< Genlist object */
	Evas_Object *sel_all_checkbox;  /**< 'Select all' checkbox object */
	Evas_Object *sel_all_layout;    /**< 'Select all' layout object */
	Evas_Object *box;               /**< Box object */
} genlist_widget;

/**
 *@brief Add new genlist widget to view
 *@param[in]   view     View object.
 *@return Genlist widget on success or NULL.
 */
genlist_widget *genlist_widget_add(view_data *view);

/**
 *@brief Set content to genlist
 *@param[in]   widget      Widget object.
 *@param[in]   file_list   List with file items.
 */
void genlist_widget_content_set(genlist_widget *widget, const Eina_List *file_list);

/**
 *@brief Update genlist widget
 *@param[in]   widget      Widget object.
 *@return Error code. RESULT_TYPE_OK if operation success.
 */
int genlist_widget_update(genlist_widget *widget);

/**
 *@brief Clear genlist
 *@param[in]   widget      Widget object.
 */
void genlist_widget_clear(genlist_widget *widget);

#endif /*GENLIST_H_ */
