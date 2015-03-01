#ifndef MIXIN_DRAG_H
#define MIXIN_DRAG_H

#include <QtCore/QPoint>

class QDrag;
class QMouseEvent;
class QWidget;

namespace mixin {

template<typename Base>
class Drag : public Base
{
    enum State { None, Start, InProgress };
public:
    Drag(QWidget* const parent);

protected:
    // @override QWidget
    virtual void mousePressEvent(QMouseEvent* const);
    virtual void mouseReleaseEvent(QMouseEvent* const);
    virtual void mouseMoveEvent(QMouseEvent* const);

private:
    virtual bool canDrag(const QPoint &) { return false; }
    virtual void prepareDrag(QDrag &, const QPoint &) {}
    virtual void startDrag(const QPoint &) {}
    virtual void onDragFinished(const QDrag &, const Qt::DropAction) {}

private:
    State m_state;
    QPoint m_dragStartPosition;
};

} // namespace mixin

#include <cassert>

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QApplication>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>

namespace mixin {

template<typename Base>
inline Drag<Base>::
Drag(QWidget* const parent)
    : Base(parent),
      m_state(Drag::None)
{}

template<typename Base>
inline void Drag<Base>::
mousePressEvent(QMouseEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    const QPoint &pos = event->pos();
    if (event->button() == Qt::LeftButton && canDrag(pos)) {
        m_dragStartPosition = pos;
        m_state = Drag::Start;
    }

    Base::mousePressEvent(event);
}

template<typename Base>
inline void Drag<Base>::
mouseReleaseEvent(QMouseEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event;
    Base::mouseReleaseEvent(event);
    m_state = Drag::None;
}

template<typename Base>
inline void Drag<Base>::
mouseMoveEvent(QMouseEvent* const event)
{
    //qDebug() << __PRETTY_FUNCTION__ << event << event->buttons();
    Base::mouseMoveEvent(event);

    if (m_state != Drag::Start) return;
    if (event->buttons() != Qt::LeftButton) return;

    const QPoint &pos = event->pos();
    const int distance =
                (pos - m_dragStartPosition).manhattanLength();
    if (distance < QApplication::startDragDistance()) return;

    if (!canDrag(pos)) return; //TODO should it be the m_dragStartPosition?

    // Qt will take take of the deletion of drag and data.
    QDrag* const drag = new QDrag(this);
    assert(drag);
    QMimeData *data = new QMimeData;
    assert(data);
    drag->setMimeData(data);

    prepareDrag(*drag, pos);

    m_state = Drag::InProgress;
    startDrag(pos);
    const Qt::DropAction dropAction = drag->exec();
    m_state = Drag::None;

    onDragFinished(*drag, dropAction);

    this->update(); // clean up widget
}

} // namespace mixin

#endif // MIXIN_DRAG_H
