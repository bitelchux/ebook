/*
 * Copyright 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 */

#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__

#include <Evas.h>
#include <efl_extension.h>
#include <Elementary.h>

/* Forward declaration: */
struct _fs_manager;
struct _window_obj;
//struct _clipboard;
//struct _navi_path_widget;
struct _navigator;
struct _navi_path_storage;

typedef enum {
	MODE_DEFAULT = 0,
	MODE_EDIT
} mode;

/**
 * @brief Application status data
 */
typedef struct _status {
	mode curr_mode;         /**< Current app mode */
	Eina_Bool is_mainview;  /**< Current view is main view or not */
} status_data;

/**
 * @brief Application data
 */
typedef struct _app_data {
	struct _window_obj *win;                /**< Window object */
	struct _fs_manager *manager;            /**< File managet object */
//	struct _clipboard *clipboard;           /**< Clipboard object */
	struct _navigator *navigator;           /**< Navigator object */
	struct _navi_path_storage *path_storage;/**< Path storage object */
	Evas_Object *navi;                      /**< Naviframe object */
	status_data status;                     /**< App status object */
	Evas_Object *layout;
	Eext_Circle_Surface *circle_surface;
} app_data;

/**
 * @brief Create application instance
 * @return Application instance on success, otherwise NULL
 */
app_data *app_create();

/**
 * @brief Destroy application instance
 * @param[in]   app     Application instance
 */
void app_destroy(app_data *app);

/**
 * @brief Run Tizen application
 * @param[in]   app     Application instance
 * @param[in]   argc    argc paremeter received in main
 * @param[in]   argv    argv parameter received in main
 */
int app_run(app_data *app, int argc, char **argv);

void eext_genlist_cb(void *data, Evas_Object *obj, void *event_info);


#endif // __MAIN_APP_H__

