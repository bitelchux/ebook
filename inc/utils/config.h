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

#ifndef CONFIG_H_
#define CONFIG_H_

#define FM_PHONE_FOLDER    "/opt/usr/media"
#define FM_MEMORY_FOLDER   "/opt/storage/sdcard"

#define FM_PHONE_LABEL  "EbookReader"
#define FM_MEMORY_LABEL "SD Card"
#define FM_DEBUG_FOLDER "SLP_debug"

#define FM_ICON_PATH "images/"

#define FM_LAYOUT_EDJ "edje/filelist.edj"

#ifndef FM_ICON_DEFAULT
#define FM_ICON_DEFAULT FM_ICON_PATH"etc.png"
#endif

#define FM_ICON_FOLDER      FM_ICON_PATH"folder.png"
#define FM_ICON_IMAGE       FM_ICON_PATH"img.png"
#define FM_ICON_VIDEO       FM_ICON_PATH"video.png"
#define FM_ICON_MUSIC       FM_ICON_PATH"music.png"
#define FM_ICON_SOUND       FM_ICON_PATH"music.png"
#define FM_ICON_PDF         FM_ICON_PATH"pdf.png"
#define FM_ICON_PPT         FM_ICON_PATH"ppt.png"
#define FM_ICON_VOICE       FM_ICON_PATH"music.png"
#define FM_ICON_TXT         FM_ICON_PATH"text.png"
#define FM_ICON_VNOTE       FM_ICON_PATH"text.png"

#define FM_TITLE_ICON_HOME                  FM_ICON_PATH"home.png"
#define FM_TITLE_ICON_HOME_PRESS            FM_ICON_PATH"home_press.png"
#define FM_TITLE_ICON_UPPER                 FM_ICON_PATH"up_folder.png"
#define FM_TITLE_ICON_UPPER_PRESS           FM_ICON_PATH"up_folder_press.png"

#endif /* CONFIG_H_ */
