//
// open horizon -- undefined_darkness@outlook.com
//

#pragma once

#include <QGLWidget>

//------------------------------------------------------------

class scene_view : public QGLWidget
{
    Q_OBJECT

public:
    scene_view(QWidget *parent = NULL);

private:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
};

//------------------------------------------------------------
