#include "src/ui/opengl_window.h"

namespace cg {
namespace ui {

OpenGLWindow::OpenGLWindow(QWindow* parent) : QWindow(parent) {
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  setFormat(fmt);
  setSurfaceType(QSurface::OpenGLSurface);
  create();
}

OpenGLWindow::~OpenGLWindow() {}

bool OpenGLWindow::Initialize() {
  bool success = true;

  context_ = std::make_unique<QOpenGLContext>(this);
  context_->setFormat(requestedFormat());
  success = success && context_->create();
  if (!success) {
    return false;
  }

  success = success && context_->makeCurrent(this);
  success = success && initializeOpenGLFunctions();
  if (!success) {
    return false;
  }

  success = success && Setup();
  if (!success) {
    return false;
  }

  return true;
}

void OpenGLWindow::Begin() {
  if (!context_) {
    // TODO: Throw an error.
    return;
  }

  context_->makeCurrent(this);
}

void OpenGLWindow::End() {
  if (!isExposed()) {
    return;
  }

  context_->swapBuffers(this);
}

bool OpenGLWindow::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::UpdateRequest:
      // Render a frame.
      Begin();
      Render();
      End();

      // If animating, continuously request updates.
      if (animating_) {
        requestUpdate();
      }

      return true;
    default:
      return QWindow::event(event);
  }
}

void OpenGLWindow::exposeEvent(QExposeEvent* event) {
  Q_UNUSED(event);

  if (isExposed()) {
    // Render a frame.
    Begin();
    Render();
    End();
  }
}

}  // namespace ui
}  // namespace cg