#ifndef _OPENGL_WINDOW_H_
#define _OPENGL_WINDOW_H_

#include <qopengl.h>
#include <qopenglfunctions.h>
#include <qwindow.h>

namespace cg {
namespace ui {

// http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
class OpenGLWindow : public QWindow, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit OpenGLWindow(QWindow *parent = nullptr);
  ~OpenGLWindow();

  virtual bool Initialize() = 0;
  virtual void Render() = 0;

private:
};

} // namespace ui
} // namespace cg

#endif // _OPENGL_WINDOW_H_