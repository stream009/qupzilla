#ifndef MIXIN_CONTEXT_MENU_H
#define MIXIN_CONTEXT_MENU_H

#include <QtGui/QMenu>

class QAction;
class QContextMenuEvent;
class QWidget;

namespace mixin {

template<typename Base>
class ContextMenu : public Base
{
public:
    ContextMenu(QWidget* const parent);

protected:
    void addContextAction(QAction* const);

protected:
    // @override QWidget
    virtual void mousePressEvent(QMouseEvent* const);
    virtual void keyPressEvent(QKeyEvent* const);

private:
    QMenu m_menu;
};

} // namespace mixin

#include <QtCore/QDebug>
#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QWidget>

namespace mixin {

template<typename Base>
inline ContextMenu<Base>::
ContextMenu(QWidget* const parent)
    : Base(parent)
{}

template<typename Base>
inline void ContextMenu<Base>::
addContextAction(QAction* const action)
{
    m_menu.addAction(action);
}

// By right mouse button QApplication send QContextMenuEvent
// to a widget or top-most popup if it is available.
// By menu key, QKeyManager send QContextMenuEvent to a widget
// which has keyboard focus at that moment.
// These are not desirable situation for sub menu items, because
// for sub menu items, sub menu IS top-most popup and focused
// widget insted of sub menu item.

// So, in order to support sub menu we don't rely on QContextMenuEvent,
// insted we handle MouseButtonPress event and KeyPress event directory.

template<typename Base>
inline void ContextMenu<Base>::
mousePressEvent(QMouseEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    Base::mousePressEvent(event);

    if (!this->rect().contains(event->pos())) return;

    if (event->button() == Qt::RightButton) {
        m_menu.exec(event->globalPos());
    }
}

template<typename Base>
inline void ContextMenu<Base>::
keyPressEvent(QKeyEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    Base::keyPressEvent(event);

    if (event->key() == Qt::Key_Menu) {
        const QPoint &pos = this->rect().center();
        m_menu.exec(this->mapToGlobal(pos));
    }
}

} // namespace mixin

#endif // MIXIN_CONTEXT_MENU_H
