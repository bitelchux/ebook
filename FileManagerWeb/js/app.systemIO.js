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

/*global tizen, console */

/**
 * SystemIO class constructor.
 *
 * @public
 * @constructor
 */
function SystemIO() {
    'use strict';

    return;
}

(function start() { // strict mode wrapper
    'use strict';

    SystemIO.prototype = {
        /**
         * Creates new empty file in specified location.
         * Returns file handler if file was created, false otherwise.
         *
         * @private
         * @param {File} directoryHandle
         * @param {string} fileName
         * @returns {boolean}
         */
        createFile: function SystemIO_createFile(directoryHandle, fileName) {

            try {
                return directoryHandle.createFile(fileName);
            } catch (e) {
                console.error('SystemIO_createFile error:' + e.message);
                return false;
            }
        },

        /**
         * Writes content to file stream.
         *
         * @private
         * @param {File} fileHandle File handler.
         * @param {string} fileContent File content.
         * @param {function} onSuccess Success callback.
         * @param {function} onError Error callback.
         * @param {string} contentEncoding Content encoding.
         */
        writeFile: function SystemIO_writeFile(
            fileHandle,
            fileContent,
            onSuccess,
            onError,
            contentEncoding
        ) {
            onError = onError || function noop() {
                return;
            };

            fileHandle.openStream('w', function success(fileStream) {
                if (contentEncoding === 'base64') {
                    fileStream.writeBase64(fileContent);
                } else {
                    fileStream.write(fileContent);
                }

                fileStream.close();

                // launch onSuccess callback
                if (typeof onSuccess === 'function') {
                    onSuccess();
                }
            }, onError, 'UTF-8');
        },

        /**
         * Opens the specified location.
         *
         * @public
         * @param {string} directoryPath Directory path.
         * @param {function} onSuccess Success callback.
         * @param {function} onError Error callback.
         * @param {string} openMode Open Mode.
         */
        openDir: function SystemIO_openDir(
            directoryPath,
            onSuccess,
            onError,
            openMode
        ) {
            openMode = openMode || 'rw';
            onSuccess = onSuccess || function noop() {
                return;
            };

            try {
                tizen.filesystem.resolve(
                    directoryPath,
                    onSuccess,
                    onError,
                    openMode
                );
            } catch (e) {
                console.error(e.message);
            }
        },

        /**
         * Parses the specified file path and returns data parts.
         *
         * @private
         * @param {string} filePath
         * @returns {string[]}
         */
        getPathData: function SystemIO_getPathData(filePath) {
            var path = {
                    originalPath: filePath,
                    fileName: '',
                    dirName: ''
                },
                splittedPath = filePath.split('/');

            path.fileName = splittedPath.pop();
            path.dirName = splittedPath.join('/') || '/';

            return path;
        },

        /**
         * Saves the specified file content.
         *
         * @public
         * @param {string} filePath
         * @param {string} fileContent
         * @param {string} onSaveSuccess
         * @param {string} fileEncoding
         */
        saveFileContent: function SystemIO_saveFileContent(
            filePath,
            fileContent,
            onSaveSuccess,
            fileEncoding
        ) {
            var pathData = this.getPathData(filePath),
                self = this,
                fileHandle = null;

            function onOpenDirSuccess(dir) {
                // create new file
                fileHandle = self.createFile(dir, pathData.fileName);
                if (fileHandle !== false) {
                    // save data into this file
                    self.writeFile(
                        fileHandle,
                        fileContent,
                        onSaveSuccess,
                        false,
                        fileEncoding
                    );
                }
            }

            // open directory
            this.openDir(pathData.dirName, onOpenDirSuccess);
        },

        /**
         * Deletes a node with the specified path.
         *
         * @public
         * @param {string} nodePath
         * @param {function} onSuccess
         */
        deleteNode: function SystemIO_deleteNode(nodePath, onSuccess) {
            var pathData = this.getPathData(nodePath),
                self = this;

            function onDeleteSuccess() {
                onSuccess();
            }

            function onDeleteError(e) {
                console.error('SystemIO_deleteNode:_onDeleteError', e);
            }

            function onOpenDirSuccess(dir) {
                var onListFiles = function onListFiles(files) {
                    if (files.length > 0) {
                        // file exists;
                        if (files[0].isDirectory) {
                            self.deleteDir(
                                dir,
                                files[0].fullPath,
                                onDeleteSuccess,
                                onDeleteError
                            );
                        } else {
                            self.deleteFile(
                                dir,
                                files[0].fullPath,
                                onDeleteSuccess,
                                onDeleteError
                            );
                        }
                    } else {
                        onDeleteSuccess();
                    }
                };

                // check file exists;
                dir.listFiles(onListFiles, function error(e) {
                    console.error(e);
                }, {
                    name: pathData.fileName
                });
            }

            this.openDir(pathData.dirName, onOpenDirSuccess, function error(e) {
                console.error('openDir error:' + e.message);
            });
        },

        /**
         * Deletes the specified file.
         * Returns false if couldn't delete file.
         *
         * @public
         * @param {File} dir
         * @param {string} filePath path
         * @param {function} onDeleteSuccess success callback
         * @param {function} onDeleteError error callback
         * @returns {boolean}
         */
        deleteFile: function SystemIO_deleteFile(
            dir,
            filePath,
            onDeleteSuccess,
            onDeleteError
        ) {
            try {
                dir.deleteFile(filePath, onDeleteSuccess, onDeleteError);
            } catch (e) {
                console.error('SystemIO_deleteFile error: ' + e.message);
                return false;
            }
        },

        /**
         * Deletes the specified directory.
         *
         * @private
         * @param {File} dir
         * @param {string} dirPath dir path
         * @param {function} onDeleteSuccess delete success callback
         * @param {function} onDeleteError delete error callback
         * @returns {boolean}
         */
        deleteDir: function SystemIO_deleteDir(
            dir,
            dirPath,
            onDeleteSuccess,
            onDeleteError
        ) {
            try {
                dir.deleteDirectory(
                    dirPath,
                    false,
                    onDeleteSuccess,
                    onDeleteError
                );
            } catch (e) {
                console.error('SystemIO_deleteDir error:' + e.message);
                return false;
            }

            return true;
        },

        /**
         * Obtains a list of storages and pass it to the specified callback
         * function.
         *
         * @public
         * @param {string} type Storage type.
         * @param {function} onSuccess Success callback.
         * @param {string} excluded Excluded storage.
         */
        getStorages: function SystemIO_getStorages(type, onSuccess, excluded) {
            try {
                tizen.filesystem.listStorages(function success(storages) {
                    var tmp = [],
                        len = storages.length,
                        i;

                    if (type !== undefined) {
                        for (i = 0; i < len; i += 1) {
                            if (storages[i].label !== excluded) {
                                if (
                                    storages[i].type === 0 ||
                                        storages[i].type === type
                                ) {
                                    tmp.push(storages[i]);
                                }
                            }
                        }
                    } else {
                        tmp = storages;
                    }

                    if (typeof onSuccess === 'function') {
                        onSuccess(tmp);
                    }
                });
            } catch (e) {
                console.error('SystemIO_getStorages error:' + e.message);
            }
        },

        /**
         * Obtains a list of files from the specified directory and pass it
         * to the specified callback function.
         *
         * @public
         * @param {File} dir
         * @param {function} onSuccess
         */
        getFilesList: function SystemIO_getFilesList(dir, onSuccess) {
            try {
                dir.listFiles(
                    function success(files) {
                        var tmp = [],
                            len = files.length,
                            i;

                        for (i = 0; i < len; i += 1) {
                            tmp.push({
                                name: files[i].name,
                                isDirectory: files[i].isDirectory
                            });
                        }

                        if (typeof onSuccess === 'function') {
                            onSuccess(tmp);
                        }
                    },
                    function error(e) {
                        console.error(
                            'SystemIO_getFilesList dir.listFiles() error:',
                            e
                        );
                    }
                );
            } catch (e) {
                console.error('SystemIO_getFilesList error:', e.message);
            }
        }
    };
}());
