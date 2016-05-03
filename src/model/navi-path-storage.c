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

#include "model/navi-path-storage.h"
#include "utils/app-types.h"
#include "utils/logger.h"

struct _navi_path_storage {
	Eina_List *path_list;
};

navi_path_storage *navi_path_storage_create()
{
	navi_path_storage *storage = calloc(1, sizeof(navi_path_storage));
	return storage;
}

void navi_path_storage_destroy(navi_path_storage *storage)
{
	if (storage) {
		char *temp = NULL;
		EINA_LIST_FREE(storage->path_list, temp)
		{
			free(temp);
		}
		storage->path_list = NULL;
		free(storage);
	}
}

int navi_path_storage_add_folder(navi_path_storage *storage, const char *folder_name)
{
	RETVM_IF(!storage, RESULT_TYPE_INVALID_ARG, "Storage is NULL");
	RETVM_IF(!folder_name, RESULT_TYPE_INVALID_ARG, "Name is NULL");

	char *temp = strdup(folder_name);
	storage->path_list = eina_list_append(storage->path_list, temp);

	return RESULT_TYPE_OK;
}

int navi_path_storage_remove_path_to_index(navi_path_storage *storage, int index)
{
	RETVM_IF(!storage, RESULT_TYPE_INVALID_ARG, "Storage is NULL");

	int list_size = eina_list_count(storage->path_list);
	RETVM_IF(list_size < index, RESULT_TYPE_INVALID_ARG, "Requested index '%d' is to big", index);

	if (storage->path_list) {
		while (eina_list_count(storage->path_list) > index) {
			Eina_List *last_node = eina_list_last(storage->path_list);
			if (last_node) {
				char *name = eina_list_data_get(last_node);
				free(name);
			}
			storage->path_list = eina_list_remove_list(storage->path_list, last_node);
		}
		return RESULT_TYPE_OK;
	} else {
		ERR("List is empty");
		return RESULT_TYPE_FAIL;
	}
}

int navi_path_storage_get_path_list(navi_path_storage *storage, Eina_List **path_list)
{
	RETVM_IF(!storage, RESULT_TYPE_INVALID_ARG, "Storage is NULL");
	RETVM_IF(!path_list, RESULT_TYPE_INVALID_ARG, "Folder list is NULL");

	*path_list = storage->path_list;

	return RESULT_TYPE_OK;
}
