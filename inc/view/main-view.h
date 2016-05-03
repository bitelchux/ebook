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

#ifndef MAIN_VIEW_H_
#define MAIN_VIEW_H_

#include <Elementary.h>

struct _app_data;

/**
 * @brief Creates main view
 * @param[in]   app      A descriptor of the application.
 * @param[in]   parent   Parent object
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int main_view_add(struct _app_data *app, Evas_Object *parent);

/**
 * @brief Updates main view
 * @param[in]   navi_item        Navi Item which needs update.
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int main_view_update(Elm_Object_Item *navi_item);

#endif /* MAIN_VIEW_H_ */
