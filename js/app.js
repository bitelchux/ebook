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

/*global tizen, setTimeout, app, Ui, Model, Helpers, Config, Clipboard*/
/**
 * App class constructor.
 *
 * @public
 * @constructor
 */
var App = function App() {
    'use strict';

    return;
};

(function start() { // strict mode wrapper
    'use strict';

    App.prototype = {

        /**
         * Required modules.
         *
         * @public
         * @const {string[]}
         */
        requires: [
            'js/app.config.js',
            'js/app.model.js',
            'js/app.ui.js',
            'js/app.ui.templateManager.js',
            'js/app.ui.templateManager.modifiers.js',
            'js/app.systemIO.js',
            'js/app.helpers.js',
            'js/app.clipboard.js'
        ],

        /**
         * Model instance.
         *
         * @public
         * @type {Model}
         */
        model: null,

        /**
         * Ui instance.
         *
         * @public
         * @type {Ui}
         */
        ui: null,

        /**
         * Config instance.
         *
         * @public
         * @type {Config}
         */
        config: null,

        /**
         * Helpers instance.
         *
         * @public
         * @type {Helpers}
         */
        helpers: null,

        /**
         * Current path.
         *
         * @public
         * @type {string}
         */
        currentPath: 'root',

        /**
         * Current directory handler.
         *
         * @private
         * @type {File}
         */
        currentDirHandle: null,

        /**
         * Clipboard instance.
         *
         * @public
         * @type {Clipboard}
         */
        clipboard: null,

        /**
         * Initializes the app.
         *
         * @public
         */
        init: function App_init() {
            this.config = new Config();
            this.model = new Model();
            this.ui = new Ui();
            this.helpers = new Helpers();
            this.clipboard = new Clipboard();

            this.initUi();
        },

        /**
         * Initializes UI.
         *
         * @private
         */
        initUi: function App_initUi() {
            this.ui.init(this.model.getInternalStorages());
        },

        /**
         * Displays media storages.
         *
         * @public
         */
        displayStorages: function App_displayStorages() {
            this.currentPath = '';
            if (!this.ui.editMode) {
                this.ui.scrollContentTo(0);
            }
            this.ui.displayStorages(this.model.getInternalStorages());
        },

        /**
         * Displays specified folder.
         *
         * @public
         * @param {string} path
         * @param {boolean} [refresh=false]
         */
        displayFolder: function App_displayFolder(path, refresh) {
            var self = this;

            refresh = refresh || false;

            // get folder data and push into rendering method
            this.model.getFolderData(path, function displayFolder(dir, nodes) {
                // on success

                // update current path
                self.currentPath = path;

                // update current dir handle
                self.currentDirHandle = dir;

                // display folder UI
                if (refresh === undefined) {
                    self.ui.scrollContentTo(0);
                }
                self.ui.displayFolder(path, nodes, refresh);
            });
        },

        /**
         * Opens specified file.
         *
         * @public
         * @param {string} fullUri File URI
         */
        openFile: function App_openFile(fullUri) {
     	      PDFJS.getDocument(fullUri).then(function(pdf) {
     	    	  pdfFile = pdf;
     	    	  pdfSelector(pdf);
     	    	  isFileOn = false; 
     	    	  tau.changePage("selectorPage",{
         			allowSamePageTransition: true
    	    	  });
    		},function(r){alert("Not supported file type") });
     	  
        	
        	/**
        	
            tizen.filesystem.resolve(
                fullUri,
                function success() {
                    this.model.openFile(fullUri);
                }.bind(this),
                function showAlert() {
                    // file doesn't exists
                    this.ui.alertPopup('File does no longer exist',
                               this.refreshCurrentPage.bind(this, true));
                }.bind(this)
            );**/
        },

        /**
         * Displays parent location.
         *
         * @public
         */
        goLevelUp: function App_goLevelUp() {
            // split current path and get proper path for parent location
            var newPath = this.currentPath.split('/').slice(0, -1).join('/');

            if (newPath !== '') {
                this.displayFolder(newPath);
            } else {
                this.displayStorages();
            }
        },

        /**
         * Creates new dir in currently viewed dir.
         *
         * @public
         * @param {string} dirName
         * @param {function} callback
         * @returns {boolean} return status
         */
        createDir: function App_createDir(dirName, callback) {
            var status = true;

            if (this.currentDirPath !== '') {
                try {
                    this.currentDirHandle.createDirectory(dirName);
                } catch (e) {
                    status = false;
                    app.ui.alertPopup(e.message, callback);
                }
                this.refreshCurrentPage();
            } else {
                status = false;
                app.ui.alertPopup(
                    'You can\'t create new nodes in the main view'
                );
            }
            return status;
        },

        /**
         * Triggers refresh current page.
         *
         * @public
         * @param {boolean} [refresh=false]
         */
        refreshCurrentPage: function App_refreshCurrentPage(refresh) {
            refresh = refresh || false;

            if (this.currentPath !== '') {

                app.model.isStorageExists(this.currentPath,
                    app.displayFolder.bind(app, app.model.currentPath, refresh),
                    function updateUi() {
                        app.ui.closeActivePopup();
                        app.displayStorages();
                        setTimeout(
                            function showAlertPopup() {
                                app.ui.alertPopup(
                                    'Path "' +
                                        app.model.currentPath +
                                        '" does no longer exist'
                                );
                            },
                            200
                        );
                    });
            } else {
                this.displayStorages();
            }
        },

        /**
         * Shows alert popup when delete error occurred.
         *
         * @private
         */
        onDeleteError: function App_onDeleteError() {
            app.ui.alertPopup(
                    'Error occured. Data has not been deleted.'
                );
            app.refreshCurrentPage();
        },

        /**
         * Deletes nodes with specified paths.
         *
         * @public
         * @param {string[]} nodes nodePaths
         */
        deleteNodes: function App_deleteNodes(nodes) {
            this.model.deleteNodes(
                nodes,
                this.ui.removeNodeFromList.bind(this.ui),
                this.onDeleteError
            );
        },

        /**
         * Saves the specified files to the clipboard.
         *
         * @public
         * @param {string[]} paths File paths.
         * @param {number} mode Clipboard mode.
         */
        saveToClipboard: function App_saveToClipboard(paths, mode) {
            var clipboardLength = this.clipboard.add(paths);

            if (clipboardLength > 0) {
                this.clipboard.setMode(mode);
                app.ui.alertPopup('Data saved in clipboard');
            } else {
                app.ui.alertPopup(
                    'Error occured. Data has not been saved in clipboard'
                );
            }

            this.ui.refreshPasteActionBtn(this.clipboard.isEmpty());
        },

        /**
         * Pastes nodes from clipboard to current dir.
         * Returns true if paste succeeded, false otherwise.
         *
         * @public
         * @returns {boolean}
         */
        pasteClipboard: function App_pasteClipboard() {
            var self = this,
                clipboardData = self.clipboard.get(),
                sourceDirPath = '';

            if (clipboardData.length === 0) {
                this.ui.closeInfoPopup();
                this.ui.alertPopup('Clipboard is empty');
                return false;
            }

            sourceDirPath = clipboardData[0].split('/')
                .slice(0, -1).join('/');
            self.model.isStorageExists(sourceDirPath,
                function copyAndMove(sourceDir) {

                    if (self.clipboard.getMode() === self
                            .clipboard.COPY_MODE_ID) {
                        self.model.copyNodes(
                            sourceDir,
                            self.currentDirHandle,
                            clipboardData,
                            self.currentPath,
                            self.onPasteClipboardSuccess.bind(self)
                        );
                    } else {
                        self.model.moveNodes(
                            sourceDir,
                            self.currentDirHandle,
                            clipboardData,
                            self.currentPath,
                            self.onPasteClipboardSuccess.bind(self)
                        );
                    }

                }, function error() {
                    app.ui.alertPopup('Unable to perform operation.');
                });
            this.ui.refreshPasteActionBtn(this.clipboard.isEmpty());
            return true;
        },

        /**
         * Returns true if the clipboard is empty, false otherwise.
         *
         * @public
         * @returns {boolean}
         */
        emptyClipboard: function App_emptyClipboard() {
            return this.clipboard.get().length === 0;
        },

        /**
         * Handler for paste clipboard success.
         *
         * @private
         */
        onPasteClipboardSuccess: function App_onPasteClipboardSuccess() {
            this.clipboard.clear();
            this.ui.closeInfoPopup();
            this.refreshCurrentPage();
        },

        /**
         * Exits the app.
         *
         * @private
         */
        exit: function App_exit() {
            tizen.application.getCurrentApplication().exit();
        }
    };
}());
