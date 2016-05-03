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

#include "utils/model-utils.h"
#include "utils/config.h"
#include "utils/common-utils.h"
#include "utils/logger.h"

#include <app.h>
#include <ctype.h>
#include <regex.h>
#include <dirent.h>

#define FM_NAME_PATTERN "[\\:*\"&<>'/]"

typedef struct _file_type_by_ext {
	const char *ext;
	file_type ftype;
} file_type_by_ext;

static file_type_by_ext FILE_TYPE_BY_EXT[] = {
	{"3G2",   FILE_TYPE_VIDEO},
	{"3GP",   FILE_TYPE_VIDEO},
	{"3GPP",  FILE_TYPE_VIDEO},
	{"AAC",   FILE_TYPE_MUSIC},
	{"AMR",   FILE_TYPE_VOICE},
	{"ASF",   FILE_TYPE_VIDEO},
	{"AVI",   FILE_TYPE_VIDEO},
	{"AWB",   FILE_TYPE_VOICE},
	{"BMP",   FILE_TYPE_IMAGE},
	{"DCF",   FILE_TYPE_DRM},
	{"DIVX",  FILE_TYPE_VIDEO},
	{"DOC",   FILE_TYPE_DOC},
	{"DOCX",  FILE_TYPE_DOC},
	{"G72",   FILE_TYPE_MUSIC},
	{"GIF",   FILE_TYPE_IMAGE},
	{"H263",  FILE_TYPE_VIDEO},
	{"HTM",   FILE_TYPE_HTML},
	{"HTML",  FILE_TYPE_HTML},
	{"IMY",   FILE_TYPE_SOUND},
	{"IPK",   FILE_TYPE_APP},
	{"JAD",   FILE_TYPE_JAVA},
	{"JAR",   FILE_TYPE_JAVA},
	{"JPE",   FILE_TYPE_IMAGE},
	{"JPEG",  FILE_TYPE_IMAGE},
	{"JPG",   FILE_TYPE_IMAGE},
	{"M3G",   FILE_TYPE_FLASH},
	{"M4A",   FILE_TYPE_MUSIC},
	{"MID",   FILE_TYPE_SOUND},
	{"MIDI",  FILE_TYPE_SOUND},
	{"MKA",   FILE_TYPE_MUSIC},
	{"MKV",   FILE_TYPE_VIDEO},
	{"MMF",   FILE_TYPE_SOUND},
	{"MP3",   FILE_TYPE_MUSIC},
	{"MP4",   FILE_TYPE_VIDEO},
	{"MPEG",  FILE_TYPE_VIDEO},
	{"MPG",   FILE_TYPE_VIDEO},
	{"MXMF",  FILE_TYPE_SOUND},
	{"OPML",  FILE_TYPE_RSS},
	{"PDF",   FILE_TYPE_PDF},
	{"PEM",   FILE_TYPE_CERTIFICATION},
	{"PJPEG", FILE_TYPE_IMAGE},
	{"PNG",   FILE_TYPE_IMAGE},
	{"PPT",   FILE_TYPE_PPT},
	{"PPTX",  FILE_TYPE_PPT},
	{"SCN",   FILE_TYPE_MOVIE_MAKER},
	{"SDP",   FILE_TYPE_VIDEO},
	{"SMP",   FILE_TYPE_SOUND},
	{"SPF",   FILE_TYPE_SOUND},
	{"SPM",   FILE_TYPE_SOUND},
	{"SVG",   FILE_TYPE_SVG},
	{"SVGZ",  FILE_TYPE_SVG},
	{"SWF",   FILE_TYPE_FLASH},
	{"THM",   FILE_TYPE_THEME},
	{"TXT",   FILE_TYPE_TXT},
	{"VBM",   FILE_TYPE_VBOOKMARK},
	{"VCF",   FILE_TYPE_VCONTACT},
	{"VCS",   FILE_TYPE_VCALENDAR},
	{"VNT",   FILE_TYPE_VNOTE},
	{"WAV",   FILE_TYPE_SOUND},
	{"WBMP",  FILE_TYPE_IMAGE},
	{"WGT",   FILE_TYPE_WGT},
	{"WMA",   FILE_TYPE_MUSIC},
	{"WMV",   FILE_TYPE_VIDEO},
	{"XHTML", FILE_TYPE_HTML},
	{"XLS",   FILE_TYPE_EXCEL},
	{"XLSX",  FILE_TYPE_EXCEL},
	{"XMF",   FILE_TYPE_SOUND}
};

