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

#include "model/clipboard.h"
#include "utils/common-utils.h"
#include "utils/logger.h"

struct _clipboard {
	Eina_List *selected_files;
	operation_type oper_type;
};

clipboard *clipboard_create()
{
	clipboard *clipboard_obj = calloc(1, sizeof(clipboard));
	return clipboard_obj;
}

void clipboard_destroy(clipboard *clipboard_obj)
{
	clipboard_clear(clipboard_obj);
	free(clipboard_obj);
}

int clipboard_add_data(clipboard *clipboard_obj, Eina_List *file_list)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG, "Clipboard object is NULL");
	RETVM_IF(!file_list, RESULT_TYPE_INVALID_ARG, "File list is NULL");

	int res = RESULT_TYPE_OK;

	clipboard_clear(clipboard_obj);

	res = common_util_copy_selected_file_list(file_list, &clipboard_obj->selected_files);
	if (res != RESULT_TYPE_OK) {
		clipboard_clear(clipboard_obj);
		ERR("Failed to copy source list");
	}
	return res;
}

int clipboard_get_data(clipboard *clipboard_obj, Eina_List **file_list)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG, "Clipboard object is NULL");
	*file_list = clipboard_obj->selected_files;

	return RESULT_TYPE_OK;
}

int clipboard_set_operation(clipboard *clipboard_obj, operation_type type)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG, "Clipboard object is NULL");
	clipboard_obj->oper_type = type;

	return RESULT_TYPE_OK;
}

int clipboard_get_operation(clipboard *clipboard_obj, operation_type *type)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG , "Clipboard object is NULL");
	*type = clipboard_obj->oper_type;

	return RESULT_TYPE_OK;
}

int clipboard_is_empty(clipboard *clipboard_obj, Eina_Bool *empty)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG, "Clipboard object is NULL");
	*empty = (clipboard_obj->selected_files ? EINA_FALSE : EINA_TRUE);

	return RESULT_TYPE_OK;
}

int clipboard_clear(clipboard *clipboard_obj)
{
	RETVM_IF(!clipboard_obj, RESULT_TYPE_INVALID_ARG, "Clipboard object is NULL");

	common_util_clear_file_list(&clipboard_obj->selected_files);
	clipboard_obj->oper_type = OPERATION_TYPE_NONE;

	return RESULT_TYPE_OK;
}
