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

#ifndef APP_TYPES_H_
#define APP_TYPES_H_

#include <Eina.h>
#include <Evas.h>
#include <Elementary.h>

typedef enum {
	RESULT_TYPE_OK = 0,
	RESULT_TYPE_FAIL = -1,
	RESULT_TYPE_FAIL_ALLOCATE_MEMORY = -2,
	RESULT_TYPE_INVALID_ARG = -3,
	RESULT_TYPE_INVALID_PATH = -4,
	RESULT_TYPE_NOT_EXIST = -5,
	RESULT_TYPE_DUPLICATED_NAME = -6,
	RESULT_TYPE_INVALID_FILE_NAME = -7,
	RESULT_TYPE_DIR_NOT_FOUND = -8,
	RESULT_TYPE_APP_CTRL_LAUNCH_FAILED = -9,
	RESULT_TYPE_APP_CTRL_NOT_FOUND = -10,
	RESULT_TYPE_BUSY = -11,
	RESULT_TYPE_OPERATION_INTERUPTED = -12,
	RESULT_TYPE_OPERATION_INVALID_DEST = -13
} result_type;

typedef enum {
	OPERATION_TYPE_NONE,
	OPERATION_TYPE_COPY,
	OPERATION_TYPE_MOVE,
	OPERATION_TYPE_DELETE
} operation_type;

typedef enum {
	FILE_TYPE_NONE = 0,
	FILE_TYPE_DIR,              /**< Folder category */
	FILE_TYPE_FILE,             /**< File category */
	FILE_TYPE_IMAGE,            /**< Image category */
	FILE_TYPE_VIDEO,            /**< Video category */
	FILE_TYPE_MUSIC,            /**< Music category */
	FILE_TYPE_SOUND,            /**< Sound category */
	FILE_TYPE_PDF,              /**< Pdf category */
	FILE_TYPE_DOC,              /**< Word category */
	FILE_TYPE_PPT,              /**< Powerpoint category */
	FILE_TYPE_EXCEL,            /**< Excel category */
	FILE_TYPE_VOICE,            /**< Voice category */
	FILE_TYPE_HTML,             /**< Html category */
	FILE_TYPE_FLASH,            /**< Flash category */
	FILE_TYPE_GAME,             /**< Game category */
	FILE_TYPE_APP,              /**< Application category */
	FILE_TYPE_THEME,            /**< Theme category */
	FILE_TYPE_TXT,              /**< Txt category */
	FILE_TYPE_VCONTACT,         /**< Vcontact category */
	FILE_TYPE_VCALENDAR,        /**< Vcalendar category */
	FILE_TYPE_VNOTE,            /**< Vnote category */
	FILE_TYPE_VBOOKMARK,        /**< Vbookmark category */
	FILE_TYPE_VIDEO_PROJECT,    /**< Video editor project category */
	FILE_TYPE_RADIO_RECORDED,   /**< radio recorded clips category */
	FILE_TYPE_MOVIE_MAKER,      /**< Movie maker project category */
	FILE_TYPE_SVG,              /**< Svg category */
	FILE_TYPE_RSS,              /**< Rss reader file, *.opml */
	FILE_TYPE_CERTIFICATION,    /**< certification file, *.pem */
	FILE_TYPE_JAVA,             /**< java file, *.jad, *.jar */
	FILE_TYPE_WGT,              /**< wrt , *.wgt, *.wgt */
	FILE_TYPE_DRM,              /**< drm file , *.dcf */
	FILE_TYPE_ETC,              /**< Other files category */
	FILE_TYPE_MAX
} file_type;

typedef enum {
	STORAGE_TYPE_NONE = 0,
	STORAGE_TYPE_PHONE,
} storage_type;

/**
 * @brief Node info data
 */
typedef struct _node_info {
	char *parent_path;          /**< Node path */
	char *name;                 /**< Node name */
	file_type type;             /**< Node type */
	Eina_Bool is_selected;      /**< Node selected or not */
} node_info;

/**
 * @brief Storage info data
 */
typedef struct _storage_info {
	char *root_path;        /**< Storage path */
	char *root_name;        /**< Storage name */
	storage_type type;      /**< Storage type */
} storage_info;

#endif /* APP_TYPES_H_ */
