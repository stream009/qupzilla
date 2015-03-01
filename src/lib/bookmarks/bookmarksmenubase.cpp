#include "bookmarksmenubase.h"

#include <QtCore/QMimeData>

class ActionMimeData : public QMimeData
{
public:
    ActionMimeData(QAction &action)
        : m_action(&action) {}

    QAction &action() const { return *m_action; }

private:
    QAction *m_action;
};

#include "bookmarkitem.h"
#include "bookmarks.h"
#include "bookmarksmenu.h"
#include "bookmarkstools.h"
#include "mainapplication.h"

#include <QtCore/QPoint>
#include <QtGui/QDrag>
#include <QtGui/QDropEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

BookmarksMenuBase::
BookmarksMenuBase(QWidget* const parent)
    : Base(parent),
      m_activeSubMenu(NULL),
      m_window(NULL)
{}

void BookmarksMenuBase::
setMainWindow(BrowserWindow* window)
{
    Q_ASSERT(window);

    m_window = window;
}

void BookmarksMenuBase::
bookmarkActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmark(item);
    }
}

void BookmarksMenuBase::
bookmarkCtrlActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewTab(item);
    }
}

void BookmarksMenuBase::
bookmarkShiftActivated()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewWindow(item);
    }
}

bool BookmarksMenuBase::
canDrag(const QPoint &pos)
{
    //qDebug() << __func__;
    QAction* const target = this->actionAt(pos);
    return target;
}

void BookmarksMenuBase::
prepareDrag(QDrag &drag, const QPoint &pos)
{
    //qDebug() << __func__;
    QAction* const action = this->actionAt(pos);
    assert(action);

    ActionMimeData* const data = new ActionMimeData(*action);
    assert(data);
    data->setText(action->text());
    drag.setMimeData(data);

    const QRect &rect = this->actionGeometry(action);
    drag.setPixmap(QPixmap::grabWidget(this, rect));
}

void BookmarksMenuBase::
onDragFinished(const QDrag &drag, const Qt::DropAction dropAction)
{
    if (dropAction == Qt::MoveAction && drag.target() != this) {
        ActionMimeData* const data =
                    dynamic_cast<ActionMimeData*>(drag.mimeData());
        assert(data);
        this->removeAction(&data->action());
        qDebug() << &data->action() << "removed from" << this;
    }
}

bool BookmarksMenuBase::
canDrop(const QDragMoveEvent &event)
{
    //qDebug() << __func__;
    QAction* const target = this->actionAt(event.pos());
    if (!target) return false;

    const ActionMimeData* const data =
                dynamic_cast<const ActionMimeData*>(event.mimeData());
    assert(data);
    QAction &source = data->action();
    if (&source == target) return false;

    return true;
}

bool BookmarksMenuBase::
canDrop(const QMimeData &mimeData)
{
    //qDebug() << __func__;
    return mimeData.hasFormat("text/plain");
}

void BookmarksMenuBase::
paintDropMarker(QPaintEvent &)
{
    //qDebug() << __func__ << event;
    const QPoint &pos = this->mapFromGlobal(QCursor::pos());
    QAction* const action = this->actionAt(pos);
    if (!action) return;

    QPainter p(this);
    p.setPen(this->palette().foreground().color());

    const QRect &rect = this->actionGeometry(action);
    //qDebug() << rect.topLeft() << rect.topRight();
    p.drawLine(rect.topLeft(), rect.topRight());
}

void BookmarksMenuBase::
onDrop(QDropEvent &event)
{
    //qDebug() << __PRETTY_FUNCTION__ << &event;
    QAction* const dropTo = this->actionAt(event.pos());
    if (!dropTo) return;

    const QMimeData* const data = event.mimeData();
    assert(data);

    if (event.dropAction() == Qt::MoveAction) {
        const ActionMimeData* const aData =
                            dynamic_cast<const ActionMimeData*>(data);
        assert(aData);
        QAction &target = aData->action(); //TODO factor out

        insertAction(dropTo, &target);
    }
    else if (event.dropAction() == Qt::CopyAction) {
        assert(false);
        assert(data->hasText());

        //TODO create new bookmark item
        QAction* const newAction = new QAction(data->text(), this); //TODO
        assert(newAction);
        insertAction(dropTo, newAction);
    }
    else {
        assert("Unimplemented drop action");
    }

    event.acceptProposedAction();
}

static BookmarkItem &
toBookmarkItem(const QVariant &value)
{
    BookmarkItem *result =
            static_cast<BookmarkItem*>(value.value<void*>());
    assert(result);

    return *result;
}

static void
dump(const BookmarkItem* const folder) //TODO remove
{
    assert(folder->isFolder());

    foreach (BookmarkItem* const item, folder->children()) {
        if (item->isSeparator()) {
            qDebug() << "--- Separator ---";
        }
        else if (item->isFolder()) {
            qDebug() << item->title() << "[F]";
        }
        else {
            qDebug() << item->title();
        }
    }
}

void BookmarksMenuBase::
insertAction(QAction* const before, QAction* const action)
{
    assert(before);
    assert(action);

    Base::insertAction(before, action);

    QWidget* receiver = this;
#if 0
    while (receiver && dynamic_cast<BookmarksMenu*>(receiver) == NULL) {
        receiver = receiver->parentWidget();
    }
    assert(receiver);
#endif
    qDebug() << this << action << receiver;
    foreach (QAction* const a, this->actions()) {
        qDebug() << a << a->text();
    }
    this->connect(action,   SIGNAL(triggered()),
                  receiver, SLOT(bookmarkActivated()));
    this->connect(action,   SIGNAL(ctrlTriggered()),
                  receiver, SLOT(bookmarkCtrlActivated()));
    this->connect(action,   SIGNAL(shiftTriggered()),
                  receiver, SLOT(bookmarkShiftActivated()));

    BookmarkItem &beforeItem = toBookmarkItem(before->data());
    BookmarkItem &actionItem = toBookmarkItem(action->data());

    BookmarkItem* const fromParent = actionItem.parent();
    BookmarkItem* const toParent = beforeItem.parent();
    assert(fromParent);
    assert(toParent);

    int toRow = toParent->children().indexOf(&beforeItem);
    assert(toRow >= 0);

    if (fromParent == toParent) {
        const int fromRow = fromParent->children().indexOf(&actionItem);
        assert(fromRow >= 0);
        if (fromRow < toRow) { // count the removal before the insertion in.
            --toRow;
        }
    }

    mApp->bookmarks()->insertBookmark(toParent, toRow, &actionItem);
}

void BookmarksMenuBase::
dragMoveEvent(QDragMoveEvent* const event)
{
    assert(event);
    Base::dragMoveEvent(event);

    QAction* const target = this->actionAt(event->pos());
    if (!target) return;

    QMenu* const menu = target->menu();
    if (m_activeSubMenu && m_activeSubMenu != menu) {
        m_activeSubMenu->close();
        m_activeSubMenu = NULL;
    }
    if (menu && menu != m_activeSubMenu) {
        const QRect &rect = this->actionGeometry(target);
        const QPoint pos(rect.right(), rect.top());
        m_activeSubMenu = menu;
        menu->popup(this->mapToGlobal(pos));
    }
}

void BookmarksMenuBase::
openBookmark(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmark(m_window, item);
    }
}

void BookmarksMenuBase::
openBookmarkInNewTab(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmarkInNewTab(m_window, item);
    }
}

void BookmarksMenuBase::
openBookmarkInNewWindow(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    BookmarksTools::openBookmarkInNewWindow(item);
}
