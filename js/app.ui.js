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

/*global window, document, tizen, $, app, TemplateManager, Helpers, tau */

/**
 * UI class constructor.
 *
 * @public
 * @constructor
 */
function Ui() {
    'use strict';

    return;
}

(function start() { // strict mode wrapper
    'use strict';

    Ui.prototype = {
        /**
         * Root mode.
         *
         * @private
         * @type {boolean}
         */
        root: true,

        /**
         * Locked folders.
         *
         * @private
         * @type {string[]}
         */
        lockedFolders: ['ringtones'],

        /**
         * UI edit mode.
         *
         * @public
         * @type {boolean}
         */
        editMode: false,

        /**
         * TemplateManager instance.
         *
         * @public
         * @type {TemplateManager}
         */
        templateManager: null,

        /**
         * Helpers instance.
         *
         * @private
         * @type {Helpers}
         */
        helpers: null,

        /**
         * Name of row gradient class.
         *
         * @private
         * @const {string}
         */
        CSS_GRADIENT_CLASS: 'gradientBackground',

        /**
         * Delete button selector.
         *
         * @private
         * @const {string}
         */
        DELETE_BTN: '#deleteActionBtn',

        /**
         * Move button selector.
         *
         * @private
         * @const {string}
         */
        MOVE_BTN: '#moveActionBtn',

        /**
         * Copy button selector.
         *
         * @private
         * @const {string}
         */
        COPY_BTN: '#copyActionBtn',

        /**
         * Cancel button selector.
         *
         * @private
         * @const {string}
         */
        CANCEL_BTN: '#cancelActionBtn',

        /**
         * Current header height.
         *
         * @private
         * @type {number}
         */
        currentHeaderHeight: null,

        /**
         * Current scroll position.
         *
         * @private
         * @type {number}
         */
        currentScrollPosition: null,

        /**
         * Initializes the UI class.
         *
         * @public
         * @param {FileSystemStorage[]} storages
         */
        init: function Ui_init(storages) {
            this.templateManager = new TemplateManager();
            this.helpers = new Helpers();
            // Disable text selection
            $.mobile.tizen.disableSelection(document);
            $(document).ready(this.initDom.bind(this, storages));
        },

        /**
         * Initializes DOM.
         *
         * @private
         * @param {FileSystemStorage[]} storages
         */
        initDom: function Ui_initDom(storages) {
            var self = this;

            this.templateManager.loadToCache(
                [
                    'fileRow',
                    'folderRow',
                    'levelUpRow',
                    'emptyFolder'
                ],
                function success() {
                    self.addEvents();
                    self.displayStorages(storages);
                }
            );

            $('#infoPopup').popup();
        },

        /**
         * Registers UI events.
         *
         * @private
         */
        addEvents: function Ui_addEvents() {
            var self = this;

            document.addEventListener('webkitvisibilitychange',
                function onWebkitVisibilityChange() {
                    if (document.webkitVisibilityState === 'visible') {
                        if (self.isOperationInProgress()) {
                            return;
                        }
                        self.refreshSelectAllStatus();
                        app.refreshCurrentPage(true);
                    }
                });

            window.addEventListener('tizenhwkey', function onTizenhwkey(e) {
                var uri = $('#navbar span+span').attr('uri');

                if (e.keyName === 'back') {
                    if (self.isOperationInProgress()) {
                        return;
                    }
                    if (tau.engine.getRouter().hasActivePopup()) {
                        window.history.back();
                    } else if (self.editMode === true) {
                        self.handleCancelEditAction();
                    } else if (!uri) {
                        if (app.ui.root === false) {
                            app.ui.root = true;
                            $('#fileList').empty();
                            app.ui.appendFolderRow(0, 'root');
                            app.currentPath = 'root';
                        } else {
                            tizen.application.getCurrentApplication().exit();
                        }
                    } else if (!app.ui.root) {
                        app.goLevelUp();
                    }
                }
            });

            $('#newFolderBtn').click(function onClick() {
                tau.widget.Popup(document.getElementById('addFolderPopup'))
                    .open();
            });

            $(window).resize(function onResize() {
                $.mobile.activePage.page('refresh');
            });

            // touch events for all nodes
            $('ul#fileList')
                .on('click', 'li.levelUp', function onClick() {
                    if (self.editMode === true) {
                        self.handleCancelEditAction();
                    }
                    app.goLevelUp();
                })
                .on('click', 'li.node', function onClick() {
                    self.handleNodeClick($(this), true);
                })
                .on('change', 'input[type=checkbox]', function onChange(e) {
                    e.stopPropagation();
                    e.preventDefault();
                    self.handleNodeClick($(this).closest('li.node'), true);
                });

            $('.selectAll input').on(
                'change',
                this.handleSelectAllChange.bind(this)
            );

            // navbar
            $('#navbar').on('click', 'span', function onClick() {
                var uri = $(this).attr('uri');

                if (uri === 'home' && app.currentPath !== '') {
                    app.displayStorages();
                } else if (uri === app.model.currentPath) {
                    app.displayFolder(uri, true);
                } else {
                    if (self.editMode === true) {
                        self.handleCancelEditAction();
                    }
                    app.displayFolder(uri);
                }
            });

            // level up
            $('#levelUpBtn').on('click', function onClick() {
                if (self.editMode === true) {
                    self.handleCancelEditAction();
                }
                app.goLevelUp();
            });

            $('#homeBtn').on('click', app.displayStorages.bind(app));

            // edit action
            $('#editActionBtn').on('click', this.handleEditAction.bind(this));

            // delete action
            $('#deleteActionBtn').on(
                'click',
                this.handleDeleteAction.bind(this)
            );

            // copy action
            $('#copyActionBtn').on('click', this.handleCopyAction.bind(this));

            // move action
            $('#moveActionBtn').on('click', this.handleMoveAction.bind(this));

            // paste action
            $('a#pasteActionBtn').on('click', function onClick() {
                $('#morePopup').popup('close');
                $('#infoPopup')
                    .popup('open')
                    .one('popupafteropen', function afterOpen() {
                        app.pasteClipboard();
                    });
            });

            // remove active class
            $('[data-role = "tabbar"] li > a').on('click', function onClick() {
                $(this).removeClass('ui-focus, ui-btn-active');
            });
            // exit
            $('#exitBtn').on('click', app.exit);

            // add folder popup actions
            $('#addFolderPopup').on('popupafterclose',
                function onPopupAfterClose() {
                    // clear input value
                    $('#newFolderName').val('New folder');
                });

            $('#saveNewFolder').on('click', this.saveNewFolder.bind(this));
            $('#newFolderForm').on('submit', this.saveNewFolder.bind(this));
        },

        /**
         * Saves new folder.
         * Returns true if saving folder succeeded, false otherwise.
         *
         * @private
         * @param {Event} e
         * @returns {boolean}
         */
        saveNewFolder: function Ui_saveNewFolder(e) {
            var folderName = $('#newFolderName').val(),
                status = true,
                open = function open() {
                    $('#addFolderPopup').popup('open', {
                        positionTo: 'window'
                    });
                },
                self = this,
                aPopup;

            e.preventDefault();
            e.stopPropagation();

            $('#addFolderPopup').one('popupafterclose',
                function onPopupAfterClose() {
                    if (folderName === '') {
                        self.alertPopup('Empty folder name', open);
                        status = false;
                    } else if (folderName.match(/[\*\.\/\\\?\"\'\:<>|]/)) {
                        self.alertPopup('The following special characters ' +
                            'are not allowed: *./\\?:<>|\'"', open);
                        status = false;
                    } else {
                        status = app.createDir(folderName, open);
                    }
                });
            aPopup = $.mobile.popup.active;
            if (
                aPopup && aPopup.element.getAttribute('id') === 'addFolderPopup'
            ) {
                aPopup.close();
            }
            return status;
        },

        /**
         * Returns if some operation (i.e. copying/moving nodes) is in progress.
         * This is detected by infoPopup visibility.
         *
         * @private
         * @returns {boolean}
         */
        isOperationInProgress: function Ui_isOperationInProgress() {
            var activePopup = $.mobile.popup.active;

            return activePopup && activePopup.id === 'infoPopup';
        },

        /**
         * Closes popup with information about operations in progress.
         *
         * @public
         */
        closeInfoPopup: function Ui_closeInfoPopup() {
            $('#infoPopup').popup('close');
        },

        /**
         * Closes active popup.
         *
         * @public
         */
        closeActivePopup: function Ui_closeActivePopup() {
            var popup = $.mobile.popup.active;

            if (popup) {
                popup.close();
            }
        },

        /**
         * Displays alert popup.
         *
         * @public
         * @param {string} text Popup message text.
         * @param {function} [callback] Callback executed when popup is
         * being closed.
         */
        alertPopup: function Ui_alertPopup(text, callback) {
            $('#alertPopup .text').text(text);
            $('#infoPopup').popup('close');
            $('#alertPopup').popup('open', {'positionTo': 'window'});
            if (callback instanceof Function) {
                $('#alertPopup').one('popupafterclose',
                    function onPopupafterclose() {
                        callback();
                    });
            }
        },

        /**
         * Displays confirm popup.
         *
         * @public
         * @param {string} text Popup message text.
         * @param {function} [confirmCallback] Callback executed when the
         * confirm button on the popup is being clicked.
         * @param {function} [completeCallback] Callback executed when the
         * popup is being closed.
         */
        confirmPopup: function Ui_confirmPopup(text, confirmCallback,
                                               completeCallback) {
            var popup = $('#confirmPopup');

            popup.find('.text').text(text);
            popup.popup('open', {positionTo: 'window'});
            popup.find('.confirm').one('vclick', function onClick() {
                if (confirmCallback instanceof Function) {
                    confirmCallback();
                }
            });
            if (completeCallback instanceof Function) {
                popup.one('popupafterclose', completeCallback);
            }
        },

        /**
         * Handles node click events.
         *
         * @private
         * @param {File} node
         * @param {boolean} toggleCheckbox
         */
        handleNodeClick: function Ui_handleNodeClick(node, toggleCheckbox) {
            if (this.root) {
                app.model.loadInternalStorages(function success() {
                    app.displayStorages();
                });
                this.root = false;
            } else if (this.editMode === true) {
                if (toggleCheckbox === true) {
                    this.toggleCheckBoxState(node); // select the checkbox
                }

                this.refreshSelectAllStatus();
                this.refreshEditMenu();
            } else if (node.hasClass('folder')) {
                // otherwise display folder
                app.displayFolder(node.attr('uri'));
            } else {
                // file
                app.openFile(node.attr('fullUri'));
            }
        },

        /**
         * Handles edit action.
         *
         * @public
         */
        handleEditAction: function Ui_handleEditAction() {
            this.editMode = true;
            $('.standard-bar-btn').addClass('hidden');
            $('.edit-bar-btn').removeClass('hidden');
            this.disableControlBarButtons(
                [
                    this.DELETE_BTN,
                    this.COPY_BTN,
                    this.MOVE_BTN
                ]
            );
            $('#fileList .folder .nodename, #fileList > li > span.nodename')
                .animate({'width': '70%'});
            this.showEditCheckBoxes();
        },

        /**
         * Handles cancel edit action.
         *
         * @private
         * @param {boolean} emptyList
         */
        handleCancelEditAction: function Ui_handleCancelEditAction(emptyList) {
            this.editMode = false;
            $('.edit-bar-btn').addClass('hidden');
            $('.standard-bar-btn').removeClass('hidden');
            this.hideEditCheckBoxes();
            if (this.isFileListEmpty() || emptyList) {
                $('#editActionBtn').addClass('vhidden').blur();
            }
        },

        /**
         * Handles delete action.
         *
         * @private
         * @param {Error} e
         */
        handleDeleteAction: function Ui_handleDeleteAction(e) {
            var nodesToDelete = [],
                $rowElement,
                self = this,
                listLength = $('ul#fileList li.node').length;

            e.stopPropagation();
            e.preventDefault();

            this.confirmPopup('Selected nodes will be deleted. Are you sure?',
                function success() {
                    $('ul#fileList input:checkbox:checked')
                        .each(function pushElements() {
                            $rowElement = $(this).closest('li');
                            nodesToDelete.push({
                                id: $rowElement.attr('id'),
                                uri: $rowElement.attr('uri'),
                                name: $rowElement.attr('label'),
                                folder: $rowElement.hasClass('folder')
                            });
                        });

                    if (nodesToDelete.length > 0) {
                        app.deleteNodes(nodesToDelete);
                        self.scrollContentTo(0);
                        $('ul#fileList input:checkbox:checked').remove();
                        if (nodesToDelete.length === listLength) {
                            self.handleCancelEditAction(true);
                        } else {
                            self.refreshEditMenu();
                        }
                    }
                }
            );
        },

        /**
         * Handles copy action.
         *
         * @private
         * @param {Error} e
         */
        handleCopyAction: function Ui_handleCopyAction(e) {
            var paths = [];

            e.stopPropagation();
            e.preventDefault();

            if (this.editMode === true) {
                $('ul#fileList input:checkbox:checked').each(
                    function pushElements() {
                        paths.push($(this).closest('li').attr('uri'));
                    });
                app.saveToClipboard(paths, app.clipboard.COPY_MODE_ID);
            }
        },

        /**
         * Handles move action.
         *
         * @private
         * @param {Error} e
         */
        handleMoveAction: function Ui_handleMoveAction(e) {
            var paths = [];

            e.stopPropagation();
            e.preventDefault();

            if (this.editMode === true) {
                $('ul#fileList input:checkbox:checked').each(
                    function pushElements() {
                        paths.push($(this).closest('li').attr('uri'));
                    }
                );
                app.saveToClipboard(paths, app.clipboard.MOVE_MODE_ID);
            }
        },

        /**
         * Scrolls content to the specified position.
         *
         * @public
         * @param {number} value
         */
        scrollContentTo: function scrollContentTo(value) {
            $('#main [data-role="content"]').scrollview('scrollTo', 0, value);
        },

        /**
         * Displays the specified storages.
         *
         * @public
         * @param {FileSystemStorage[]} nodes Storage elements.
         */
        displayStorages: function Ui_displayStorages(nodes) {
            var len = nodes.length, nodeName, i;

            this.updateNavbar('');
            $('#fileList').empty();

            for (i = 0; i < len; i = i + 1) {
                nodeName = nodes[i].label;
                if (nodeName !== '' &&
                    (nodes[i].type === 0 || nodes[i].type === 'INTERNAL') &&
                    nodeName.indexOf('wgt-') === -1 &&
                    $.inArray(nodeName, this.lockedFolders) === -1) {
                    this.appendFolderRow(i, nodeName);
                }
            }

            $('#levelUpBtn, #homeBtn').addClass('vhidden');
            $('#editActionBtn, #moreActionBtn').addClass('vhidden').blur();

            $('h1#mainTitle').html('Media');

            if (this.editMode) {
                this.scrollContentTo(0);
            }

            this.hideSelectAllArea();
            this.handleCancelEditAction();
        },

        /**
         * Adds the specified file to the files list.
         *
         * @private
         * @param {number} id ID of the file.
         * @param {string} name Full URI of the given file.
         */
        appendFolderRow: function appendFolderRow(id, name) {
            $(this.templateManager.get('folderRow', {
                id: id,
                name: name,
                uri: name,
                fullUri: name
            })).appendTo('#fileList');
        },

        /**
         * Renders node list for folder.
         *
         * @public
         * @param {string} folderName
         * @param {File[]} nodes
         * @param {bool} [refresh=false]
         */
        /*jshint maxcomplexity:15, maxstatements:49 */
        displayFolder: function Ui_displayFolder(folderName, nodes, refresh) {
            var len = nodes.length,
                listElements = [this.templateManager.get('levelUpRow')],
                nodeName = '',
                checkedRows = [],
                checkedRowsLen = 0,
                i = 0;

            refresh = refresh || false;

            // update title
            this.updateTitle(
                this.templateManager.modifiers.escape(folderName)
            );
            // update navbar
            this.updateNavbar(
                this.templateManager.modifiers.escape(folderName)
            );
            this.refreshPasteActionBtn();

            nodes.sort(this.helpers.fileComparison);

            // render nodes
            for (i = 0; i < len; i = i + 1) {
                nodeName = nodes[i].name;
                if (nodeName !== '') {
                    if (nodes[i].isDirectory) {
                        // folder
                        listElements.push(this.templateManager.get(
                            'folderRow',
                            {
                                id: i,
                                name: nodeName,
                                uri: nodes[i].fullPath,
                                fullUri: nodes[i].toURI()
                            }
                        ));
                    } else {
                        // file
                        listElements.push(this.templateManager.get(
                            'fileRow',
                            {
                                id: i,
                                name: nodeName,
                                uri: nodes[i].fullPath,
                                fullUri: nodes[i].toURI(),
                                thumbnailURI: this.helpers.getThumbnailURI(
                                    nodeName,
                                    nodes[i]
                                )
                            }
                        ));
                    }
                }
            }

            if (listElements.length === 1) {
                // set content for empty folder
                listElements.push(this.templateManager.get('emptyFolder'));
                // hide edit button for empty content
                $('#editActionBtn').addClass('vhidden').blur();
                this.handleCancelEditAction();
            } else {
                $('#editActionBtn').removeClass('vhidden');
            }

            // scroll to top of list
            this.scrollContentTo(0);

            $('#levelUpBtn').removeClass('vhidden');
            $('#homeBtn').removeClass('vhidden');
            $('#moreActionBtn').removeClass('vhidden');

            if (refresh === true && this.editMode === true) {
                $.each($('#fileList .ui-checkbox input:checked'),
                    function pushElements() {
                        checkedRows.push($(this).closest('li').attr('uri'));
                    });
            }

            // update file list
            $('#fileList').html(listElements.join(''))
                .trigger('refresh')
                .trigger('create');
        	
             //   $('.selectAll').hide();
                $('#fileList .folder .nodename, #fileList > li > span.nodename')
                    .css({"background-color":"yellow","height":"10px"});
                $('ul#fileList > li').css({"margin":"auto","height":"10px"});
                $('.my-ui-checkbox').addClass('hidden');
                this.clearDeleteMode();
            
/*
            if (this.editMode === true) {
                $('.selectAll').show();
                $('#fileList .folder .nodename, #fileList > li > span.nodename')
                    .css('width', '70%');
                $('.my-ui-checkbox').removeClass('hidden');
                if (refresh === true) {
                    checkedRowsLen = checkedRows.length;
                    if (checkedRowsLen) {
                        if (
                            checkedRowsLen !==
                            $('#fileList .ui-checkbox input').length
                        ) {
                            this.setCheckboxValue('.selectAll input', false);
                        }
                        // restore checked checkboxes
                        for (i = 0; i < checkedRowsLen; i += 1) {
                            this.setCheckboxValue(
                                '#' +
                                $('#fileList [uri="' +
                                    checkedRows[i] +
                                    '"]').attr('id') +
                                ' input:checkbox',
                                'checked'
                            );
                        }

                        // if there are no checked checkboxes
                        if (!$('#fileList .ui-checkbox input:checked').length) {
                            this.clearDeleteMode();
                        }
                    } else {
                        this.clearDeleteMode();
                    }
                }
            } else
  */ 	

            if (!refresh) {
                this.hideSelectAllArea();
            }

            this.refreshSelectAllStatus();
        },

        /**
         * Clears confirm popup and disables action buttons.
         *
         * @private
         */
        clearDeleteMode: function Ui_clearDeleteMode() {
            var aPopup = $.mobile.popup.active;

            if (aPopup && aPopup.element.getAttribute('id') ===
                'confirmPopup') {
                $.mobile.popup.active.close();
            }
            this.disableControlBarButtons(
                [
                    this.DELETE_BTN,
                    this.COPY_BTN,
                    this.MOVE_BTN
                ]
            );
        },

        /**
         * Toggles a checkbox associated with a given list element.
         *
         * @private
         * @param {jQuery} listElement
         */
        toggleCheckBoxState: function Ui_toggleCheckBoxState(listElement) {
            var checkboxInput = null;

            checkboxInput = listElement.find('.ui-checkbox');
            this.setCheckboxValue(
                checkboxInput,
                !checkboxInput.prop('checked')
            );
        },

        /**
         * Shows item checkboxes and topbar with select all option.
         *
         * @private
         */
        showEditCheckBoxes: function Ui_showEditCheckBoxes() {
            var self = this;
            this.showSelectAllArea();
            self.editMode = true;
            $('.my-ui-checkbox').removeClass('hidden');
        },

        /**
         * Hides item checkboxes and topbar with select all option.
         * All checkboxes are auto unchecked.
         *
         * @private
         */
        hideEditCheckBoxes: function Ui_hideEditCheckBoxes() {
            var self = this;

            this.hideSelectAllArea(); // hide select all option topbar

            $('.my-ui-checkbox').addClass('hidden');
            $.mobile.activePage.page('refresh');

            // uncheck all checkboxes
            $('ul#fileList input[type=checkbox]').each(
                function setCheckboxValue() {
                    self.setCheckboxValue(this, false);
                }
            );

            this.setCheckboxValue('.selectAll input', false);
        },

        /**
         * Saves current header and content height.
         *
         * @private
         */
        saveHeights: function Ui_saveHeights() {
            this.currentHeaderHeight = $('#main div[data-role="header"]')
                .height();
            this.currentScrollPosition = $('#main div[data-role="content"]')
                .scrollview('getScrollPosition').y;
        },

        /**
         * Changes content scroll position after showing/hiding selectAllArea.
         *
         * @private
         */
        changeContentScrollPosition: function Ui_changeContentScrollPosition() {
            var diff,
                contentHeight;
            if (this.currentScrollPosition !== 0) {
                contentHeight = $('#main div[data-role="header"]').height();
                diff = contentHeight - this.currentHeaderHeight;
                $('#main div[data-role="content"]')
                    .scrollview(
                    'scrollTo',
                    0,
                    -(this.currentScrollPosition + diff)
                );
            }
        },

        /**
         * Shows topbar with select all option.
         *
         * @private
         */
        showSelectAllArea: function Ui_showSelectAllArea() {
            this.saveHeights();
            $('.selectAll').show();
            $.mobile.activePage.page('refresh');
            this.changeContentScrollPosition();
        },

        /**
         * Hides topbar with select all option.
         *
         * @private
         */
        hideSelectAllArea: function Ui_hideSelectAllArea() {
            this.saveHeights();
            $('.selectAll').hide();
            $.mobile.activePage.page('refresh');
            this.changeContentScrollPosition();
        },

        /**
         * Enables the specified footer buttons.
         *
         * @private
         * @param {string[]} buttons Button selectors.
         */
        enableControlBarButtons: function Ui_enableControlBarButtons(buttons) {
            var i = 0,
                len = buttons.length;

            for (i = 0; i < len; i += 1) {
                $(buttons[i]).prop('disabled', false);
            }
        },

        /**
         * Disables the specified footer buttons.
         *
         * @private
         * @param {string[]} buttons Button selectors.
         */
        disableControlBarButtons:
            function Ui_disableControlBarButtons(buttons) {
                var i = 0,
                    len = buttons.length;

                for (i = 0; i < len; i += 1) {
                    $(buttons[i]).prop('disabled', true);
                }
            },

        /**
         * Updates title.
         *
         * @private
         * @param {string} path
         */
        updateTitle: function Ui_updateTitle(path) {
            var regexp = new RegExp('([^\/])+$', 'g'),
                match = path.match(regexp),
                lastDir = match[0] || '(dir)';
            $('h1#mainTitle').html(lastDir);
        },

        /**
         * Updates navbar.
         *
         * @private
         * @param {string} path
         */
        updateNavbar: function Ui_updateNavbar(path) {
            var html = ['<span uri="home">Media</span>'],
                splitted,
                len,
                i;

            if (typeof path === 'string' && path !== '') {
                splitted = path.split('/');
                len = splitted.length;

                for (i = 0; i < len; i = i + 1) {
                    html.push(
                        '<span uri="' +
                        splitted.slice(0, i + 1).join('/') +
                        '">' +
                        splitted[i] +
                        '</span>'
                    );
                }
            }
            $('#navbar').html(html.join(' > '));
        },

        /**
         * Handles click event on the "select all" checkbox.
         *
         * @private
         */
        handleSelectAllChange: function Ui_handleSelectAllChange() {
            var $selectAllInput = $('.selectAll .ui-checkbox'),
                self = this;

            if ($selectAllInput.is(':checked')) {
                // check all checkboxes
                $('ul#fileList input[type=checkbox]').each(
                    function setCheckboxValue() {
                        self.setCheckboxValue(this, true);
                    }
                );

                this.enableControlBarButtons(
                    [
                        this.DELETE_BTN,
                        this.COPY_BTN,
                        this.MOVE_BTN
                    ]
                );
            } else {
                $('ul#fileList input[type=checkbox]').each(
                    function setCheckboxValue() {
                        self.setCheckboxValue(this, false);
                    }
                );

                this.disableControlBarButtons(
                    [
                        this.DELETE_BTN,
                        this.COPY_BTN,
                        this.MOVE_BTN
                    ]
                );
            }
        },

        /**
         * Updates the "select all" checkbox state.
         *
         * @private
         */
        refreshSelectAllStatus: function Ui_refreshSelectAllStatus() {
            var $selectAllInput = $('.selectAll .ui-checkbox');
            // update status of select all checkbox
            if ($('ul#fileList input:checkbox:not(:checked)').length === 0) {
                // all nodes checked
                this.setCheckboxValue($selectAllInput, true);
            } else {
                // some node is not checked
                this.setCheckboxValue($selectAllInput, false);
            }
        },

        /**
         * Refreshes activity of edit menu.
         *
         * @private
         */
        refreshEditMenu: function refreshEditMenu() {
            if ($('ul#fileList input:checkbox:checked').length > 0) {
                this.enableControlBarButtons(
                    [
                        this.DELETE_BTN,
                        this.COPY_BTN,
                        this.MOVE_BTN
                    ]
                );
            } else {
                this.disableControlBarButtons(
                    [
                        this.DELETE_BTN,
                        this.COPY_BTN,
                        this.MOVE_BTN
                    ]
                );
            }
        },

        /**
         * Removes html node element from the list.
         *
         * @public
         * @param {string} nodeUri node uri
         */
        removeNodeFromList: function Ui_removeNodeFromList(nodeUri) {
            $('ul#fileList > li[uri="' + nodeUri.replace(/"/g, '\"') + '"]')
                .remove();

            // hide select All checkbox if removed all elements;
            if ($('ul#fileList > li.node').length === 0) {
                this.hideSelectAllArea();
            }
        },

        /**
         * Updates the "paste" menu item visibility.
         */
        refreshPasteActionBtn: function Ui_refreshPasteActionBtn() {
            if (app.emptyClipboard()) {
                $('#pasteActionBtnRow').addClass('hidden');
            } else {
                $('#pasteActionBtnRow').removeClass('hidden');
            }
            $('#moreActionMenu').listview('refresh');
        },

        /**
         * Returns true if the file list is empty, false otherwise.
         *
         * @private
         * @returns {boolean}
         */
        isFileListEmpty: function Ui_isFileListEmpty() {
            return ($('ul#fileList').children('.node').length < 1);
        },

        /**
         * Sets value to the specified checkbox.
         *
         * @private
         * @param {HTMLElement|string} element
         * @param {string} value
         */
        setCheckboxValue: function Ui_setCheckboxValue(element, value) {
            var $element = $(element);

            if ($element) {
                $element.prop('checked', value);
            }
        }
    };
}());
