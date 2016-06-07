/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
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

/*global $, App  */

/**
 * This file acts as a loader for the application and its dependencies.
 *
 * First, the 'app.js' script is loaded.
 * Then, scripts defined in 'app.requires' are loaded.
 * Finally, the app is initialized - the app is instantiated ('app = new App()')
 * and 'app.init()' is called.
 * @public
 */

/**
 * App class instance.
 *
 * @public
 * @type {App}
 */
var app = null;

(function start() { // strict mode wrapper
    'use strict';

    ({
        /**
         * Loader init - loads the App constructor.
         *
         * @private
         */
        init: function init() {
            var self = this;

            $.getScript('js/app.js')
                .done(function done() {
                    // once the app is loaded, create the app object
                    // and load the libraries
                    app = new App();
                    self.loadLibs();
                })
                .fail(this.onGetScriptError);
        },

        /**
         * Loads dependencies.
         *
         * @private
         */
        loadLibs: function loadLibs() {
            var loadedLibs = 0,
                i = 0,
                filename = null;

            function onGetScriptDone() {
                loadedLibs += 1;
                if (loadedLibs >= app.requires.length) {
                    app.init();
                }
            }

            if ($.isArray(app.requires)) {
                for (i = 0; i < app.requires.length; i += 1) {
                    filename = app.requires[i];
                    $.getScript(filename)
                        .done(onGetScriptDone)
                        .fail(this.onGetScriptError);
                }
            }
        },

        /**
         * Handles ajax errors
         * .
         * @private
         * @param {Event} e
         */
        onGetScriptError: function onGetScriptError(e) {
            app.ui.alertPopup('An error occurred: ' + e.message);
        }
    }).init(); // run the loader
}());
