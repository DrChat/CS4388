/**
 * Copyright (C) Justin Moore 2017
 * See LICENSE in the project root (MIT) for usage
 */

#include <cmath>
#include <cstdio>
#include <memory>

#include <qcoreevent.h>
#include <qevent.h>
#include <qguiapplication.h>
#include <qopenglshaderprogram.h>

#include "core/math.h"
#include "ui/opengl_window.h"

namespace cg {

static const char kVertexShader[] = R"(
#version 330

// inputs:
layout(location = 0) in vec3 vPos;

// outputs:
// nothing!

void main() {
  gl_Position.xyz = vPos;
  gl_Position.w = 1.0;
})";

static const char kFragmentShader[] = R"(
#version 330

layout(location = 0) out vec4 color;

void main() {
  color = vec4(1.0, 0.0, 0.0, 1.0);
})";

class MainWindow : public ui::OpenGLWindow {
 public:
  ~MainWindow() {
    // TODO: GL cleanup
    delete[] objects_;
  }

  virtual bool Setup() override {
    pipeline_program_ = std::make_unique<QOpenGLShaderProgram>(this);

    bool success = true;
    success = success &&
              pipeline_program_->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                         kVertexShader);
    success = success &&
              pipeline_program_->addShaderFromSourceCode(
                  QOpenGLShader::Fragment, kFragmentShader);
    if (!success || !pipeline_program_->link()) {
      auto err = pipeline_program_->log();
      return false;
    }

    program_vpos_attr_ = pipeline_program_->attributeLocation("vPos");
    glGenVertexArrays(1, &vao_);

    // Generate buffer offsets
    objects_ = new Object[OBJECT_COUNT];
    std::memset(objects_, 0, sizeof(Object) * OBJECT_COUNT);
    for (int i = 1; i < OBJECT_COUNT; i++) {
      objects_[i].mode = GL_TRIANGLES;
      objects_[i].vao = vao_;
    }

    GLfloat point_verts[] = {0.f, 0.f, 0.f};
    GLfloat line_verts[] = {-0.5f, 0.f, 0.f, 0.5f, 0.f, 0.f};
    GLfloat tri_verts[] = {0.f, 0.5f, 0.f, -0.5f, 0.f, 0.f, 0.5f, 0.f, 0.f};
    GLfloat square_verts[] = {
        -0.5f, -0.5f, 0.f,  // bl
        0.5f,  -0.5f, 0.f,  // br
        -0.5f, 0.5f,  0.f,  // tl

        -0.5f, 0.5f,  0.f,  // tl
        0.5f,  -0.5f, 0.f,  // br
        0.5f,  0.5f,  0.f,  // tr
    };

    // Hexagon (generated with sin/cos)
    GLfloat hexa_verts[9 * 3];
    std::memset(hexa_verts, 0x0, sizeof(hexa_verts));
    hexa_verts[0] = 0.f;
    hexa_verts[1] = 0.f;
    hexa_verts[2] = 0.f;
    for (int i = 0; i < 8; i++) {
      // i / <n target verts> * 2pi;
      hexa_verts[(i + 1) * 3 + 0] =
          hexa_verts[0] + sin(i / 6.f * 2.f * 3.14159f) / 2.f;
      hexa_verts[(i + 1) * 3 + 1] =
          hexa_verts[1] + cos(i / 6.f * 2.f * 3.14159f) / 2.f;
      hexa_verts[(i + 1) * 3 + 2] = hexa_verts[2];
    }

    // Circle (generated with sin/cos)
    GLfloat circ_verts[23 * 3];
    std::memset(circ_verts, 0x0, sizeof(circ_verts));
    circ_verts[0] = 0.f;
    circ_verts[1] = 0.f;
    circ_verts[2] = 0.f;
    for (int i = 0; i < 22; i++) {
      // i / <n target verts> * 2pi;
      circ_verts[(i + 1) * 3 + 0] =
          circ_verts[0] + sin(i / 21.f * 2.f * 3.14159f) / 2.f;
      circ_verts[(i + 1) * 3 + 1] =
          circ_verts[1] + cos(i / 21.f * 2.f * 3.14159f) / 2.f;
      circ_verts[(i + 1) * 3 + 2] = circ_verts[2];
    }

    GLfloat cube_verts[] = {
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,
    };
    for (size_t i = 0; i < cg::countof(cube_verts); i++) {
      cube_verts[i] /= 2.f;
    }

