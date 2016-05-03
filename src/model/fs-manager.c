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

#include "model/fs-manager.h"
#include "model/fs-operation.h"
#include "utils/config.h"
#include "utils/model-utils.h"
#include "utils/logger.h"

#include <storage.h>
#include <stdbool.h>

enum {
    FM_CHECK_ARG_VALID = 0x0001,
    FM_CHECK_EXIST = 0x0002,
    FM_CHECK_PATH_VALID = 0x0004,
    FM_CHECK_PARENT_DIR_EXIST = 0x0008,
    FM_CHECK_DUPLICATED = 0x0010
};

#define FM_COMP_RES_D1_LESS_D2    -1
#define FM_COMP_RES_D1_GREATER_D2 1
#define FM_COMP_RES_D1_EQUAL_D2   0

struct _fs_manager {
	Eina_Bool is_locked;
	fs_operation *operation;
	fs_manager_complete_cb_func user_cb_func;
	void *user_cb_data;
};

//static void _fs_manager_clear_data(fs_manager *manager);
static Eina_Bool _fs_manager_check_path_valid(const char *path, int *ret);
static Eina_Bool _fs_manager_check_parent_exists(const char *path, int *ret);
static int _fs_manager_check_error(const char *fullpath, int check_option);
static int _fs_manager_sort_by_name_cb(const void *d1, const void *d2);
//static int _fs_manager_generate_operation(fs_manager *manager,
//					Eina_List *source_list,
//					const char *dest_path,
//					operation_type oper_type,
//					fs_manager_complete_cb_func func,
//					void *data);

//static void _on_operation_completed(void *data);

fs_manager *fs_manager_create()
{
	fs_manager *manager = calloc(1, sizeof(fs_manager));
	if (manager) {
		manager->is_locked = EINA_FALSE;
	}

	return manager;
}

void fs_manager_destroy(fs_manager *manager)
{
	if (manager) {
	//	_fs_manager_clear_data(manager);
		free(manager);
	}
}

int fs_manager_get_storage_list(fs_manager *manager, Eina_List **storage_list)
{

	RETVM_IF(!manager, RESULT_TYPE_INVALID_ARG, "File manager is NULL");
	if (manager->is_locked) {
		ERR("File manager is busy");
		return RESULT_TYPE_BUSY;
	}

	storage_info *const pNode_device = calloc(1, sizeof(node_info));
	pNode_device->root_name = strdup(FM_PHONE_LABEL);
	pNode_device->root_path = strdup(FM_PHONE_FOLDER);
	pNode_device->type = STORAGE_TYPE_PHONE;

	*storage_list = eina_list_append(*storage_list, pNode_device);

	*storage_list = eina_list_sort(*storage_list, eina_list_count(*storage_list), _fs_manager_sort_by_name_cb);

	return RESULT_TYPE_OK;
}

