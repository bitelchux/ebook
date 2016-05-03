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

#ifndef MODEL_UTILS_H_
#define MODEL_UTILS_H_

#include "utils/app-types.h"

#define DIR_MODE S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

/**
 * @brief Check wheather filepath is right
 * @param[in]   fullpath    Full path of checked file/folder
 * @param[in]   is_dir      Flag if checked file path is dir
 * @return Error code.  RESULT_TYPE_OK if file path is correct
 *                      RESULT_TYPE_INVALID_PATH if file path is invalid
 */
int model_utils_is_path_valid(const char *fullpath, Eina_Bool is_dir);

/**
 * @brief Check if file exists
 * @param[in]   filepath    Path to file
 * @return      EINA_TRUE if file exists
 *              EINA_FALSE if file does not exist
 */
Eina_Bool model_utils_is_file_exists(const char *filepath);

/**
 * @brief Check if file is a directory
 * @param[in]   filepath    Path to file
 * @return      EINA_TRUE if file exist and is a directory
 *              EINA_FALSE if file does not exist
 */
Eina_Bool model_utils_file_is_dir(const char *filepath);

/**
 * @brief Get the directory where file reside
 * @param[in]   filepath    Path to file
 * @return The directory name
 */
char* model_utils_get_dir_name(const char *filepath);

/**
 * @brief Get file category by file path
 * @param[in]   filepath    Path to file
 * @param[out]  category    Type of file category
 * @return Error code.  RESULT_TYPE_OK if file path is correct
 *                      RESULT_TYPE_INVALID_PATH if file path is invalid
 *                      RESULT_TYPE_FAIL if failed to get file type
 */
int model_utils_get_file_category(const char *filepath, file_type *category);

/**
 * @brief Get icon resource name for file type
 * @param[in]   ftype   File type
 * @return resource name of the icon
 */
const char *model_utils_get_default_icon_name(file_type ftype);

/**
 * @brief Get type of storage
 * @param[in]   fullpath    Full path to file/dir
 * @return type of storage where file/dir is located
 */
storage_type model_utils_get_storage_type(const char *fullpath);

/**
 * @brief Get "public" path to file/dir (hide real device file path)
 * @param[in]   original_path   Real full path to file/dir
 * @return public file path to file/dir
 */
char *model_utils_get_public_file_path(const char *original_path);

/**
 * @brief Check if path is a root path of storage
 * @param[in]   fullpath    Path to check
 * @return Storage type. Result will be STORAGE_TYPE_NONE if checked path is not a root
 */
storage_type model_utils_is_root_path(const char *fullpath);

/**
 * @brief Launch service to view file content
 * @param[in]   file_info   File info
 * @return Error code.  RESULT_TYPE_OK if operation success
 */
int model_utils_launch_file(const node_info *file_info);

/**
 * @brief Reads directory and fills two lists with node_info structures
 * @param[in]       dir_path   Path to the directory to read
 * @param[in/out]   dir_list   List to append node_info of the sub directories
 * @param[in/out]   file_list  List to append node_info of files
 * @return Error code.  RESULT_TYPE_OK if operation success
 */
int model_utils_read_dir(const char *dir_path, Eina_List **dir_list, Eina_List **file_list);

#endif /* MODEL_UTILS_H_ */
