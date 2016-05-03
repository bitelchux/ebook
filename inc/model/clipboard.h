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

#ifndef CLIPBOARD_H_
#define CLIPBOARD_H_

#include "utils/app-types.h"

/* Forward declaration: */

typedef struct _clipboard clipboard;

/**
 * @brief Create clipboard instance
 * @return Clipboard instance on success, otherwise NULL
 */
clipboard *clipboard_create();

/**
 * @brief Destroy clipboard instance
 * @param[in]   clipboard_obj   Clipboard instance
 */
void clipboard_destroy(clipboard *clipboard_obj);

/**
 * @brief Save file list to clipboard
 * @param[in]   clipboard_obj   Clipboard instance
 * @param[in]   file_list       File list to save
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_add_data(clipboard *clipboard_obj, Eina_List *file_list);

/**
 * @brief Get file list from clipboard
 * @param[in]   clipboard_obj   Clipboard instance
 * @param[out]  file_list       File list get from clipboard
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_get_data(clipboard *clipboard_obj, Eina_List **file_list);

/**
 * @brief Save operation type to clipboard
 * @param[in]   clipboard_obj   Clipboard instance
 * @param[in]   type            Type of operation to save
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_set_operation(clipboard *clipboard_obj, operation_type type);

/**
 * @brief Get operation type from clipboard
 * @param[in]   clipboard_obj   Clipboard instance
 * @param[out]  type            Type of operation get from clipboard
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_get_operation(clipboard *clipboard_obj, operation_type *type);

/**
 * @brief Check if clipboard is empty
 * @param[in]   clipboard_obj   Clipboard instance
 * @param[out]  empty           Empty status get from clipboard. EINA_TRUE - if is empty, EINA_FALSE - otherwise
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_is_empty(clipboard *clipboard_obj, Eina_Bool *empty);

/**
 * @brief Clear all data saved in clipboard
 * @param[in]   clipboard_obj   Clipboard instance
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int clipboard_clear(clipboard *obj);

#endif /* CLIPBOARD_H_ */