int fs_manager_get_file_list(fs_manager *manager, const char *dir_path, Eina_List **file_list)
{

	RETVM_IF(!manager, RESULT_TYPE_INVALID_ARG, "File manager is NULL");
	RETVM_IF(!dir_path, RESULT_TYPE_INVALID_ARG, "Path is NULL");
	RETVM_IF(!file_list, RESULT_TYPE_INVALID_ARG, "File list is NULL");

	if (manager->is_locked) {
		ERR("File manager is busy");
		return RESULT_TYPE_BUSY;
	}

	int option = FM_CHECK_EXIST;
	int ret = _fs_manager_check_error(dir_path, option);
	if (ret != RESULT_TYPE_OK) {
		return ret;
	}

	Eina_List *dirs = NULL;
	Eina_List *files = NULL;
	ret = model_utils_read_dir(dir_path, &dirs, &files);
	if (ret != RESULT_TYPE_OK) {
		ERR("Failed to read dir '%s'", dir_path);
		return ret;
	}

	dirs = eina_list_sort(dirs, eina_list_count(dirs), _fs_manager_sort_by_name_cb);

	files = eina_list_sort(files, eina_list_count(files), _fs_manager_sort_by_name_cb);

	*file_list = eina_list_merge(dirs, files);

	return RESULT_TYPE_OK;
}
/*
int fs_manager_copy_files(fs_manager *manager,
		Eina_List *source_list,
		const char *dest_path,
		fs_manager_complete_cb_func cb_func,
		void *cb_data)
{
	return _fs_manager_generate_operation(manager,
			source_list,
			dest_path,
			OPERATION_TYPE_COPY,
			cb_func,
			cb_data);
}

int fs_manager_move_files(fs_manager *manager,
		Eina_List *source_list,
		const char *dest_path,
		fs_manager_complete_cb_func cb_func,
		void *cb_data)
{
	return _fs_manager_generate_operation(manager,
			source_list,
			dest_path,
			OPERATION_TYPE_MOVE,
			cb_func,
			cb_data);
}

int fs_manager_delete_files(fs_manager *manager,
		Eina_List *source_list,
		fs_manager_complete_cb_func cb_func,
		void *cb_data)
{
	return _fs_manager_generate_operation(manager,
			source_list,
			NULL,
			OPERATION_TYPE_DELETE,
			cb_func,
			cb_data);
}

int fs_manager_create_folder(fs_manager *manager, const char *dir)
{
	RETVM_IF(!manager, RESULT_TYPE_INVALID_ARG, "File manager is NULL");
	RETVM_IF(!dir, RESULT_TYPE_INVALID_ARG, "Directory path is NULL");

	if (manager->is_locked) {
		ERR("File manager is busy");
		return RESULT_TYPE_BUSY;
	}

	int option = FM_CHECK_DUPLICATED | FM_CHECK_PATH_VALID;
	int ret = _fs_manager_check_error(dir, option);
	if (ret != RESULT_TYPE_OK) {
		return ret;
	}

	if (mkdir(dir, DIR_MODE) < 0) {
		ERR("Failed to create folder '%s'", dir);
		return RESULT_TYPE_FAIL;
	}
	return RESULT_TYPE_OK;
}
*/
static Eina_Bool _fs_manager_check_path_valid(const char *path, int *ret)
{
	*ret = model_utils_is_path_valid(path, model_utils_is_file_exists(path));
	return (*ret == RESULT_TYPE_OK);
}

static Eina_Bool _fs_manager_check_parent_exists(const char *path, int *ret)
{
	*ret = RESULT_TYPE_FAIL;
	char *const parent_path = model_utils_get_dir_name(path);
	if (parent_path) {
		*ret = model_utils_is_file_exists(parent_path) ?
				RESULT_TYPE_OK :
				RESULT_TYPE_DIR_NOT_FOUND;
		free(parent_path);
	} else {
		*ret = RESULT_TYPE_FAIL_ALLOCATE_MEMORY;
	}
	return (*ret == RESULT_TYPE_OK);
}

static int _fs_manager_sort_by_name_cb(const void *d1, const void *d2)
{
	node_info *txt1 = (node_info *)d1;
	node_info *txt2 = (node_info *)d2;
	char *name1 = NULL;
	char *name2 = NULL;
	int comp_res = FM_COMP_RES_D1_EQUAL_D2;

	if (!txt1 || !txt1->name) {
		return FM_COMP_RES_D1_GREATER_D2;
	}
	if (!txt2 || !txt2->name) {
		return FM_COMP_RES_D1_LESS_D2;
	}
	name1 = strdup(txt1->name);
	if (!name1) {
		return FM_COMP_RES_D1_LESS_D2;
	}
	eina_str_tolower(&name1);

	name2 = strdup(txt2->name);
	if (!name2) {
		free(name1);
		name1 = NULL;
		return FM_COMP_RES_D1_LESS_D2;
	}
	eina_str_tolower(&name2);

	comp_res = strcmp(name1, name2);

	free(name1);
	free(name2);
	return comp_res;
}

