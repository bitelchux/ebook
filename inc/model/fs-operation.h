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

#ifndef _FS_OPERATION_H_
#define _FS_OPERATION_H_

#include "model/fs-manager.h"
#include "utils/app-types.h"

#include <Eina.h>

/* Forward declaration: */
typedef struct _fs_operation fs_operation;

/**
 * @brief File system operation callback data
 */
typedef struct _fs_operation_cb_data {
	struct _fs_manager *manager;    /**< file system manager structure pointer */
	int result;                     /**< result of operation */
} fs_operation_cb_data;

/**
 * @brief Prototype of file system operation complete callback function
 */
typedef void (*fs_operation_cb_func)(void *data);

/**
 * @brief Create file system operation instance
 * @return File system operation instance on success, otherwise NULL
 */
fs_operation *fs_operation_create();

/**
 * @brief Destroy file system operation instance
 * @param[in]   operation       File system operation instance
 */
void fs_operation_destroy(fs_operation *operation);

/**
 * @brief Set data to file system operation
 * @param[in]   operation       File system operation instance
 * @param[in]   file_list       List of files to process
 * @param[in]   dst_path        Path to dir to copy/move files
 * @param[in]   operation       Type of operation to perform
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_operation_set_data(fs_operation *operation,
		Eina_List *file_list,
		const char *dst_path,
		operation_type type);

/**
 * @brief Execute file system operation
 * @remark Operation will be executed asynchronously.
 * @remark On operation complete callback function will be called.
 * @param[in]   operation       File system operation instance
 * @param[in]   cb_func         Callback function to notify when operation is complete
 * @param[in]   cb_data         Callback function data
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_operation_execute(fs_operation *operation,
		fs_operation_cb_func cb_func,
		fs_operation_cb_data *cb_data);

#endif /* _FS_OPERATION_H_ */