enum {
	FILE_TYPE_BY_EXT_ITEM_SIZE = sizeof(file_type_by_ext),
	FILE_TYPE_BY_EXT_LENGTH = sizeof(FILE_TYPE_BY_EXT) / FILE_TYPE_BY_EXT_ITEM_SIZE,
	MAX_EXT_LENGTH = 8
};

static char *icon_array[FILE_TYPE_MAX] = {
	[FILE_TYPE_DIR] = FM_ICON_FOLDER,
	[FILE_TYPE_IMAGE] = FM_ICON_IMAGE,
	[FILE_TYPE_VIDEO] = FM_ICON_VIDEO,
	[FILE_TYPE_MUSIC] = FM_ICON_MUSIC,
	[FILE_TYPE_SOUND] = FM_ICON_SOUND,
	[FILE_TYPE_PDF] = FM_ICON_PDF,
	[FILE_TYPE_PPT] = FM_ICON_PPT,
	[FILE_TYPE_VOICE] = FM_ICON_VOICE,
	[FILE_TYPE_TXT] = FM_ICON_TXT,
	[FILE_TYPE_VNOTE] = FM_ICON_VNOTE,
};

static int _model_utils_file_type_by_ext_compare(const void *key, const void *elem);
static file_type _model_utils_get_category_by_file_ext(const char *file_ext, const char *fullpath);
static int _model_utils_is_valid_name(const char *filename);

int model_utils_is_path_valid(const char *path, Eina_Bool is_dir)
{
	int result = RESULT_TYPE_OK;
	if (!path) {
		return RESULT_TYPE_INVALID_PATH;
	}

	size_t length = strlen(path);
	if (length == 0 || path[0] != '/') {
		return RESULT_TYPE_INVALID_PATH;
	}

	if (is_dir == EINA_TRUE) {
		if ((path[length - 1] == '/') && (length > 1)) {
			return RESULT_TYPE_INVALID_PATH;
		}
	}

	const char *file_name = NULL;
	file_name = common_util_get_filename(path);
	result = _model_utils_is_valid_name(file_name);

	if (result != RESULT_TYPE_OK) {
		ERR("Is not valid dir path name");
	}

	return result;
}

