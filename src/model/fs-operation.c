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

#include "model/fs-operation.h"
#include "utils/common-utils.h"
#include "utils/model-utils.h"
#include "utils/logger.h"

#include <sys/types.h>
#include <dirent.h>
#include <Ecore.h>

#define FM_MAX_INT_DIGITS_COUNT 10
#define FM_COPY_BUF_SIZE 16384
/*
struct _fs_operation {
	Eina_List *source_list;
	char *dst_path;
	operation_type oper_type;
	fs_operation_cb_func cb_func;
	fs_operation_cb_data *cb_data;
	Ecore_Thread *exec_thread;
	Eina_Bool is_canceled;
};

typedef struct _fs_operation_copy_info {
	char *source_path;
	char *destination_path;
	file_type type;
} fs_operation_copy_info;

static int _fs_operation_copy(fs_operation *operation);
static int _fs_operation_move(fs_operation *operation);
static int _fs_operation_delete(fs_operation *operation);
static int _fs_operation_rename(fs_operation *operation, const char *source_path, const char *dest_path);
static void _fs_operation_clear_data(fs_operation *operation);
static void _fs_operation_clear_copy_list(Eina_List **copy_list);
static int _fs_operation_recursive_fill_copy_list(fs_operation *operation, Eina_List *src_list, Eina_List **copy_list,
                                                       const char *destination_path);
static int _fs_operation_proceed_copy(Eina_List *copy_list);
static bool _fs_operation_file_copy(const char *src, const char *dst);
static bool _fs_operation_file_recursive_rm(const char *dir);
static int  _fs_operation_get_items_list(Eina_List **list_to_fill, const char *source_path);
static char *_fs_operation_remove_file_extention(const char *path);
static char *_fs_operation_append_filename_if_duplicate(const char *path_to_check, const char *original_name);

static void _fs_operation_delete_exec_thread(fs_operation *operation);
static void _fs_operation_run(void *data, Ecore_Thread *thread);
static void _fs_operation_end(void *data, Ecore_Thread *thread);


fs_operation *fs_operation_create()
{
	fs_operation *operation = calloc(1, sizeof(fs_operation));
	if (operation) {
		operation->oper_type = OPERATION_TYPE_NONE;
		operation->is_canceled = EINA_FALSE;
	}
	return operation;
}

void fs_operation_destroy(fs_operation *operation)
{
	if (operation) {
		if (operation->exec_thread && !operation->is_canceled) {
			operation->is_canceled = EINA_TRUE;
			return;
		}
		_fs_operation_delete_exec_thread(operation);
		_fs_operation_clear_data(operation);
		free(operation);
	}
}

int fs_operation_set_data(fs_operation *operation,
		Eina_List *file_list,
		const char *dst_path,
		operation_type type)
{
	RETVM_IF(!operation, RESULT_TYPE_INVALID_ARG, "Operation object is NULL");
	RETVM_IF(!file_list, RESULT_TYPE_INVALID_ARG, "File list is NULL");
	RETVM_IF(!dst_path && (type != OPERATION_TYPE_DELETE), RESULT_TYPE_INVALID_ARG, "Destination path is NULL");
	RETVM_IF(type == OPERATION_TYPE_NONE, RESULT_TYPE_INVALID_ARG, "No appropriate operation type");

	int res = common_util_copy_selected_file_list(file_list, &operation->source_list);
	if (res != RESULT_TYPE_OK) {
		_fs_operation_clear_data(operation);
		ERR("Failed to copy source list");
		return res;
	}
	if (dst_path) {
		operation->dst_path = strdup(dst_path);
		if (!operation->dst_path) {
			_fs_operation_clear_data(operation);
			ERR("Failed to allocate memory for destination path");
			return RESULT_TYPE_FAIL_ALLOCATE_MEMORY;
		}
	}
	operation->oper_type = type;

	return RESULT_TYPE_OK;
}


void _fs_operation_delete_exec_thread(fs_operation *operation)
{
	if (operation->exec_thread) {
		ecore_thread_cancel(operation->exec_thread);
		operation->exec_thread = NULL;
	}
}

void _fs_operation_run(void *data, Ecore_Thread *thread)
{
	fs_operation *operation = data;
	int res = RESULT_TYPE_FAIL;

	switch (operation->oper_type) {
	case OPERATION_TYPE_COPY:
		res = _fs_operation_copy(operation);
		break;
	case OPERATION_TYPE_MOVE:
		res = _fs_operation_move(operation);
		break;
	case OPERATION_TYPE_DELETE:
		res = _fs_operation_delete(operation);
		break;
	default:
		ERR("Operation type not set");
		return;
		break;
	}

	if (operation->is_canceled) {
		return;
	}

	if (operation->cb_data) {
		operation->cb_data->result = res;
	}

}

void _fs_operation_end(void *data, Ecore_Thread *thread)
{
	fs_operation *operation = data;

	_fs_operation_delete_exec_thread(operation);

	if (operation->is_canceled) {
		fs_operation_destroy(operation);
		return;
	}

	if (operation->cb_func) {
		operation->cb_func(operation->cb_data);
	}
}

int fs_operation_execute(fs_operation *operation,
		fs_operation_cb_func cb_func,
		fs_operation_cb_data *cb_data)
{
	RETVM_IF(!operation, RESULT_TYPE_INVALID_ARG, "Operation object is NULL");
	RETVM_IF(!operation->source_list, RESULT_TYPE_FAIL, "File list not set");
	RETVM_IF(operation->oper_type == OPERATION_TYPE_NONE, RESULT_TYPE_FAIL, "Type of operation not set");
	RETVM_IF(!operation->dst_path && (operation->oper_type != OPERATION_TYPE_DELETE),
			RESULT_TYPE_FAIL, "Destination path not set");

	operation->cb_func = cb_func;
	operation->cb_data = cb_data;

	operation->exec_thread = ecore_thread_feedback_run(_fs_operation_run,
			NULL,
			_fs_operation_end,
			NULL,
			(void *)operation,
			EINA_TRUE);

	RETVM_IF(!operation->exec_thread, RESULT_TYPE_FAIL, "Failed to create thread");

	return RESULT_TYPE_OK;
}

static void _fs_operation_clear_data(fs_operation *operation)
{
	common_util_clear_file_list(&operation->source_list);
	operation->oper_type = OPERATION_TYPE_NONE;
	operation->cb_func = NULL;
	operation->cb_data = NULL;
	free(operation->dst_path);
	operation->dst_path = NULL;
}

static bool _fs_operation_file_copy(const char *src, const char *dst)
{
	FILE *source_file = NULL;
	FILE *destintation_file = NULL;
	char buf[FM_COPY_BUF_SIZE] = {0};
	char src_realpath[PATH_MAX] = {'\0'};
	char dst_realpath[PATH_MAX] = {'\0'};
	size_t num = 0;

	if (!realpath(src, src_realpath)) {
		return false;
	}
	if (realpath(dst, dst_realpath) && !strcmp(src_realpath, dst_realpath)) {
		return false;
	}

	source_file = fopen(src, "rb");
	if (!source_file) {
		return false;
	}

	destintation_file = fopen(dst, "wb");
	if (!destintation_file) {
		fclose(source_file);
		return false;
	}

	while ((num = fread(buf, 1, sizeof(buf), source_file)) > 0) {
		if (fwrite(buf, 1, num, destintation_file) != num) {
			fclose(source_file);
			fclose(destintation_file);
			return false;
		 }
	 }

	fclose(source_file);
	fclose(destintation_file);

	return true;
}

static bool _fs_operation_file_recursive_rm(const char *dir)
{
	DIR *dir_ptr = NULL;
	struct dirent *dp = NULL;
	char path[PATH_MAX] = {'\0'};
	struct stat st;

	if (stat(dir, &st) == -1) {
		return false;
	}

	if (S_ISDIR(st.st_mode)) {
		dir_ptr = opendir(dir);
		if (dir_ptr) {
			while ((dp = readdir(dir_ptr))) {
				if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, ".."))) {
					snprintf(path, PATH_MAX, "%s/%s", dir, dp->d_name);
					if (!_fs_operation_file_recursive_rm(path)) {
						closedir(dir_ptr);
						return false;
					}
				}
			}
			closedir(dir_ptr);
		}
	}

	if (remove(dir) < 0) {
		return false;
	}

	return true;
}

static void _fs_operation_clear_copy_list(Eina_List **copy_list)
{
	RETM_IF(!copy_list, "Copy list pointer is NULL");

	if (*copy_list) {
		fs_operation_copy_info *pNode = NULL;
		EINA_LIST_FREE(*copy_list, pNode)
		{
			free(pNode->source_path);
			free(pNode->destination_path);
			free(pNode);
		}
		*copy_list = NULL;
	}
}

static char *_fs_operation_remove_file_extention(const char *path)
{
	int len = strlen(path);
	char *striped_path = calloc(1, (len + 1));
	char *extention = strrchr(path, '.');

	if (!extention) {
		strncpy(striped_path, path, (len + 1));
		return striped_path;
	}
	strncpy(striped_path, path, (extention - path));
	striped_path[extention - path] = '\0';

	return striped_path;
	}

static char *_fs_operation_append_filename_if_duplicate(const char *path_to_check, const char *striped_path)
{
	if (model_utils_is_file_exists(path_to_check)) {
		int copy_index = 1;
		char *new_path = NULL;
		char index_buffer[FM_MAX_INT_DIGITS_COUNT] = { 0 };
		char *extention = NULL;
		Eina_Bool is_dir = model_utils_file_is_dir(path_to_check);
		if (!is_dir) {
			extention = strrchr(path_to_check, '.');
		}

		while (true) {
			sprintf(index_buffer, "%d", copy_index);
			new_path = common_util_strconcat(striped_path, "(", index_buffer, ")", extention, NULL);
			if (model_utils_is_file_exists(new_path)) {
				free(new_path);
				copy_index++;
			} else {
				return new_path;
			}
		}
	}

	return strdup(path_to_check);
}

static int _fs_operation_get_items_list(Eina_List **list_to_fill, const char *source_path)
{
	Eina_List *dir_list = NULL;
	Eina_List *file_list = NULL;

	int res = model_utils_read_dir(source_path, &dir_list, &file_list);
	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to get items list");

	*list_to_fill = eina_list_merge(dir_list, file_list);

	return RESULT_TYPE_OK;
}

static int _fs_operation_recursive_fill_copy_list(fs_operation *operation, Eina_List *src_list,
		Eina_List **copy_list,
		const char *destination_path)
{
	Eina_List *list = NULL;
	node_info *data = NULL;

	EINA_LIST_FOREACH(src_list, list, data) {
		if (operation->is_canceled) {
			return RESULT_TYPE_OPERATION_INTERUPTED;
		}

		char *striped_path = NULL;
		char *source_path = common_util_strconcat(data->parent_path, "/", data->name, NULL);
		char *dest_path = common_util_strconcat(destination_path, "/", data->name, NULL);

		if (strcmp(source_path, destination_path) == 0) {
			free(source_path);
			free(dest_path);
			return RESULT_TYPE_OPERATION_INVALID_DEST;
		}

		if (model_utils_file_is_dir(dest_path)) {
			striped_path = strdup(dest_path);
		} else {
			striped_path = _fs_operation_remove_file_extention(dest_path);
		}

		char *checked_path = _fs_operation_append_filename_if_duplicate(dest_path, striped_path);

		int res = RESULT_TYPE_OK;

		fs_operation_copy_info *copy_item = calloc(1, sizeof(fs_operation_copy_info));
		copy_item->source_path = source_path;
		copy_item->destination_path = checked_path;
		copy_item->type = data->type;

		if (data->type == FILE_TYPE_DIR) {
			Eina_List *new_src_list = NULL;
			res = _fs_operation_get_items_list(&new_src_list, source_path);

			if (res == RESULT_TYPE_OK) {
				res = _fs_operation_recursive_fill_copy_list(operation, new_src_list, copy_list, checked_path);
				common_util_clear_file_list(&new_src_list);
			}
		}

		*copy_list = eina_list_append(*copy_list, copy_item);

		free(dest_path);
		free(striped_path);

		RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to copy items");
	}

	return RESULT_TYPE_OK;
}

static int _fs_operation_proceed_copy(Eina_List *copy_list)
{
	Eina_List *list = NULL;
	fs_operation_copy_info *data = NULL;
	int res = RESULT_TYPE_OK;

	EINA_LIST_REVERSE_FOREACH(copy_list, list, data) {
		if (data->type == FILE_TYPE_DIR) {
			(void)mkdir(data->destination_path, DIR_MODE);
		} else if (!_fs_operation_file_copy(data->source_path, data->destination_path)) {
			res = RESULT_TYPE_FAIL;
		}
	}
	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to proceed copy list");

	return RESULT_TYPE_OK;
}

static int _fs_operation_copy(fs_operation *operation)
{
	Eina_List *copy_list = NULL;

	int res = _fs_operation_recursive_fill_copy_list(operation, operation->source_list, &copy_list, operation->dst_path);

	if (res == RESULT_TYPE_OK) {
		res = _fs_operation_proceed_copy(copy_list);
	}

	_fs_operation_clear_copy_list(&copy_list);

	RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to copy source");

	return RESULT_TYPE_OK;
}


static int _fs_operation_rename(fs_operation *operation, const char *source_path, const char *dest_path)
{
	int res = RESULT_TYPE_FAIL;
	struct stat st;
	if (stat(source_path, &st) != 0) {
		ERR("Failed to get stat for folder %s. ERRNO = %d", source_path, errno);
		return res;
	}

	if (S_ISREG(st.st_mode)) {
		if (!_fs_operation_file_copy(source_path, dest_path)) {
			ERR("Failed to copy file %s", source_path);
			return res;
		}

		chmod(dest_path, st.st_mode);
		int res_remove = remove(source_path);
		RETVM_IF(res_remove < 0, RESULT_TYPE_FAIL, "Failed to remove file %s", source_path);
		res = RESULT_TYPE_OK;
	} else {
		Eina_List *copy_list = NULL;
		Eina_List *src_list = NULL;

		res = _fs_operation_get_items_list(&src_list, source_path);
		RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to get item list for dis %s", source_path);

		res = _fs_operation_recursive_fill_copy_list(operation, src_list, &copy_list, dest_path);
		common_util_clear_file_list(&src_list);
		RETVM_IF(res != RESULT_TYPE_OK, res, "Failed to fill copy list recursivly");

		fs_operation_copy_info *copy_item = calloc(1, sizeof(fs_operation_copy_info));
		copy_item->source_path = strdup(source_path);
		copy_item->destination_path = strdup(dest_path);
		copy_item->type = FILE_TYPE_DIR;
		copy_list = eina_list_append(copy_list, copy_item);

		res = _fs_operation_proceed_copy(copy_list);
		_fs_operation_clear_copy_list(&copy_list);
		RETVM_IF(res != RESULT_TYPE_OK, res, "Failed perform operation copy");

		if (!_fs_operation_file_recursive_rm(source_path)) {
			ERR("Failed to delete dir %s recursively", source_path);
			return RESULT_TYPE_FAIL;
		}
	}

	return res;
}

static int _fs_operation_move(fs_operation *operation)
{
	Eina_List *list = NULL;
	node_info *data = NULL;
	int result = RESULT_TYPE_OK;

	EINA_LIST_FOREACH(operation->source_list, list, data) {
		if (operation->is_canceled) {
			return RESULT_TYPE_OPERATION_INTERUPTED;
		}

		char *striped_path = NULL;
		char *source_path = common_util_strconcat(data->parent_path, "/", data->name, NULL);
		char *dest_path = common_util_strconcat(operation->dst_path, "/", data->name, NULL);

		if (strcmp(source_path, dest_path) == 0) {
			free(source_path);
			free(dest_path);
			return RESULT_TYPE_OPERATION_INVALID_DEST;
		}

		if (model_utils_file_is_dir(dest_path)) {
			striped_path = strdup(dest_path);
		} else {
			striped_path = _fs_operation_remove_file_extention(dest_path);
		}

		char *checked_path = _fs_operation_append_filename_if_duplicate(dest_path, striped_path);

		if (rename(source_path, checked_path)) {
			if (errno == EINVAL) {
				result =  RESULT_TYPE_OPERATION_INVALID_DEST;
			} else if (errno == EXDEV) {
				result = _fs_operation_rename(operation, source_path, checked_path);
			}
		}

		free(dest_path);
		free(striped_path);
		free(source_path);
		free(checked_path);

		RETVM_IF(result != RESULT_TYPE_OK, result, "Failed to move source ");
	}

	return RESULT_TYPE_OK;
}

static int _fs_operation_delete(fs_operation *operation)
{
	Eina_List *list = NULL;
	node_info *data = NULL;

	int result = RESULT_TYPE_OK;
	EINA_LIST_FOREACH(operation->source_list, list, data) {
		if (operation->is_canceled) {
			result = RESULT_TYPE_OPERATION_INTERUPTED;
			break;
		}

		char *temp_name = common_util_strconcat(data->parent_path, "/", data->name, NULL);

		if (data->type == FILE_TYPE_DIR && model_utils_file_is_dir(temp_name)) {
			if (!_fs_operation_file_recursive_rm(temp_name)) {
				ERR("Failed to delete dir %s recursively", temp_name);
			}
		} else if (model_utils_is_file_exists(temp_name)) {
			if (remove(temp_name) < 0) {
				ERR("Failed to delete file %s", temp_name);
			}
		} else {
			ERR("%s %s doesn't exist", (data->type == FILE_TYPE_DIR) ? "Dir" : "File", temp_name);
		}
		free(temp_name);
	}

	return result;
}
*/
