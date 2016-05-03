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

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#include <Eina.h>
#include <stdbool.h>

/**
 * @brief Appends input strings into new one
 * @remark result string is newly allocated string, so it must be free after use
 * @remark last input parameter of function must be NULL
 * @param[in]   string1,... Input strings for concatenation
 * @return newly concatenated sting
 */
char *common_util_strconcat(const char *string1, ...);

/**
 * @brief Check string on empty state
 * @param[in] string String to check
 * @return true is string is empty, false otherwise
 */
bool common_util_is_string_empty(const char *string);

/**
 * @brief Copy file list from one to another (for EINA_LISTs that data is "node_info" type)
 * @param[in]       source_list Source file list
 * @param[in/out]   dest_list   Destination file list
 * @return Error code.  RESULT_TYPE_OK if operation success.
 */
int common_util_copy_selected_file_list(Eina_List *source_list, Eina_List **dest_list);

/**
 * @brief Clear file list (for EINA_LISTs that data is "node_info" type)
 * @param[in]       file_list   File list to clear
 */
void common_util_clear_file_list(Eina_List **file_list);

/**
 * @brief Clear storage list (for EINA_LISTs that data is "storage_info" type)
 * @param[in]       file_list   File list to clear
 */
void common_util_clear_storage_list(Eina_List **storage_list);

/**
 * @brief Gets the filename from a give path
 * @param[in] The complete path
 * @return Only the file name
 */
const char *common_util_get_filename(const char *path);

#endif /* COMMON_UTILS_H_ */
