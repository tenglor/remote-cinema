#include "imageviewwidget.h"
#include <QPixmap>

ImageViewWidget::ImageViewWidget(QWidget *parent) : QWidget(parent)
{
    label = new QLabel(this);
    //label->resize(this->size());
    label->setGeometry(this->geometry());
}

void ImageViewWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    label->setGeometry(this->geometry());
    //label->resize(this->size());
}

void ImageViewWidget::setImage(const QImage &image){
    QPixmap pixmap = QPixmap::fromImage(image).scaled(label->size());
    label->setPixmap(pixmap);
    label->update();
    repaint();
}

