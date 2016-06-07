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

/*global $ */

/**
 * Helpers class constructor.
 *
 * @constructor
 */
function Helpers() {
    'use strict';

    return;
}

(function start() { // strict mode wrapper
    'use strict';

    Helpers.prototype = {

        /**
         * Maps an extension to the icon name representing this extension.
         *
         * @private
         * @type {object}
         */
        extensions: {
            '.bmp':       'img.png',
            '.gif':       'img.png',
            '.jpeg':      'img.png',
            '.jpg':       'img.png',
            '.png':       'img.png',
            '.tiff':      'img.png',
            '.3ga':       'music.png',
            '.aac':       'music.png',
            '.ac3':       'music.png',
            '.amr':       'music.png',
            '.awb':       'music.png',
            '.m4a':       'music.png',
            '.m4p':       'music.png',
            '.m4r':       'music.png',
            '.mp3':       'music.png',
            '.ogg':       'music.png',
            '.wav':       'music.png',
            '.wma':       'music.png',
            '.3gp':       'video.png',
            '.avc':       'video.png',
            '.avi':       'video.png',
            '.m4v':       'video.png',
            '.mkv':       'video.png',
            '.mov':       'video.png',
            '.mp4':       'video.png',
            '.mpeg':      'video.png',
            '.mpg':       'video.png',
            '.ogv':       'video.png',
            '.vc1':       'video.png',
            '.wmv':       'video.png',
            '.doc':       'text.png',
            '.docx':      'text.png',
            '.odt':       'text.png',
            '.ods':       'text.png',
            '.txt':       'text.png',
            '.xls':       'text.png',
            '.xlsx':      'text.png',
            '.vcard':     'text.png',
            '.vcf':       'text.png',
            '.icalendar': 'text.png',
            '.ical':      'text.png',
            '.ics':       'text.png',
            '.ifb':       'text.png',
            '.pdf':       'pdf.png',
            '.odp':       'ppt.png',
            '.ppt':       'ppt.png',
            '.wgt':       'etc.png'
        },

        /**
         * Capitalizes the first letter.
         *
         * @public
         * @param {string} text
         * @returns {string}
         */
        UCFirst: function Helpers_UCFirst(text) {
            return text.charAt(0).toUpperCase() + text.slice(1);
        },

        /**
         * Returns the name without extension of the specified file name.
         *
         * @private
         * @param {string} fileName
         * @returns {string} file name without extension
         */
        getFileName: function Helpers_getFileName(fileName) {
            var fileNameLen = fileName.lastIndexOf('.');
            if (fileNameLen !== -1) {
                fileName = fileName.slice(0, fileNameLen);
            }
            return fileName;
        },

        /**
         * Returns extension of the specified file name.
         *
         * @private
         * @param {string} fileName
         * @returns {string} extension for specified file name
         */
        getFileExtension: function Helpers_getFileExtension(fileName) {
            var splittedFileName = fileName.split('.'),
                ext = '';

            if (splittedFileName.length > 1) {
                ext = '.' + splittedFileName.pop();
            }
            return ext;
        },

        /**
         * Returns icon filename for the given extension.
         * For example, for '.mp3' returns 'music.png'.
         *
         * @private
         * @param {string} ext
         * @returns {string}
         */
        resolveFileIcon: function Helpers_resolveFileIcon(ext) {
            ext = ext.toLowerCase();
            return this.extensions[ext] || 'etc.png';
        },

        /**
         * Returns thumbnail URI for specified file.
         *
         * @public
         * @param {string} fileName
         * @param {File} node
         * @returns {string}
         */
        getThumbnailURI: function Helpers_getThumbnailURI(fileName, node) {
            var ext = this.getFileExtension(fileName);

            if (!node.thumbnailURIs) {
                return 'images/' + this.resolveFileIcon(ext);
            }

            if (
                node.thumbnailURIs[0] &&
                    $.inArray(ext, ['.mp4', '.jpg', '.png', '.gif'])
            ) {
                return node.thumbnailURIs[0];
            }

            return 'images/etc.png';
        },

        /**
         * Checks if the specified list of files contains a file with the
         * given name. If not, this filename is returned.
         * Otherwise the unique name is generated and returned. This name is
         * created by adding the number in parenthesis at the end of the file.
         *
         * For example if the test.txt file exists in the files list, the
         * test(n).txt is returned, where n is minimal integer value making this
         * name unique.
         *
         * @public
         * @param {string} sourceName
         * @param {File[]} filesList
         * @returns {string}
         */
        getCopyFileName: function Helpers_getCopyFileName(
            sourceName,
            filesList
        ) {
            var i = 1, copyFileName = sourceName,
                filesNames = filesList.map(function getElementName(element) {
                    return element.name;
                }),
                index = filesNames.indexOf(copyFileName),
                ext = this.getFileExtension(sourceName);

            while (index !== -1) {
                if (filesList[index].isDirectory) {
                    copyFileName = sourceName + '(' + i + ')';
                } else {
                    copyFileName = this.getFileName(
                        sourceName
                    ) + '(' + i + ')' + ext;
                }
                i += 1;
                index = filesNames.indexOf(copyFileName);
            }

            return copyFileName;
        },

        /**
         * Fixes invalid URI returned by API's File::toURI() method.
         * Returns corrected URI.
         *
         * @public
         * @param {string} invalidUri
         * @returns {string}
         */
        fixURI: function Helpers_fixURI(invalidUri) {
            var scheme, address, k;
            invalidUri = invalidUri.split('://');
            scheme = invalidUri[0];
            invalidUri.shift();
            address = invalidUri.join('://').split('/');
            for (k = address.length - 1; k >= 0; k -= 1) {
                address[k] = encodeURIComponent(address[k]);
            }
            return scheme + '://' + address.join('/');
        },

        /**
         * File comparison function using their names (case insensitive).
         *
         * @public
         * @param {File} x
         * @param {File} y
         * @returns {number}
         */
        fileComparison: function fileComparison(x, y) {
            if (x.isDirectory !== y.isDirectory) {
                return x.isDirectory ? -1 : 1;
            }
            var a = x.name.toLowerCase(),
                b = y.name.toLowerCase();
            if (a < b) {
                return -1;
            }
            if (a > b) {
                return 1;
            }
            return 0;
        }

    };
}());
