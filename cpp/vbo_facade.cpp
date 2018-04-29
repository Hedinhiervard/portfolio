/*
 © 2016 iosoftware
 http://palmkingdoms.com
 support@palmkingdoms.com
*/

#include <limits>

#include "logog/include/logog.hpp"

#include "gameworld/objects/BaseMapObject.h"
#include "gameworld/objects/PathElement.h"
#include "gameworld/protos/BaseMapObjectT.h"
#include "gameworld/rendering/FrameHierarchy.h"
#include "gameworld/rendering/vbo/VboFacade.h"
#include "mgr/TextureMgr.h"

using namespace cocos2d;
using namespace fileaccessor;
using namespace iocpputil;
using namespace std;

template <typename VERTEX>
void VboFacade<VERTEX>::reset()
{
    m_vboBuffer.reset();
    m_vboSize = 0;
    m_active = false;
}

template <typename VERTEX>
void VboFacade<VERTEX>::setEmptyKeepingReservedSize()
{
    m_vboSize = 0;
    m_active = false;
}

template <typename VERTEX>
void VboFacade<VERTEX>::create()
{
    if(m_vboBuffer.vertexArray.empty())
        return;
    weak_assert(m_vboBuffer.indexArray.size() > 0);
    weak_assert(m_vboBuffer.vertexArray.size() < numeric_limits<GLsizei>::max());

    m_vboSize = static_cast<GLuint>(m_vboBuffer.vertexArray.size());

    if(m_vao == 0)
        glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if(m_vbuffer == 0)
        glGenBuffers(1, &m_vbuffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer);
    CHECK_GL_ERROR_DEBUG();

    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * m_vboBuffer.vertexArray.size(), m_vboBuffer.vertexArray.data(), m_updateType);
    CHECK_GL_ERROR_DEBUG();

    if(m_ibuffer == 0)
        glGenBuffers(1, &m_ibuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffer);
    CHECK_GL_ERROR_DEBUG();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLshort) * m_vboBuffer.indexArray.size(),
                 m_vboBuffer.indexArray.data(),
                 m_updateType);
    CHECK_GL_ERROR_DEBUG();

    m_shader->setupAttribPointers();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_DEBUG();

    glBindVertexArray(0);
    m_active = true;
}

template <typename VERTEX>
void VboFacade<VERTEX>::resetQuad(const VertexSequence& seq)
{
    m_vboBuffer.resetQuad(seq);
}

template <typename VERTEX>
void VboFacade<VERTEX>::updateV(const VertexSequence& seq)
{
    if(m_vboBuffer.vertexArray.empty())
        return;

    weak_assert(seq.vboOffset >= 0);
    weak_assert(static_cast<size_t>(seq.vboOffset + seq.vertexCount) <= m_vboBuffer.vertexArray.size());
    if(seq.vertexCount == 0)
        return;

    weak_assert(m_active);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer);
    CHECK_GL_ERROR_DEBUG();

    glBufferSubData(GL_ARRAY_BUFFER, seq.vboOffset * sizeof(VERTEX), seq.vertexCount * sizeof(VERTEX), &m_vboBuffer.vertexArray[seq.vboOffset]);
    CHECK_GL_ERROR_DEBUG();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_DEBUG();

    glBindVertexArray(0);
}

template <typename VERTEX>
void VboFacade<VERTEX>::updateI(const VertexSequence& seq)
{
    if(m_vboBuffer.indexArray.empty())
        return;

    weak_assert(seq.vboOffset >= 0);
    weak_assert(static_cast<size_t>(seq.vboOffset + seq.vertexCount) <= m_vboBuffer.indexArray.size());
    if(seq.vertexCount == 0)
        return;

    weak_assert(m_active);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffer);
    CHECK_GL_ERROR_DEBUG();

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    seq.vboOffset * sizeof(GLshort),
                    seq.vertexCount * sizeof(GLshort),
                    &m_vboBuffer.indexArray[seq.vboOffset]);
    CHECK_GL_ERROR_DEBUG();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_DEBUG();

    glBindVertexArray(0);
}


VboFacadeBase::VboFacadeBase(GLenum updateType, GLenum depthFunc)
        : m_updateType(updateType)
        , m_active(false)
        , m_depthFunc(depthFunc)
{
}

template <typename VERTEX>
VboFacade<VERTEX>::VboFacade(shared_ptr<VboShader<VERTEX>> shader,
                             GLenum updateType, GLenum depthFunc)
        : VboFacadeBase(updateType, depthFunc), m_shader(shader)
{
    //    auto id = sm_ids.empty() ? 0 : (*sm_ids.rbegin() + 1);
    //    sm_ids.insert(id);
    //    m_vboBuffer.setId(id);
}

template <typename VERTEX>
VboFacade<VERTEX>::~VboFacade()
{
    if(m_vbuffer != 0)
        glDeleteBuffers(1, &m_vbuffer);
    if(m_ibuffer != 0)
        glDeleteBuffers(1, &m_ibuffer);
    if(m_vao != 0)
        glDeleteVertexArrays(1, &m_vao);
}

template <typename VERTEX>
void VboFacade<VERTEX>::draw()
{
    if(!m_active || !m_enabled)
        return;
    glDepthFunc(m_depthFunc);

    m_shader->switchTo();

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer);
    CHECK_GL_ERROR_DEBUG();

    glDrawElements(GL_TRIANGLES, static_cast<GLuint>(m_vboBuffer.indexArray.size()), GL_UNSIGNED_SHORT, nullptr);
    CHECK_GL_ERROR_DEBUG();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_DEBUG();

    glBindVertexArray(0);

    ccGLActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR_DEBUG();
}

template class VboFacade<TexturedVertex>;
template class VboFacade<MaskedTexturedVertex>;
template class VboFacade<ColoredTexturedVertex>;
template class VboFacade<AlphaTexturedVertex>;
template class VboFacade<ColoredAlphaTexturedVertex>;
template class VboFacade<AlphaVertex>;
