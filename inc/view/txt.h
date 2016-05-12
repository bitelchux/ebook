/*
 * txt.h
 *
 *  Created on: May 6, 2016
 *      Author: jkjang
 */

#ifndef TXT_H_
#define TXT_H_

#include "utils/app-types.h"
#include "view/view.h"
#include <Elementary.h>
#include <efl_extension.h>

#define ELM_DEMO_EDJ "edje/ui_controls.edj"



#endif /* TXT_H_ */


int txt_view_add(struct _app_data *app, Evas_Object *parent, const char *path, const char *dir_name, const node_info* file_info);
