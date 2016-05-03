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

#ifndef __WINDOW_OBJ_H__
#define __WINDOW_OBJ_H__

#include <Evas.h>
#include <efl_extension.h>

typedef struct _window_obj {
	Evas_Object *win;       /**< Window object */
	Evas_Object *bg;        /**< Background object */
	Evas_Object *conform;   /**< Conform object */
	Evas_Object *layout;    /**< Main layout object */
	Eext_Circle_Surface *circle_surface;

} window_obj;

/**
 * @brief Creates window
 * @return Window object on success or NULL.
 */
window_obj *win_create();

/**
 * @brief Lower a window object.
 * @param[in]   obj   A window object
 */
void win_lower(window_obj *obj);

/**
 * @brief Free window object
 * @param[in]   obj   A window object
 */
void win_destroy(window_obj *obj);

/**
 * @brief Setes a layout object to window
 * @param[in]   obj      Window object.
 * @param[in]   layout   Layout object.
 */
void win_set_layout(window_obj *obj, Evas_Object *layout);

/**
 * @brief Returns host layout object
 * @param[in]   obj   Window object.
 * @return Layout object on success or NULL.
 */
Evas_Object *win_get_host_layout(const window_obj *obj);

#endif // __WINDOW_OBJ_H__
