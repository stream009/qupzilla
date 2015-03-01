#ifndef BOOKMARKS_MENU_BASE_H
#define BOOKMARKS_MENU_BASE_H

#include "browserwindow.h"
#include "enhancedmenu.h"
#include "mixin/drag.h"
#include "mixin/drop.h"

#include <QtCore/QPointer>
#include <QtGui/QMenu>

class QAction;
class QDrag;
class QDragMoveEvent;
class QDropEvent;
class QPaintEvent;
class QPoint;
class QWidget;

class BookmarkItem;
class BrowserWindow;

typedef mixin::Drag<
        mixin::Drop<Menu> > BookmarksMenuBaseBase;

class BookmarksMenuBase : public BookmarksMenuBaseBase
{
    Q_OBJECT
    typedef BookmarksMenuBaseBase Base;
public:
    BookmarksMenuBase(QWidget* const parent);

    void setMainWindow(BrowserWindow* window);
    QPointer<BrowserWindow> mainWindow() const { return m_window; }

    // @overload QWidget
    void insertAction(QAction* const before, QAction* const);

private:
    // @override Drag
    virtual bool canDrag(const QPoint &);
    virtual void prepareDrag(QDrag &, const QPoint &);
    virtual void onDragFinished(const QDrag &, const Qt::DropAction);

    // @override Drop
    virtual bool canDrop(const QDragMoveEvent &);
    virtual bool canDrop(const QMimeData &);
    virtual void paintDropMarker(QPaintEvent &event);
    virtual void onDrop(QDropEvent&);

    // @override QWidget
    virtual void dragMoveEvent(QDragMoveEvent* const);

private slots:
    void bookmarkActivated();
    void bookmarkCtrlActivated();
    void bookmarkShiftActivated();

private:
    void openBookmark(BookmarkItem* item);
    void openBookmarkInNewTab(BookmarkItem* item);
    void openBookmarkInNewWindow(BookmarkItem* item);

private:
    QMenu *m_activeSubMenu;
    QPointer<BrowserWindow> m_window;
};

#endif // BOOKMARKS_MENU_BASE_H
