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

/*global tizen, SystemIO, app, setTimeout, console, alert*/

/**
 * Model class constructor.
 *
 * @public
 * @constructor
 */
function Model() {
    'use strict';

    this.init();
}

(function start() { // strict mode wrapper
    'use strict';

    Model.prototype = {

        /**
         * Open file unlocked flag.
         *
         * @private
         * @type {boolean}
         */
        openFileUnLocked: true,

        /**
         * SystemIO instance.
         *
         * @public
         * @type {SystemIO}
         */
        systemIO: null,

        /**
         * List of storage descriptors.
         *
         * @private
         * @type {object[]}
         */
        storages: [{label: 'root', type: 'INTERNAL'}],

        /**
         * Current path.
         *
         * @public
         * @type {string}
         */
        currentPath: '',

        /**
         * Error message.
         *
         * @private
         * @const {string}
         */
        ERR_MSG_APPCONTROL_PKG_NOT_FOUND:
            'There is no suitable application to open this file. ' +
            'Please install application that handles this kind of files',

        /**
         * Initializes model.
         *
         * @public
         */
        init: function Model_init() {
            this.systemIO = new SystemIO();
        },

        /**
         * Returns storages.
         *
         * @public
         * @returns {FileSystemStorage[]} storages
         */
        getInternalStorages: function Model_getInternalStorages() {
            return this.storages;
        },

        /**
         * Saves storages.
         *
         * @public
         * @param {function} onSuccess Success callback.
         */
        loadInternalStorages: function Model_loadInternalStorages(onSuccess) {
            var self = this;

            this.systemIO.getStorages('INTERNAL', function success(storages) {
                self.storages = storages;
                if (typeof onSuccess === 'function') {
                    onSuccess();
                }
            }, 'internal0');
        },

        /**
         * Returns folder data.
         *
         * @public
         * @param {string} path Node path.
         * @param {function} onSuccess Success callback.
         */
        getFolderData: function Model_getFolderData(path, onSuccess) {
            var self = this,
                onOpenSuccess = function onOpenSuccess(dir) {
                    dir.listFiles(
                        function success(files) {
                            self.currentPath = dir.fullPath;
                            onSuccess(dir, files);
                        },
                        function error(e) {
                            console.error(
                                'Model_getFolderData listFiles error',
                                e
                            );
                        }
                    );
                },
                onOpenError = function onOpenError(e) {
                    console.error('Model_getFolderData openDir error', e);
                };

            this.systemIO.openDir(path, onOpenSuccess, onOpenError);
        },

        /**
         * Checks if the specified storage exists or not and passes this
         * boolean information to the specified callback function.
         *
         * @public
         * @param {string} nodeName
         * @param {function} success
         * @param {function} error
         */
        isStorageExists: function isStorageExists(nodeName, success, error) {
            tizen.filesystem.resolve(nodeName, success, error);
        },

        /**
         * Launches a service to open the file.
         *
         * @public
         * @param {string} fullUri ext
         */
        openFile: function Model_openFile(fullUri) {
            var self = this,
                serviceReplyCB = {};

            if (this.openFileUnLocked) {
                serviceReplyCB = {
                    onsuccess: function onsuccess() {
                        self.openFileUnLocked = true;
                    },
                    onfailure: function onfailure() {
                        self.openFileUnLocked = true;
                        console.error('Launch service failed');
                    }
                };
                this.openFileUnLocked = false;
                try {
                    tizen.application.launchAppControl(
                        new tizen.ApplicationControl(
                            'http://tizen.org/appcontrol/operation/view',
                            fullUri
                        ),
                        null,
                        function success() {
                            setTimeout(function success() {
                                self.openFileUnLocked = true;
                            }, 500);
                        },
                        function error(e) {
                            self.openFileUnLocked = true;
                            console.error(e,
                                'Service launch failed. Exception message:' +
                                    e.message
                                );

                            if (e.type === 'NotFoundError') {
                                alert(self.ERR_MSG_APPCONTROL_PKG_NOT_FOUND);
                            }
                        },
                        serviceReplyCB
                    );
                } catch (e) {
                    self.openFileUnLocked = true;
                    console.error('openFile failed', e);
                }
            }
        },

        /**
         * Notifies the Media Content that the specified file could have been
         * added.
         *
         * @public
         * @param {string} path
         * @param {function} successCallback
         * @param {string} mode
         */
        refreshContent: function refreshContent(path, successCallback, mode) {
            var errorCallback = function errorCallback(error) {
                console.error('Error: ', error);
            };

            successCallback = successCallback || null;
            mode = mode || 'copy';

            if (mode === 'copy') {
                tizen.filesystem.resolve(path, function success(file) {
                    if (file.isFile) {
                        tizen.content.scanFile(file.toURI(), successCallback,
                            errorCallback);
                    } else {
                        successCallback();
                    }
                }, errorCallback);
            } else {
                successCallback();
            }
        },

        /**
         * Resolves the specified file path and refreshes the Media Content.
         *
         * @public
         * @param {string} path
         * @param {function} successCallback
         */
        resolveAndRefresh: function resolveAndRefresh(path, successCallback) {
            var self = this;

            tizen.filesystem.resolve(path, function success(file) {
                self.refreshContent(file.toURI(), successCallback);
            }, null);
        },

        /**
         * Deletes the specified nodes from the filesystem.
         *
         * @public
         * @param {File[]} nodes Collection of node objects.
         * @param {function} onSuccess
         * @param {function} onError
         */
        deleteNodes: function Model_deleteNodes(
            nodes,
            onSuccess,
            onError
        ) {
            var len = nodes.length,
                i = 0,
                onDeleteNodeSuccess = function onDeleteNodeSuccess(file) {
                    try {
                        app.clipboard.removeRecursively(file.fullPath);
                        app.ui.refreshPasteActionBtn();
                        if (onSuccess instanceof Function) {
                            onSuccess(file.fullPath);
                        }
                    } catch (e) {
                        console.error(e);
                    }
                },
                onDeleteNodeError = function onDeleteNodeError(error) {
                    if (typeof onError === 'function') {
                        onError();
                    } else {
                        console.error(error);
                    }
                },
                remove = function remove(file, success, failure) {
                    var fullPath = file.fullPath,
                        removeOnSuccess = function removeOnSuccess() {
                            if (success instanceof Function) {
                                tizen.content.scanFile(file.toURI());
                                success(file);
                            }
                        },
                        removeOnFailure = function removeOnFailure(error) {
                            if (failure instanceof Function) {
                                failure(error, file);
                            } else {
                                console.error(error);
                            }
                        };

                    tizen.filesystem.resolve(
                        file.parent.fullPath,
                        function onParentResolve(parent) {

                            if (file.isFile) {
                                parent.deleteFile(
                                    fullPath,
                                    removeOnSuccess,
                                    removeOnFailure
                                );
                            } else {
                                file.listFiles(
                                    function success(files) {
                                        var filesLength = files.length,
                                            index = filesLength - 1,
                                            counter = 0,
                                            removeEmptyDir =
                                                function removeEmptyDir() {
                                                    parent.deleteDirectory(
                                                        fullPath,
                                                        false,
                                                        removeOnSuccess,
                                                        removeOnFailure
                                                    );
                                                },
                                            removeSuccess =
                                                function removeSuccess() {
                                                    counter += 1;
                                                    if (counter ===
                                                            filesLength) {
                                                        removeEmptyDir();
                                                    }
                                                };

                                        if (filesLength > 0) {
                                            while (index >= 0) {
                                                remove(
                                                    files[index],
                                                    removeSuccess,
                                                    removeOnFailure
                                                );
                                                index -= 1;
                                            }
                                        } else {
                                            removeEmptyDir();
                                        }
                                    },
                                    removeOnFailure
                                );
                            }
                        },
                        removeOnFailure
                    );
                }, // end remove

                resolveSuccess = function resolveSuccess(file) {
                    remove(file, onDeleteNodeSuccess, onDeleteNodeError);
                };

            for (i = 0; i < len; i = i + 1) {
                tizen.filesystem.resolve(
                    nodes[i].uri,
                    resolveSuccess.bind(this),
                    null
                );
            }
        },

        /**
         * Copies specified files to the destination path.
         * Overwrites existing files.
         *
         * @public
         * @param {File} sourceDir Source directory handle.
         * @param {File} destinationDir Destination directory handle.
         * @param {string[]} paths Array with absolute virtual file paths.
         * @param {string} destinationPath Destination path.
         * @param {function} onSuccess Success callback.
         */
        copyNodes: function Model_copyNodes(
            sourceDir,
            destinationDir,
            paths,
            destinationPath,
            onSuccess
        ) {
            var len = paths.length,
                self = this,
                scanned = 0,
                scanSuccess = function scanSuccess() {
                    scanned += 1;
                    if (scanned === len) {
                        onSuccess();
                    }
                },
                onCopyNodeSuccess = function onCopyNodeSuccess(filePath) {
                    self.refreshContent(filePath, scanSuccess, 'copy');
                },
                onCopyNodeFailure = function onCopyNodeFailure(e) {
                    console.error(e);
                    setTimeout(function refresh() {
                        app.refreshCurrentPage();
                        app.ui.alertPopup('Copying error');
                    }, 200);
                },
                i,
                sourceName;

            this.systemIO.getFilesList(destinationDir,
                function success(filesList) {
                    var showAlertPopup = function showAlertPopup() {
                            app.ui.alertPopup('Copying error');
                        },
                        finalPath = null;

                    for (i = 0; i < len; i = i + 1) {
                        if ((destinationPath + '/')
                                .indexOf(paths[i] + '/') !== -1) {
                            setTimeout(showAlertPopup, 200);
                            return;
                        }
                    }

                    for (i = 0; i < len; i = i + 1) {
                        sourceName = paths[i].split('/').pop();
                        sourceName = app.helpers.getCopyFileName(
                            sourceName,
                            filesList
                        );
                        finalPath = destinationPath + '/' + sourceName;

                        try {
                            sourceDir.copyTo(
                                paths[i],
                                destinationPath + '/' + sourceName,
                                true,
                                onCopyNodeSuccess.bind(self, finalPath),
                                onCopyNodeFailure
                            );
                        } catch (e) {
                            console.error(e);
                        }
                    }
                });
        },

        /**
         * Moves specified files to the destination path.
         * Overwrites existing files.
         *
         * @public
         * @param {File} sourceDir Source directory handle.
         * @param {File} destinationDir Destination directory handle.
         * @param {string[]} paths Array with absolute virtual file paths.
         * @param {string} destinationPath Destination path.
         * @param {function} onSuccess Success callback.
         */
        moveNodes: function Model_moveNodes(
            sourceDir,
            destinationDir,
            paths,
            destinationPath,
            onSuccess
        ) {
            var len = paths.length,
                self = this,
                scanned = 0,
                toScan = len * 2,
                illegalMove = false,
                scanSuccess = function scanSuccess() {
                    scanned += 1;
                    if (scanned === toScan) {
                        onSuccess();
                    }
                },
                onMoveNodeSuccess =
                    function onMoveNodeSuccess(oldfile, newFilePath) {
                        self.refreshContent(oldfile.toURI(),
                            scanSuccess, 'delete');
                        self.refreshContent(newFilePath, scanSuccess, 'copy');
                    },
                onMoveNodeFailure = function onMoveNodeFailure(error) {
                    console.error('Moving error: ', error);
                    app.ui.alertPopup('Moving error');
                },
                i = 0,
                sourceName = '';

            len -= 1;
            while (len >= 0) {
                if (destinationPath.match(paths[len])) {
                    illegalMove = true;
                    break;
                }
                len -= 1;
            }

            len = paths.length;

            if (illegalMove) {
                setTimeout(function success() {
                    app.ui.alertPopup('You cannot move folder into itself.');
                }, 200);
                return;
            }

            this.systemIO.getFilesList(destinationDir, function success() {
                var resolveSuccess = function resolveSuccess(
                    path,
                    destinationPath,
                    sourceName,
                    oldfile
                ) {
                    var finalPath = destinationPath + '/' + sourceName;

                    sourceDir.moveTo(
                        path,
                        finalPath,
                        false,
                        onMoveNodeSuccess.bind(self, oldfile, finalPath),
                        onMoveNodeFailure
                    );
                };
                for (i = 0; i < len; i = i + 1) {
                    if ((destinationPath + '/')
                            .indexOf(paths[i] + '/') !== -1) {
                        app.ui.alertPopup('Moving error');
                        return;
                    }
                }

                for (i = 0; i < len; i = i + 1) {
                    sourceName = paths[i].split('/').pop();
                    try {
                        tizen.filesystem.resolve(
                            paths[i],
                            resolveSuccess.bind(
                                self,
                                paths[i],
                                destinationPath,
                                sourceName
                            ),
                            onMoveNodeFailure
                        );
                    } catch (e) {
                        console.error(e);
                    }
                }
            });
        }
    };
}());