    glBindVertexArray(vao_);

    // Generate a buffer
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    GLintptr offset = 0;

    // Uh... reserve the data? Intel crashes without this.
    glBufferData(GL_ARRAY_BUFFER, 1024 * 1024 * 2, NULL, GL_STATIC_DRAW);

#define UPLOAD_OBJECT(o, v)                               \
  \
do {                                                      \
    \
glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(v), (v)); \
    objects_[o].offset = offset;                          \
    objects_[o].nverts = GLuint(cg::countof(v)) / 3;      \
    offset += sizeof(v);                                  \
  \
}                                                    \
  while (0)

    UPLOAD_OBJECT(OBJECT_POINT, point_verts);
    UPLOAD_OBJECT(OBJECT_LINE, line_verts);
    UPLOAD_OBJECT(OBJECT_TRIANGLE, tri_verts);
    UPLOAD_OBJECT(OBJECT_SQUARE, square_verts);
    UPLOAD_OBJECT(OBJECT_HEXAGON, hexa_verts);
    UPLOAD_OBJECT(OBJECT_CIRCLE, circ_verts);
    UPLOAD_OBJECT(OBJECT_CUBE, cube_verts);

    objects_[OBJECT_POINT].mode = GL_POINTS;
    objects_[OBJECT_LINE].mode = GL_LINES;
    objects_[OBJECT_HEXAGON].mode = GL_TRIANGLE_FAN;
    objects_[OBJECT_CIRCLE].mode = GL_TRIANGLE_FAN;

#undef UPLOAD_OBJECT

    glBindVertexArray(0);

    // Clear with all white
    glClearColor(1.f, 1.f, 1.f, 1.f);
    return true;
  }

  virtual void Render() override {
    const qreal pixel_ratio = devicePixelRatio();
    glViewport(0, 0, width() * pixel_ratio, height() * pixel_ratio);
    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    // clear the picture
    glClear(GL_COLOR_BUFFER_BIT);

    pipeline_program_->bind();

    if (current_object_ != OBJECT_NONE) {
      Object& object = objects_[current_object_];

      // Point to verts, 3 per with a stride of 0
      glBindVertexArray(object.vao);
      glEnableVertexAttribArray(program_vpos_attr_);

      glVertexAttribPointer(program_vpos_attr_, 3, GL_FLOAT, GL_FALSE, 0,
                            (void*)uintptr_t(object.offset));
      glDrawArrays(object.mode, 0, object.nverts);

      glDisableVertexAttribArray(program_vpos_attr_);
    }

    pipeline_program_->release();
  }

  void keyPressEvent(QKeyEvent* evt) override {
    switch (evt->key()) {
      case Qt::Key::Key_P:
        current_object_ = OBJECT_POINT;
        break;
      case Qt::Key::Key_L:
        current_object_ = OBJECT_LINE;
        break;
      case Qt::Key::Key_T:
        current_object_ = OBJECT_TRIANGLE;
        break;
      case Qt::Key::Key_S:
        current_object_ = OBJECT_SQUARE;
        break;
      case Qt::Key::Key_H:
        current_object_ = OBJECT_HEXAGON;
        break;
      case Qt::Key::Key_C:
        current_object_ = OBJECT_CIRCLE;
        break;
      case Qt::Key::Key_U:
        current_object_ = OBJECT_CUBE;
        break;
    }
  }

 private:
  std::unique_ptr<QOpenGLShaderProgram> pipeline_program_ = nullptr;
  GLuint program_vpos_attr_ = 0;

  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  enum ObjectType {
    OBJECT_NONE,
    OBJECT_POINT,
    OBJECT_LINE,
    OBJECT_TRIANGLE,
    OBJECT_SQUARE,
    OBJECT_HEXAGON,
    OBJECT_CIRCLE,
    OBJECT_CUBE,
    OBJECT_COUNT,
  };

  ObjectType current_object_ = OBJECT_NONE;

  struct Object {
    GLuint vao;
    GLuint offset;  // Vertex data offset
    GLuint nverts;  // Number of vertices
    GLenum mode;    // Draw mode
  };

  Object* objects_ = nullptr;
};

}  // namespace cg

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);

  cg::MainWindow window;
  window.Initialize();
  window.resize(640, 480);
  window.set_animating(true);
  window.show();

  return app.exec();
}
