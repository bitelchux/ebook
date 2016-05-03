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

#ifndef _NAVIGATOR_H_
#define _NAVIGATOR_H_

#include "view/view.h"
#include "main-app.h"

/* Forward declaration: */
typedef struct _navigator navigator;

/**
 * @brief Create navigator instance
 * @return File system manager instance on success, otherwise NULL
 */
navigator *navigator_create(app_data *data);

/**
 * @brief Destroy navigator instance
 * @param[in]   navi    Navigator instance
 */
void navigator_destroy(navigator *navi);

/**
 * @brief Add view to navigator
 * @param[in]   navi        Navigator instance
 * @param[in]   view_title  View title
 * @param[in]   view        View data
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navigator_add_view(navigator *navi, const char *view_title, view_data *view);

/**
 * @brief Make navigation to previous view
 * @param[in]   navi        Navigator instance
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navigator_goto_previous_view(navigator *navi);

/**
 * @brief Make navigation to root view
 * @param[in]   navi        Navigator instance
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navigator_goto_root_view(navigator *navi);

/**
 * @brief Make navigation to view by its index
 * @param[in]   navi        Navigator instance
 * @param[in]   index       View index in stack
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navigator_goto_view_by_index(navigator *navi, int index);

#endif /* _NAVIGATOR_H_ */
