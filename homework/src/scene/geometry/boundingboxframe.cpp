#include "boundingboxframe.h"



BoundingBoxFrame::BoundingBoxFrame(const BoundingBox &bounding_box)
    :min(bounding_box.min),
    max(bounding_box.max),
    color(0.25f, 0.75f, 0.25f)
{
}

BoundingBoxFrame::BoundingBoxFrame(const BoundingBox& bounding_box,
                                   const glm::vec3& color)
    :min(bounding_box.min),
      max(bounding_box.max),
      color(color)
{
}


void BoundingBoxFrame::create()
{
    // Draw the Bounding Box

    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    pos.reserve(8);
    pos.push_back(glm::vec3(min.x, min.y, min.z));
    pos.push_back(glm::vec3(max.x, min.y, min.z));
    pos.push_back(glm::vec3(max.x, max.y, min.z));
    pos.push_back(glm::vec3(min.x, max.y, min.z));
    pos.push_back(glm::vec3(min.x, min.y, max.z));
    pos.push_back(glm::vec3(max.x, min.y, max.z));
    pos.push_back(glm::vec3(max.x, max.y, max.z));
    pos.push_back(glm::vec3(min.x, max.y, max.z));

    col.reserve(8);
    for (int i = 0; i < 8; i++)
    {
        col.push_back(this->color);
    }


    idx.reserve(24);

    idx.push_back(0);idx.push_back(4);
    idx.push_back(1);idx.push_back(5);
    idx.push_back(2);idx.push_back(6);
    idx.push_back(3);idx.push_back(7);

    idx.push_back(0);idx.push_back(1);
    idx.push_back(1);idx.push_back(2);
    idx.push_back(2);idx.push_back(3);
    idx.push_back(3);idx.push_back(0);

    idx.push_back(4);idx.push_back(5);
    idx.push_back(5);idx.push_back(6);
    idx.push_back(6);idx.push_back(7);
    idx.push_back(7);idx.push_back(4);

    count = idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(pos.data(), pos.size() * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(col.data(), col.size() * sizeof(glm::vec3));

}

GLenum BoundingBoxFrame::drawMode()
{
    return GL_LINES;
}
