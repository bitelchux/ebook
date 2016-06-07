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

/*global $*/
/**
 * ModifierManager class constructor.
 *
 * @public
 * @constructor
 */
function ModifierManager() {
    'use strict';

    this.init();
}

(function start() {
    'use strict';

    ModifierManager.prototype = {

        /**
         * Modifiers definitions.
         *
         * @private
         * @type {object}
         */
        modifiers: {

            /**
             * Returns HTML content of the span containing the specified text.
             *
             * @public
             * @param {string} str
             * @returns {string}
             */
            escape: function escape(str) {
                return $('<span>').text(str).html();
            },

            /**
             * Returns the specified string to the URI encoded version.
             *
             * @public
             * @param {string} str
             * @returns {string}
             */
            escapeEncies: function escapeEncies(str) {
                var tagsToReplace = {
                    '&': '&amp;',
                    '<': '&lt;',
                    '>': '&gt;',
                    '"': '&quot;'
                };

                return str.replace(/[&<>\"]/g, function replace(tag) {
                    return tagsToReplace[tag] || tag;
                });
            }
        },

        /**
         * UI module initialization.
         *
         * @public
         */
        init: function init() {
            return;
        },

        /**
         * Returns modifiers.
         *
         * @public
         * @returns {object} modifiers object.
         */
        getAll: function getAll() {
            return this.modifiers;
        }

    };
}());
