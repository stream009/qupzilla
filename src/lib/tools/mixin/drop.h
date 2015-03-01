#ifndef QWIDGET_MIXIN_DROP_H
#define QWIDGET_MIXIN_DROP_H

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QMimeData;
class QMouseEvent;
class QPaintEvent;
class QPoint;
class QWidget;

namespace mixin {

template<typename Base>
class Drop : public Base
{
    enum State { None, Accept };
public:
    Drop(QWidget* const parent);

protected:
    // @override QWidget
    virtual void dragEnterEvent(QDragEnterEvent* const);
    virtual void dragMoveEvent(QDragMoveEvent* const);
    virtual void dropEvent(QDropEvent* const);
    virtual void mouseReleaseEvent(QMouseEvent* const);
    virtual void paintEvent(QPaintEvent* const);

private:
    virtual bool canDrop(const QDragMoveEvent &) { return false; }
    virtual bool canDrop(const QMimeData &) { return false; }
    virtual void paintDropMarker(QPaintEvent &) {}
    virtual void onDrop(QDropEvent&) {}

private:
    State m_state;
};

} // namespace mixin

#include <cassert>

#include <QtCore/QDebug>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QWidget>

namespace mixin {

template<typename Base>
inline Drop<Base>::
Drop(QWidget* const parent)
    : Base(parent),
      m_state(Drop::None)
{
    this->setAcceptDrops(true);
}

template<typename Base>
inline void Drop<Base>::
dragEnterEvent(QDragEnterEvent* const event)
{
    assert(event);
    //qDebug() << __PRETTY_FUNCTION__ << event;
    const QMimeData* const mimeData = event->mimeData();
    assert(mimeData);
    if (canDrop(*mimeData)) {
        event->accept();
    }
    else {
        qDebug() << "unsupported drag:" << event->mimeData()->formats();
    }
}

template<typename Base>
inline void Drop<Base>::
dragMoveEvent(QDragMoveEvent* const event)
{
    assert(event);
    //qDebug() << __PRETTY_FUNCTION__ << event;
    if (canDrop(*event)) {
        m_state = Drop::Accept;
        event->accept();
    }
    else {
        m_state = Drop::None;
        event->ignore();
    }
    this->update();
}

template<typename Base>
inline void Drop<Base>::
dropEvent(QDropEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    assert(event);
    event->acceptProposedAction();
    onDrop(*event);
    m_state = Drop::None;
}

template<typename Base>
inline void Drop<Base>::
mouseReleaseEvent(QMouseEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    Base::mouseReleaseEvent(event);
    m_state = Drop::None;
}

template<typename Base>
inline void Drop<Base>::
paintEvent(QPaintEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    assert(event);
    Base::paintEvent(event);

    if (m_state != Drop::Accept) return;

    paintDropMarker(*event);
}

} // namespace mixin

#endif // QWIDGET_MIXIN_DROP_H
