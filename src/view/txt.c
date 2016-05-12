/*
 * txt.c
 *
 *  Created on: May 6, 2016
 *      Author: jkjang
 */
#include "view/txt.h"
#include "view/list-view.h"
//#include "view/ctrl-bar-view.h"
#include "view/genlist-widget.h"
#include "view/navi-path-widget.h"
//#include "view/navi-bar-title.h"
#include "view/navigator.h"
//#include "view/popup.h"
#include "utils/app-types.h"
#include "utils/ui-utils.h"
#include "utils/common-utils.h"
#include "utils/model-utils.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "model/fs-manager.h"
//#include "model/getclipboard.h"
#include "model/navi-path-storage.h"
#include "main-app.h"
#include <app.h>

typedef struct {
	view_data txt_view;
	navi_path_widget *navi_path_wgt;
	Eina_List *file_list;
} txt_view_data;

static void _txt_view_destroy(txt_view_data *lv_data);
static void _txt_view_del_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);


static void
app_get_resource(const char *res_file_in, char *res_path_out, int res_path_max)
	{
	char *res_path = app_get_resource_path();
	if(res_path){
		snprintf(res_path_out,res_path_max,"%s%s",res_path,res_file_in);
		free(res_path);
		}
	}

static char*
read_file(const char* filepath)
	{
	FILE *fp = fopen(filepath,"r");
	if(fp == NULL)
		return NULL;
	fseek(fp,0,SEEK_END);
	int bufsize = ftell(fp)+ sizeof("<br><br><align=center></br></br>") + 10;
	rewind(fp);
	if(bufsize < 1)
		return NULL;

	char *buf = malloc(sizeof(char) * (bufsize));
	memset(buf, '\0',sizeof(buf));
	sprintf(buf,"%s","<br/><br/><align=center>");

	char str[200];
	while(fgets(str,200,fp)!= NULL){
		dlog_print(DLOG_ERROR,"tag","%s",str);
		sprintf(buf+strlen(buf), "%s",str);
	}
	sprintf(buf+strlen(buf),"%s","<br/><br/>\0");

	fclose(fp);
	return buf;
	}


int txt_view_add(app_data *app, Evas_Object *parent, const char *path, const char *dir_name, const node_info *file_info)
	{
	RETVM_IF(!app, RESULT_TYPE_INVALID_ARG, "App object is NULL");
	RETVM_IF(!parent, RESULT_TYPE_INVALID_ARG, "Parent object is NULL");
	RETVM_IF(!path, RESULT_TYPE_INVALID_ARG, "Path is NULL");

	app->status.is_mainview = EINA_FALSE;

	txt_view_data *data = calloc(1, sizeof(txt_view_data));
	RETVM_IF(!data, RESULT_TYPE_FAIL_ALLOCATE_MEMORY, "Fail allocate memory");

	data->txt_view.app = app;
	data->txt_view.navi = parent;
	data->txt_view.curr_path = (dir_name) ? common_util_strconcat(path, "/", dir_name, NULL)
									  : strdup(path);
	data->txt_view.is_root = model_utils_is_root_path(data->txt_view.curr_path);

	Evas_Object *scroller, *layout, *label;

	char* buf = NULL;
	char filepath[PATH_MAX] = {0,};
	app_get_resource("test.txt",filepath,PATH_MAX);
	//char *fullpath = common_util_strconcat(file_info->parent_path, "/", file_info->name, NULL);
	buf = read_file(filepath);
	/* Create Layout */

	layout = elm_layout_add(data->txt_view.navi);
	elm_layout_file_set(layout, ui_utils_get_resource(ELM_DEMO_EDJ), "elmdemo-test/scroller");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_event_callback_add(data->txt_view.navi, EVAS_CALLBACK_FREE,  _txt_view_del_cb, data);
	/* Create Scroller */
	scroller = elm_scroller_add(layout);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	elm_object_style_set(scroller, "effect");
	evas_object_show(scroller);

	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	/* Create Content */
	label = elm_label_add(scroller);
	elm_object_text_set(label, buf);

	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
	elm_label_wrap_width_set(label, 360);
	evas_object_size_hint_weight_set(label, 0.5, 0.5);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(label);

	elm_object_part_content_set(data->txt_view.navi_layout, "scroller", scroller);
	elm_object_content_set(scroller, label);

	Elm_Object_Item *it = elm_naviframe_item_push(data->txt_view.navi, "Scroller", NULL, NULL, layout, "empty");
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);

	/* Content Set*/
	elm_object_part_content_set(layout, "scroller", scroller);

	elm_object_item_data_set(data->txt_view.navi_item, data);

	return RESULT_TYPE_OK;
}


static void _txt_view_destroy(txt_view_data *lv_data)
{
	if (lv_data) {
		app_data *app = lv_data->txt_view.app;

		app->status.curr_mode = MODE_DEFAULT;

//		common_util_clear_file_list(&lv_data->file_list);

		free(lv_data->txt_view.curr_path);
		free(lv_data);
	}
}

static void _txt_view_del_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	_txt_view_destroy(data);
}

