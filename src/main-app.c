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

#include "main-app.h"
#include "view/window.h"
 #include "view/main-view.h"
#include "view/list-view.h"
#include "view/navigator.h"
#include "utils/ui-utils.h"
#include "utils/logger.h"
#include "model/fs-manager.h"
#include "model/clipboard.h"
#include "model/navi-path-storage.h"
#include "utils/app-types.h"

#include <stdlib.h>
#include <Elementary.h>
#include <app.h>
#include <elm_app.h>

#define BASE_DISPLAY_SCALE 1.8


/* app event callbacks */
static bool _on_create_cb(void *user_data);
static void _on_terminate_cb(void *user_data);
static void _on_pause_cb(void *user_data);
static void _on_resume_cb(void *user_data);
static void _on_app_control_cb(app_control_h app_control, void *user_data);

static void         _app_clear_data(app_data *app);
static Evas_Object *_app_add_naviframe(app_data *app);
static void         _app_naviframe_backbutton_cb(void *data, Evas_Object *obj, void *event_info);

app_data *app_create()
{
	app_data *app = calloc(1, sizeof(app_data));
	return app;
}

void app_destroy(app_data *app)
{
	free(app);
}

int app_run(app_data *app, int argc, char **argv)
{
	if (!app) {
		return -1;
	}

	ui_app_lifecycle_callback_s cbs = {
		.create = _on_create_cb,
		.terminate = _on_terminate_cb,
		.pause = _on_pause_cb,
		.resume = _on_resume_cb,
		.app_control = _on_app_control_cb
	};

	return ui_app_main(argc, argv, &cbs, app);
}

static bool _on_create_cb(void *user_data)
{
	app_data *app = user_data;

	if (!app) {
		return false;
	}

	elm_app_base_scale_set(BASE_DISPLAY_SCALE);

	/* Create file system manager */
	app->manager = fs_manager_create();
	RETVM_IF(!app->manager, false, "Failed to create file system manager");

	/* Create clipboard */
/*	app->clipboard = clipboard_create();
	if (!app->clipboard) {
		ERR("Failed to create clipboard");
		fs_manager_destroy(app->manager);
		return false;
	}*/

	/* Create main window */
	app->win = win_create();
	if (!app->win) {
		ERR("Failed to create main window");
		_app_clear_data(app);
		return false;
	}


	/* Create naviframe */
	app->navi = _app_add_naviframe(app);
	if (!app->navi) {
		ERR("Failed to create naviframe");
		_app_clear_data(app);
		return false;
	}

	/* Create navigation path storage */
	app->path_storage = navi_path_storage_create();
	if (!app->path_storage) {
		ERR("Failed to create navigation path storage");
		_app_clear_data(app);
		return false;
	}

	/* Create navigator */
	app->navigator = navigator_create(app);
	if (!app->navigator) {
		ERR("Failed to create navigator");
		_app_clear_data(app);
		return false;
	}

	/* Create main view */
	int res = main_view_add(app, app->navi);
	if (res != RESULT_TYPE_OK) {
		ERR("Failed to create main view");
		_app_clear_data(app);
		return false;
	}

	return true;
}

static void _on_terminate_cb(void *user_data)
{
	app_data *app = user_data;
	_app_clear_data(app);
}

static void _on_pause_cb(void *user_data)
{
}

static void _on_resume_cb(void *user_data)
{
}

static void _on_app_control_cb(app_control_h app_control, void *user_data)
{
}

static void _app_clear_data(app_data *app)
{
	 if (app) {
		fs_manager_destroy(app->manager);
		app->manager = NULL;
	/*	clipboard_destroy(app->clipboard);
		app->clipboard = NULL;
	*/	win_destroy(app->win);
		app->win = NULL;
		navigator_destroy(app->navigator);
		app->navigator = NULL;
		navi_path_storage_destroy(app->path_storage);
		app->path_storage = NULL;
	}
}

static Evas_Object *_app_add_naviframe(app_data *app)
{
	Evas_Object *naviframe = NULL;
	Evas_Object *parent = win_get_host_layout(app->win);
	if (parent) {

		naviframe = ui_utils_navi_add(parent, _app_naviframe_backbutton_cb
											, app);
		if (naviframe) {
			win_set_layout(app->win, naviframe);
		}
	}
	return naviframe;
}

static void _app_naviframe_backbutton_cb(void *data, Evas_Object *obj, void *event_info)
{
	app_data *app = (app_data *)data;

	if (app->status.curr_mode != MODE_DEFAULT) {
		app->status.curr_mode = MODE_DEFAULT;
		if (app->status.is_mainview) {
			RETM_IF(main_view_update(
					elm_naviframe_top_item_get(obj)) != RESULT_TYPE_OK,
					"Failed to navigate to previous view");
		} else {
			RETM_IF(list_view_update(
					elm_naviframe_top_item_get(obj), UPDATE_TYPE_GENLIST |
					UPDATE_TYPE_CTRL_BAR_MODE) != RESULT_TYPE_OK,
					"Failed to navigate to previous view");
		}
		return;
	}

	if (elm_naviframe_top_item_get(obj) == elm_naviframe_bottom_item_get(obj)) {
		if (app) {
			win_lower(app->win);
		}
	} else {
		int res = navigator_goto_previous_view(app->navigator);
		RETM_IF(res != RESULT_TYPE_OK, "Failed to navigate to previous view");
	}
}
