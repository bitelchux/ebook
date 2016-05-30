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

/*global $, console*/

/**
 * Clipboard class constructor.
 *
 * @public
 * @constructor
 */
function Clipboard() {
    'use strict';

    this.mode = this.INACTIVE_MODE;
}

(function start() { // strict mode wrapper
    'use strict';

    Clipboard.prototype = {
        /**
         * Clipboard mode for copying.
         *
         * @public
         * @const {number}
         */
        COPY_MODE_ID: 0,

        /**
         * Clipboard mode for moving.
         *
         * @private
         * @const {number}
         */
        MOVE_MODE_ID: 1,

        /**
         * Clipbboard inactive mode.
         *
         * @private
         * @const {number}
         */
        INACTIVE_MODE: -1,

        /**
         * Clipboard data.
         *
         * @private
         * @type {string[]}
         */
        data: [],

        /**
         * Clipboard mode: [copy | move | inactive].
         *
         * @private
         * @type {string}
         */
        mode: '',

        /**
         * Returns all paths in clipboard.
         *
         * @public
         * @returns {string[]}
         */
        get: function Clipboard_get() {
            return this.data;
        },

        /**
         * Adds a new path to the clipboard.
         * Returns current length of clipboard objects.
         *
         * @public
         * @param {string[]} paths Array of full paths.
         * @returns {number} Current length of clipboard objects.
         */
        add: function Clipboard_add(paths) {
            var len = paths.length,
                i = 0;

            // clear clipboard
            this.clear();
            for (i = 0; i < len; i += 1) {
                if (this.has(paths[i]) === false) {
                    this.data.push(paths[i]);
                }
            }

            return this.data.length;
        },

        /**
         * Removes the specified path if it is in clipboard.
         * Returns current count of objects in the clipboard.
         *
         * @public
         * @param {string} path full path
         * @returns {number} Current count of objects in the clipboard.
         */
        remove: function Clipboard_remove(path) {
            var index = $.inArray(path, this.data),
                length = 0;

            if (index >= 0) {
                this.data.splice(index, 1);
                length = this.data.length;
                if (length === 0) {
                    this.mode = this.INACTIVE_MODE;
                }
            }
            return length;
        },

        /**
         * Removes the specified path and all children paths if they
         * are already in clipboard.
         * Returns current count of objects in the clipboard.
         *
         * @public
         * @param {string} path Full path.
         * @returns {number} Current count of objects in the clipboard.
         */
        removeRecursively: function Clipboard_removeRecursively(path) {
            var escapeRegExp = function escapeRegExp(str) {
                    return str
                        .replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, '\\$&');
                },
                childPattern = new RegExp(escapeRegExp(path)),
                index = this.data.length - 1;

            while (index >= 0) {
                if (childPattern.test(this.data[index])) {
                    this.data.splice(index, 1);
                }
                index -= 1;
            }
            return this.data.length;
        },

        /**
         * Checks if the specified path is already in the clipboard.
         * Returns true if specified path is already in the clipboard, false
         * otherwise.
         *
         * @public
         * @param {string} path Full path.
         * @returns {boolean}
         */
        has: function Clipboard_has(path) {
            return $.inArray(path, this.data) !== -1;
        },

        /**
         * Clears all clipboard data and resets clipboard mode.
         *
         * @public
         */
        clear: function Clipboard_clear() {
            this.data = [];
            this.mode = this.INACTIVE_MODE;
        },

        /**
         * Sets clipboard mode.
         * Returns true if mode was successfully set, false otherwise.
         *
         * @public
         * @param {number} mode
         * @returns {boolean}
         */
        setMode: function Clipboard_setMode(mode) {
            if (
                $.inArray(
                    mode,
                    [this.MOVE_MODE_ID, this.COPY_MODE_ID]
                ) === false
            ) {
                console.error('Incorrect clipboard mode');
                return false;
            }
            this.mode = mode;
            return true;
        },

        /**
         * Returns the clipboard mode.
         *
         * @public
         * @returns {number} mode Clipboard mode.
         */
        getMode: function Clipboard_getMode() {
            return this.mode;
        },

        /**
         * Returns true if the clipboard is empty, false otherwise.
         *
         * @public
         * @returns {boolean}
         */
        isEmpty: function Clipboard_isEmpty() {
            return this.data.length === 0;
        }
    };
}());
