/* ============================================================
* QupZilla - WebKit based browser
* Copyright (C) 2014  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "bookmarksmenu.h"
#include "bookmarkstools.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "browsinglibrary.h"
#include "browserwindow.h"
#include "qzsettings.h"
#include "tabwidget.h"

BookmarksMenu::BookmarksMenu(QWidget* parent)
    : BookmarksMenuBase(parent)
//    , m_window(0)
    , m_changed(true)
{
    init();

    connect(mApp->bookmarks(), SIGNAL(bookmarkAdded(BookmarkItem*)), this, SLOT(bookmarksChanged()));
    connect(mApp->bookmarks(), SIGNAL(bookmarkRemoved(BookmarkItem*)), this, SLOT(bookmarksChanged()));
    connect(mApp->bookmarks(), SIGNAL(bookmarkChanged(BookmarkItem*)), this, SLOT(bookmarksChanged()));
}
#if 0
void BookmarksMenu::setMainWindow(BrowserWindow* window)
{
    Q_ASSERT(window);

    m_window = window;
}
#endif
void BookmarksMenu::bookmarkPage()
{
    if (this->mainWindow()) {
        this->mainWindow()->bookmarkPage();
    }
}

void BookmarksMenu::bookmarkAllTabs()
{
    if (this->mainWindow()) {
        BookmarksTools::bookmarkAllTabsDialog(this->mainWindow(), this->mainWindow()->tabWidget());
    }
}

void BookmarksMenu::showBookmarksManager()
{
    if (this->mainWindow()) {
        mApp->browsingLibrary()->showBookmarks(this->mainWindow());
    }
}

void BookmarksMenu::bookmarksChanged()
{
    m_changed = true;
}

void BookmarksMenu::aboutToShow()
{
    if (m_changed) {
        refresh();
        m_changed = false;
    }
}

void BookmarksMenu::menuMiddleClicked(Menu* menu)
{
    BookmarkItem* item = static_cast<BookmarkItem*>(menu->menuAction()->data().value<void*>());
    Q_ASSERT(item);
    openFolder(item);
}
#if 0
void BookmarksMenu::bookmarkActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmark(item);
    }
}

void BookmarksMenu::bookmarkCtrlActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewTab(item);
    }
}

void BookmarksMenu::bookmarkShiftActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewWindow(item);
    }
}
#endif
void BookmarksMenu::openFolder(BookmarkItem* item)
{
    Q_ASSERT(item->isFolder());

    if (this->mainWindow()) {
        BookmarksTools::openFolderInTabs(this->mainWindow(), item);
    }
}
#if 0
void BookmarksMenu::openBookmark(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmark(m_window, item);
    }
}

void BookmarksMenu::openBookmarkInNewTab(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmarkInNewTab(m_window, item);
    }
}

void BookmarksMenu::openBookmarkInNewWindow(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    BookmarksTools::openBookmarkInNewWindow(item);
}
#endif
void BookmarksMenu::init()
{
    setTitle(tr("&Bookmarks"));

    addAction(QIcon::fromTheme("bookmark-new"), tr("Bookmark &This Page"), this, SLOT(bookmarkPage()))->setShortcut(QKeySequence("Ctrl+D"));
    addAction(QIcon::fromTheme("bookmark-new-list"), tr("Bookmark &All Tabs"), this, SLOT(bookmarkAllTabs()));
    addAction(QIcon::fromTheme("bookmarks-organize"), tr("Organize &Bookmarks"), this, SLOT(showBookmarksManager()))->setShortcut(QKeySequence("Ctrl+Shift+O"));
    addSeparator();

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
    connect(this, SIGNAL(menuMiddleClicked(Menu*)), this, SLOT(menuMiddleClicked(Menu*)));
}

void BookmarksMenu::refresh()
{
    while (actions().count() != 4) {
        QAction* act = actions().at(4);
        if (act->menu()) {
            act->menu()->clear();
        }
        removeAction(act);
        delete act;
    }

    BookmarksTools::addActionToMenu(this, this, mApp->bookmarks()->toolbarFolder());
    addSeparator();

    foreach (BookmarkItem* child, mApp->bookmarks()->menuFolder()->children()) {
        BookmarksTools::addActionToMenu(this, this, child);
    }

    addSeparator();
    BookmarksTools::addActionToMenu(this, this, mApp->bookmarks()->unsortedFolder());
}
