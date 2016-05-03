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

#include "utils/common-utils.h"
#include "utils/app-types.h"
#include "utils/logger.h"

char *common_util_strconcat(const char *string1, ...)
{
	RETVM_IF(!string1, NULL, "First parameter is NULL");

	char *curr_string = NULL;
	char *res_string = NULL;
	Eina_Strbuf *buf = eina_strbuf_new();

	RETVM_IF(!buf, NULL, "Failed to allocate memory for string buffer");

	if (eina_strbuf_append(buf, string1) == EINA_FALSE) {
		ERR("Failed to append data to string buffer");
		eina_strbuf_free(buf);
		return NULL;
	}

	va_list args;
	va_start(args, string1);
	curr_string = va_arg(args, char*);

	while (curr_string) {
		if (eina_strbuf_append(buf, curr_string) == EINA_FALSE) {
			ERR("Failed to append data to string buffer");
			eina_strbuf_free(buf);
			return NULL;
		}
		curr_string = va_arg(args, char*);
	}
	va_end(args);

	res_string = eina_strbuf_string_steal(buf);
	eina_strbuf_free(buf);

	return res_string;
}

bool common_util_is_string_empty(const char *string)
{
	RETVM_IF(!string, true, "String is NULL");
	return !strcmp(string, "");
}

int common_util_copy_selected_file_list(Eina_List *source_list, Eina_List **dest_list)
{
	RETVM_IF(!source_list, RESULT_TYPE_INVALID_ARG, "Source list is empty");
	RETVM_IF(!dest_list, RESULT_TYPE_INVALID_ARG, "Destination list pointer is NULL");

	Eina_List *list = NULL;
	node_info *data = NULL;
	EINA_LIST_FOREACH(source_list, list, data) {
		if (data && data->is_selected) {
			node_info *pNode = calloc(1, sizeof(node_info));
			if (pNode) {
				pNode->name = strdup(data->name);
				pNode->parent_path = strdup(data->parent_path);
				pNode->type = data->type;
				pNode->is_selected = data->is_selected;
			} else {
				ERR("Failed to allocate memory");
				return RESULT_TYPE_FAIL_ALLOCATE_MEMORY;
			}
			*dest_list = eina_list_append(*dest_list, pNode);
		}
	}
	return RESULT_TYPE_OK;
}

void common_util_clear_file_list(Eina_List **file_list)
{
	RETM_IF(!file_list, "File list pointer is NULL");

	if (*file_list) {
		node_info *pNode = NULL;
		EINA_LIST_FREE(*file_list, pNode) {
			free(pNode->name);
			free(pNode->parent_path);
			free(pNode);
		}
		*file_list = NULL;
	}
}

void common_util_clear_storage_list(Eina_List **storage_list)
{
	RETM_IF(!storage_list, "File list pointer is NULL");

	if (*storage_list) {
		storage_info *pNode = NULL;
		EINA_LIST_FREE(*storage_list, pNode) {
			free(pNode->root_path);
			free(pNode->root_name);
			free(pNode);
		}
		*storage_list = NULL;
	}
}

const char *common_util_get_filename(const char *path)
{
	char *result = NULL;

	if (!path) {
		return NULL;
	}
	if ((result = strrchr(path, '/'))) {
		result++;
	} else {
		result = (char *)path;
	}

	return result;
}
