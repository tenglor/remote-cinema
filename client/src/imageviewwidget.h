#ifndef IMAGEVIEWWIDGET_H
#define IMAGEVIEWWIDGET_H

#include <QWidget>
#include <memory>
#include <QLabel>
#include <QImage>

class ImageViewWidget : public QWidget
{
    Q_OBJECT
    QLabel *label;
public:
    explicit ImageViewWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void setPixmap(const QPixmap &pixmap);
signals:


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // IMAGEVIEWWIDGET_H
