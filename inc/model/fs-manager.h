/*
 * Copyright 2014 -2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _FS_MANAGER_H_
#define _FS_MANAGER_H_

#include <Eina.h>

/* Forward declaration: */

typedef struct _fs_manager fs_manager;

/**
 * @brief Prototype of operation complete user callback function
 */
typedef void (*fs_manager_complete_cb_func)(void *cb_data, int result);

/**
 * @brief Create file system manager instance
 * @return File system manager instance on success, otherwise NULL
 */
fs_manager *fs_manager_create();

/**
 * @brief Destroy file system manager instance
 * @param[in]   manager      File system manager instance
 */
void fs_manager_destroy(fs_manager *manager);

/**
 * @brief Gets list of available storages
 * @param[in]   manager         File system manager instance
 * @param[out]  storage_list    Storage list to be set
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_get_storage_list(fs_manager *manager, Eina_List **storage_list);

/**
 * @brief Gets from directory lists of its files and sub directories
 * @param[in]   manager     File system manager instance
 * @param[in]   dir_path    Directory fullpath
 * @param[out]  file_list   List of directory sub directories and files
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_get_file_list(fs_manager *manager,
        const char *dir_path,
        Eina_List **file_list);

/**
 * @brief Copy selected files to set destination
 * @param[in]   manager         File system manager instance
 * @param[in]   selected_list   Selected files list to copy
 * @param[in]   dest_path       Destination path
 * @param[in]   cb_func         Operation complete user callback function
 * @param[in]   cb_data         User callback data
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_copy_files(fs_manager *manager,
		Eina_List *selected_list,
		const char *dest_path,
		fs_manager_complete_cb_func cb_func,
		void *cb_data);

/**
 * @brief Move selected files to set destination
 * @param[in]   manager         File system manager instance
 * @param[in]   selected_list   Selected files list to move
 * @param[in]   dest_path       Destination path
 * @param[in]   cb_func         Operation complete user callback function
 * @param[in]   cb_data         User callback data
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_move_files(fs_manager *manager,
		Eina_List *selected_list,
		const char *dest_path,
		fs_manager_complete_cb_func cb_func,
		void *cb_data);

/**
 * @brief Delete selected files
 * @param[in]   manager         File system manager instance
 * @param[in]   selected_list   Selected files list to delete
 * @param[in]   cb_func         Operation complete user callback function
 * @param[in]   cb_data         User callback data
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_delete_files(fs_manager *manager,
		Eina_List *selected_list,
		fs_manager_complete_cb_func cb_func,
		void *cb_data);

/**
 * @brief Creates new folder by set folder full path
 * @param[in]   manager     File system manager instance
 * @param[in]   dir         New directory full path
 * @return Error code. RESULT_TYPE_OK if operation success.
 */
int fs_manager_create_folder(fs_manager *manager, const char *dir);

#endif /* _FS_MANAGER_H_ */
