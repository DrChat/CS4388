#include <cmath>
#include <cstdio>
#include <memory>

#include <qguiapplication.h>
#include <qopenglshaderprogram.h>

#include "src/ui/opengl_window.h"

static const char kVertexShader[] = R"(
#version 330

// inputs:
layout(location = 0) in vec2 vPos;

// outpus:
// nothing!

void main() {
  gl_Position.xy = vPos;
  gl_Position.zw = vec2(0.0, 1.0);
})";

static const char kFragmentShader[] = R"(
#version 330

layout(location = 0) out vec4 color;

void main() {
  color = vec4(1.0, 0.0, 0.0, 1.0);
})";

namespace cg {

class MainWindow : public ui::OpenGLWindow {
 public:
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

    // Triangle vertices, counter-clockwise
    const static GLfloat tri_verts[] = {
        0.5f, 2.f / 4.f, 0.5f - 0.25f, 0.f, 0.5f + 0.25f, 0.f,
    };

    // Rectangle vertices (without index buffer)
    const static GLfloat rect_verts[] = {
        -0.5f, -0.5f,  // bl
        -0.2f, -0.5f,  // br
        -0.5f, -0.2f,  // tl

        -0.5f, -0.2f,  // tl
        -0.2f, -0.5f,  // br
        -0.2f, -0.2f,  // tr
    };

    // Hexagon
    GLfloat hexa_verts[16];
    std::memset(hexa_verts, 0xCC, sizeof(hexa_verts));
    hexa_verts[0] = -0.5f;
    hexa_verts[1] = 0.5f;
    for (int i = 0; i < 7; i++) {
      hexa_verts[(i + 1) * 2 + 0] =
          hexa_verts[0] + sin(i / 6.f * 2.f * 3.14159f) / 2.f;
      hexa_verts[(i + 1) * 2 + 1] =
          hexa_verts[1] + cos(i / 6.f * 2.f * 3.14159f) / 2.f;
    }

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate a buffer
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    GLintptr offset = 0;

    // Uh... reserve the data? Intel crashes without this.
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tri_verts) + sizeof(rect_verts) + sizeof(hexa_verts),
                 NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(tri_verts), tri_verts);
    tri_offset_ = offset;
    offset += sizeof(tri_verts);

    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(rect_verts), rect_verts);
    rect_offset_ = offset;
    offset += sizeof(rect_verts);

    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(hexa_verts), hexa_verts);
    hexa_offset_ = offset;
    offset += sizeof(hexa_verts);

    glBindVertexArray(0);

    return true;
  }

  virtual void Render() override {
    const qreal pixel_ratio = devicePixelRatio();
    glViewport(0, 0, width() * pixel_ratio, height() * pixel_ratio);

    // clear the picture
    glClearColor(1.f, 1.f, 1.f, 1.f);

    pipeline_program_->bind();

    // Point to triangle verts, 2 per with a stride of 0
    glBindVertexArray(vao_);
    glEnableVertexAttribArray(program_vpos_attr_);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glVertexAttribPointer(program_vpos_attr_, 2, GL_FLOAT, GL_FALSE, 0,
                          (void*)tri_offset_);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glVertexAttribPointer(program_vpos_attr_, 2, GL_FLOAT, GL_FALSE, 0,
                          (void*)rect_offset_);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glVertexAttribPointer(program_vpos_attr_, 2, GL_FLOAT, GL_FALSE, 0,
                          (void*)hexa_offset_);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

    glDisableVertexAttribArray(program_vpos_attr_);

    pipeline_program_->release();
  }

 private:
  std::unique_ptr<QOpenGLShaderProgram> pipeline_program_ = nullptr;
  GLuint program_vpos_attr_ = 0;

  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  GLintptr tri_offset_ = 0;
  GLintptr rect_offset_ = 0;
  GLintptr hexa_offset_ = 0;
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