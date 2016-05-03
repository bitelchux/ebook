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

#ifndef _NAVI_PATH_STORAGE_H_
#define _NAVI_PATH_STORAGE_H_

#include "view/view.h"
#include "main-app.h"

#include <Elementary.h>

/* Forward declaration: */
typedef struct _navi_path_storage navi_path_storage;

/**
 * @brief Create navigation path storage instance
 * @return Navigation path storage instance on success, otherwise NULL
 */
navi_path_storage *navi_path_storage_create();

/**
 * @brief Destroy navigation path storage instance
 * @param[in]   storage     Navigation path storage instance
 */
void navi_path_storage_destroy(navi_path_storage *storage);

/**
 * @brief Destroy navigation path storage instance
 * @param[in]   storage     Navigation path storage instance
 * @param[in]   folder_name Folder name to add
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navi_path_storage_add_folder(navi_path_storage *storage, const char *folder_name);

/**
 * @brief Removes folders from path list to set index
 * @param[in]   storage     Navigation path storage instance
 * @param[in]   index       Path list item index
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navi_path_storage_remove_path_to_index(navi_path_storage *storage, int index);

/**
 * @brief Get path list
 * @param[in]   storage     Navigation path storage instance
 * @param[out]  path_list   Path list to set result
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int navi_path_storage_get_path_list(navi_path_storage *storage, Eina_List **path_list);

#endif /* _NAVI_PATH_STORAGE_H_ */
