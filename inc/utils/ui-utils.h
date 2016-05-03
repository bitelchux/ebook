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

#ifndef UI_UTILS_H_
#define UI_UTILS_H_

#include <Evas.h>
#include <efl_extension.h>

struct _app_data;

/**
 * @brief Adds a new Naviframe object to the parent.
 * @param[in]   parent    Parent object
 * @param[in]   back_cb   The function to be called when the back button event is triggered.
 * @param[in]   cb_data   The data pointer to be passed to @p back_cb.
 * @return Naviframe object on success or NULL.
 */
Evas_Object *ui_utils_navi_add(Evas_Object *parent, Eext_Event_Cb back_cb, void *cb_data);

/**
 * @brief Adds a new Layout object to the parent.
 * @param[in]   parent       Parent object
 * @param[in]   destroy_cb   The function to be called when object is deleted
 * @return Layout object on success or NULL.
 */
Evas_Object *ui_utils_layout_add(Evas_Object *parent, Evas_Object_Event_Cb destroy_cb, void *cb_data);

/**
 * @brief Adds a new genlist object to the parent.
 * @param[in]   parent       Parent object
 * @param[in]   destroy_cb   The function to be called when object is deleted
 * @param[in]   cb_data      The data pointer to be passed to @p destroy_cb.
 * @return Genlist object on success or NULL.
 */
Evas_Object *ui_utils_genlist_add(Evas_Object *parent, Evas_Object_Event_Cb destroy_cb, void *cb_data);

/**
 * @brief Returns title dependent on current path.
 * @param[in]   curr_path       The current path.
 * @return Title for current path.
 */
const char *ui_utils_title_get(const char* curr_path);

/**
 * @brief Returns full path to the resource file by its name
 * @param[in]   res_name    Name of the resource
 * @return Full path to the resource file. NULL if fails.
 */
const char *ui_utils_get_resource(const char *res_name);

#endif /* UI_UTILS_H_ */
