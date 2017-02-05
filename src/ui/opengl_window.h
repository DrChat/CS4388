#ifndef _OPENGL_WINDOW_H_
#define _OPENGL_WINDOW_H_

#include <qopengl.h>
#include <qopenglfunctions_3_3_core.h>
#include <qwindow.h>

#include <memory>

namespace cg {
namespace ui {

// http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
class OpenGLWindow : public QWindow, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT
 public:
  explicit OpenGLWindow(QWindow* parent = nullptr);
  ~OpenGLWindow();

  virtual bool Setup() = 0;
  virtual void Render() = 0;

  bool Initialize();
  void Begin();
  void End();

  void set_animating(bool animating) {
    animating_ = animating;

    if (animating) {
      requestUpdate();
    }
  }

 private:
  bool event(QEvent* event) override;
  void exposeEvent(QExposeEvent* event) override;

  bool animating_ = false;
  std::unique_ptr<QOpenGLContext> context_ = nullptr;
};

}  // namespace ui
}  // namespace cg

#endif  // _OPENGL_WINDOW_H_