Eina_Bool model_utils_is_file_exists(const char *filepath)
{
	struct stat st;

	if ((stat(filepath, &st) < 0) && (strcmp(filepath, "/"))) {
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

Eina_Bool model_utils_file_is_dir(const char *filepath)
{
	struct stat st;

	if (stat(filepath, &st) < 0) {
		return EINA_FALSE;
	} if (S_ISDIR(st.st_mode)) {
		return EINA_TRUE;
	}

	return EINA_FALSE;
}

char *model_utils_get_dir_name(const char *filepath)
{
	char *ptr = NULL;
	char buf[PATH_MAX] = {'\0'};

	strncpy(buf, filepath, PATH_MAX);
	ptr = strrchr(buf, '/');
	if (!ptr) {
		return strdup(filepath);
	}

	if (ptr == buf) {
		return strdup("/");
	}

	*ptr = 0;
	return strdup(buf);
}

static int _model_utils_is_valid_name(const char *filename)
{
	int ret, z, cflags = 0;
	regex_t reg;
	regmatch_t pm[1];
	const size_t nmatch = 1;

	if (strncmp(filename, ".", 1) == 0) {
		return RESULT_TYPE_INVALID_FILE_NAME;
	}
	z = regcomp(&reg, FM_NAME_PATTERN, cflags);

	if (z != 0) {
		char ebuf[128];
		regerror(z, &reg, ebuf, sizeof(ebuf));
		fprintf(stderr, "%s: pattern '%s' \n", ebuf, FM_NAME_PATTERN);
		return RESULT_TYPE_INVALID_FILE_NAME;
	}

	z = regexec(&reg, filename, nmatch, pm, 0);
	if (z == REG_NOMATCH) {
		ret = RESULT_TYPE_OK;
	} else {
		ret = RESULT_TYPE_INVALID_FILE_NAME;
	}
	regfree(&reg);
	return ret;
}

static int _model_utils_file_type_by_ext_compare(const void *key, const void *elem)
{
	return strcmp((const char *)key, ((const file_type_by_ext *)elem)->ext);
}

static file_type _model_utils_get_category_by_file_ext(const char *file_ext, const char *fullpath)
{
	file_type result = FILE_TYPE_ETC;

	if (file_ext) {
		char ext_upper[MAX_EXT_LENGTH + 1];
		int i = 0;

		{
			const int skip = (file_ext[0] == '.') ? 1 : 0;
			while ((file_ext[i + skip] != '\0') && (i < MAX_EXT_LENGTH)) {
				ext_upper[i] = toupper(file_ext[i + skip]);
				++i;
			}
			ext_upper[i] = '\0';
		}

		if (i > 0) {
			file_type_by_ext *const item = bsearch(ext_upper,
					FILE_TYPE_BY_EXT,
					FILE_TYPE_BY_EXT_LENGTH,
					FILE_TYPE_BY_EXT_ITEM_SIZE,
					_model_utils_file_type_by_ext_compare);

			if (item) {
				result = item->ftype;
			}
		}
	}
	return result;
}

int model_utils_get_file_category(const char *filepath, file_type *category)
{
	if (model_utils_is_file_exists(filepath)) {
		*category = FILE_TYPE_DIR;
		return RESULT_TYPE_OK;
	}

	const char *filename = common_util_get_filename(filepath);
	if (!filename) {
		*category = FILE_TYPE_NONE;
		return RESULT_TYPE_INVALID_ARG;
	}

	const char *ext = strrchr(filename, '.');
	if ((ext) && (ext != filename) && (strlen(ext) != 1)) {
		*category = _model_utils_get_category_by_file_ext(ext + 1, filepath);
		return RESULT_TYPE_OK;
	}

	*category = FILE_TYPE_NONE;
	return RESULT_TYPE_FAIL;
}

const char *model_utils_get_default_icon_name(file_type ftype)
{
	const char *icon_name = FM_ICON_DEFAULT;

	if (icon_array[ftype]) {
		icon_name = icon_array[ftype];
	}

	return icon_name;
}

storage_type model_utils_get_storage_type(const char *fullpath)
{
	const size_t len_phone = strlen(FM_PHONE_FOLDER);
	const size_t len_memory = strlen(FM_MEMORY_FOLDER);
	storage_type storage = STORAGE_TYPE_NONE;

	if (strncmp(fullpath, FM_PHONE_FOLDER, len_phone) == 0) {
		storage = STORAGE_TYPE_PHONE;
	}  else {
		storage = STORAGE_TYPE_NONE;
	}
	return storage;
}

char *model_utils_get_public_file_path(const char *original_path)
{
	RETVM_IF(!original_path, NULL , "Input original path is NULL");

	const char *label = NULL;
	const char *folder = NULL;

	const storage_type storage = model_utils_get_storage_type(original_path);
	switch (storage) {
	case STORAGE_TYPE_PHONE:
		label = FM_PHONE_LABEL;
		folder = FM_PHONE_FOLDER;
		break;
	default:
		ERR("Failed to get public path");
		return NULL;
		break;
	}

	return common_util_strconcat(label, original_path + strlen(folder), "/", NULL);
}

int model_utils_launch_file(const node_info *file_info)
{
	RETVM_IF(!file_info, RESULT_TYPE_INVALID_ARG, "File info is NULL");

	int result = RESULT_TYPE_FAIL;

	RETVM_IF(!file_info->name || !file_info->parent_path, result, "File info data is not correct");

	char *fullpath = common_util_strconcat(file_info->parent_path, "/", file_info->name, NULL);
	RETVM_IF(!fullpath, result, "Generate full path to file failed");

	app_control_h app_ctrl = NULL;

	/* app control create */
	int ret = app_control_create(&app_ctrl);
	RETVM_IF(ret != APP_CONTROL_ERROR_NONE, result, "Failed to create app control. [0x%x]", ret);

	/* app control set URI */
	file_type category = file_info->type;
	if (category == FILE_TYPE_HTML) {
		char *html_path = common_util_strconcat("file://", fullpath, NULL);
		ret = app_control_set_uri(app_ctrl, html_path);
		free(html_path);
	} else if (category == FILE_TYPE_IMAGE) {
		app_control_add_extra_data(app_ctrl, "View By", "By Folder");
		ret = app_control_set_uri(app_ctrl, fullpath);
	} else {
		ret = app_control_set_uri(app_ctrl, fullpath);
	}

	if (ret != APP_CONTROL_ERROR_NONE) {
		ERR("Set app control uri failed. [0x%x]", ret);
		app_control_destroy(app_ctrl);
		free(fullpath);
		return result;
	}

	/* app control set operation */
	ret = app_control_set_operation(app_ctrl, APP_CONTROL_OPERATION_VIEW );
	if (ret != APP_CONTROL_ERROR_NONE) {
		ERR("Set app control operation failed. [0x%x]", ret);
		app_control_destroy(app_ctrl);
		free(fullpath);
		return result;
	}

	/* app control launch request */
	ret = app_control_send_launch_request(app_ctrl, NULL, NULL);
	if (ret == APP_CONTROL_ERROR_APP_NOT_FOUND) {
		ERR("App control file not found. [0x%x]", ret);
		result = RESULT_TYPE_APP_CTRL_NOT_FOUND;
	} else if (ret != APP_CONTROL_ERROR_NONE) {
		ERR("App control launch failed. [0x%x]", ret);
		result = RESULT_TYPE_APP_CTRL_LAUNCH_FAILED;
	} else {
		result = RESULT_TYPE_OK;
	}

	app_control_destroy(app_ctrl);
	free(fullpath);

	return result;
}

storage_type model_utils_is_root_path(const char *fullpath)
{
	RETVM_IF(!fullpath, STORAGE_TYPE_NONE , "Input fullpath is NULL");

	if (!strcmp(fullpath, FM_PHONE_FOLDER)) {
		return STORAGE_TYPE_PHONE;
	}  else {
		return STORAGE_TYPE_NONE;
	}
}

int model_utils_read_dir(const char *dir_path, Eina_List **dir_list, Eina_List **file_list)
{
	RETVM_IF(!dir_path, RESULT_TYPE_INVALID_ARG, "dir_path is NULL");
	RETVM_IF(!dir_list, RESULT_TYPE_INVALID_ARG, "dir_list is NULL");
	RETVM_IF(!file_list, RESULT_TYPE_INVALID_ARG, "file_list is NULL");

	DIR *const pDir = opendir(dir_path);
	RETVM_IF(!pDir, RESULT_TYPE_DIR_NOT_FOUND, "Failed to open dir %s", dir_path);

	struct dirent ent_struct;
	struct dirent *ent = NULL;
	while ((readdir_r(pDir, &ent_struct, &ent) == 0) && ent) {
		int skip = ((strncmp(ent->d_name, ".", 1) == 0) ||
					(strncmp(ent->d_name, "..", 2) == 0));

		skip = skip || ((ent->d_type != DT_DIR) && (ent->d_type != DT_REG));

		skip = skip || ((ent->d_type == DT_DIR) &&
				(strcmp(dir_path, FM_PHONE_FOLDER) == 0) &&
				(strcmp(ent->d_name, FM_DEBUG_FOLDER) == 0));

		node_info *const pNode = skip ? NULL : calloc(1, sizeof(node_info));

		if (pNode) {
			pNode->parent_path = strdup(dir_path);
			pNode->name = strdup(ent->d_name);
			pNode->is_selected = EINA_FALSE;

			if (ent->d_type == DT_DIR) {
				pNode->type = FILE_TYPE_DIR;
			} else {
				model_utils_get_file_category(ent->d_name, &(pNode->type));
			}

			if (pNode->type == FILE_TYPE_DIR) {
				*dir_list = eina_list_append(*dir_list, pNode);
			} else {
				*file_list = eina_list_append(*file_list, pNode);
			}
		}
	}

	closedir(pDir);

	return RESULT_TYPE_OK;
}