static int _fs_manager_check_error(const char *fullpath, int check_option)
{
	int ret = RESULT_TYPE_OK;
	int ret2 = RESULT_TYPE_OK;

	if ((check_option & FM_CHECK_ARG_VALID) && !fullpath) {
		ERR("Input argument is NULL");
		ret = RESULT_TYPE_INVALID_ARG;
	} else if ((check_option & FM_CHECK_EXIST) && (!model_utils_is_file_exists(fullpath))) {
		ERR("'%s' doesn't exist", fullpath);
		ret = RESULT_TYPE_NOT_EXIST;
	} else if ((check_option & FM_CHECK_PATH_VALID) && (!_fs_manager_check_path_valid(fullpath, &ret2))) {
		ERR("Path '%s' is invalid", fullpath);
		ret = ret2;
	} else if ((check_option & FM_CHECK_PARENT_DIR_EXIST) && (!_fs_manager_check_parent_exists(fullpath, &ret2))) {
		ERR("Parent directory for '%s' doesn't exist", fullpath);
		ret = ret2;
	} else if ((check_option & FM_CHECK_DUPLICATED) && (model_utils_is_file_exists(fullpath))) {
		ERR("Duplicated name. '%s' already exists", fullpath);
		ret = RESULT_TYPE_DUPLICATED_NAME;
	}
	return ret;
}
/*
static int _fs_manager_generate_operation(fs_manager *manager,
		Eina_List *source_list,
		const char *dest_path,
		operation_type oper_type,
		fs_manager_complete_cb_func func,
		void *data)
{
	RETVM_IF(!manager, RESULT_TYPE_INVALID_ARG, "File manager is NULL");
	RETVM_IF(!source_list, RESULT_TYPE_INVALID_ARG, "Source list is NULL");
	RETVM_IF(oper_type == OPERATION_TYPE_NONE, RESULT_TYPE_INVALID_ARG, "No appropriate operation type");
	RETVM_IF(!dest_path && (oper_type != OPERATION_TYPE_DELETE), RESULT_TYPE_INVALID_ARG, "Destination path is NULL");

	if (manager->is_locked) {
		ERR("File manager is busy");
		return RESULT_TYPE_BUSY;
	}

	manager->user_cb_func = func;
	manager->user_cb_data = data;
/*
	manager->operation = fs_operation_create();
	if (!manager->operation) {
		_fs_manager_clear_data(manager);
		ERR("Failed to allocate memory for file operation");
		return RESULT_TYPE_FAIL_ALLOCATE_MEMORY;
	}

	int result = fs_operation_set_data(manager->operation, source_list, dest_path, oper_type);
	if (result != RESULT_TYPE_OK) {
		_fs_manager_clear_data(manager);
		ERR("Failed to set operation data");
		return result;
	}

	fs_operation_cb_data *cb_data = calloc(1, sizeof(fs_operation_cb_data));
	if (!cb_data) {
		_fs_manager_clear_data(manager);
		ERR("Failed to allocate memory for callback operation data");
		return RESULT_TYPE_FAIL_ALLOCATE_MEMORY;
	}
*/
//	cb_data->manager = manager;
//	cb_data->result = RESULT_TYPE_FAIL;

	/* Lock file system manager */
//	manager->is_locked = EINA_TRUE;
/*
	result = fs_operation_execute(manager->operation, _on_operation_completed, cb_data);
	if (result != RESULT_TYPE_OK) {
		free(cb_data);
		_fs_manager_clear_data(manager);
		manager->is_locked = EINA_FALSE;
		ERR("Failed to execute operation");
	}
*/
//	return result;
//}
/*
static void _on_operation_completed(void *data)
{
	RETM_IF(!data, "Callback data is NULL");
/*
	fs_operation_cb_data *operation_data = data;
	fs_manager *manager = operation_data->manager;

	RETM_IF(!manager, "File manager in callback data is NULL");

	fs_operation_destroy(manager->operation);
	manager->operation = NULL;
*/
	/* Unlock file system manager */
//	manager->is_locked = EINA_FALSE;

	/* User callback calling */
/*	if (manager->user_cb_func) {
		manager->user_cb_func(manager->user_cb_data, operation_data->result);
		manager->user_cb_func = NULL;
		manager->user_cb_data = NULL;
	}

	free(operation_data);
}
*/
/*
static void _fs_manager_clear_data(fs_manager *manager)
{
	manager->user_cb_func = NULL;
	manager->user_cb_data = NULL;
	fs_operation_destroy(manager->operation);
	manager->operation = NULL;
}
*/
