#include "iocpputil/weak_assert.h"

#include "gameworld/rendering/vbo/VboBuffer.h"

using namespace cocos2d;
using namespace std;

template <typename VERTEX>
void VboBuffer<VERTEX>::resize(size_t quadCount)
{
    vertexArray.resize(quadCount * 4);
    indexArray.resize(quadCount * 6);

}

template <typename VERTEX>
void VboBuffer<VERTEX>::resize(size_t vertexCount, size_t indexCount)
{
    vertexArray.resize(vertexCount);
    indexArray.resize(indexCount);
}

template <typename VERTEX>
void VboBuffer<VERTEX>::reserve(size_t vertexCount, size_t indexCount)
{
    vertexArray.reserve(vertexCount);
    indexArray.reserve(indexCount);
}

template <typename VERTEX>
void VboBuffer<VERTEX>::reserve(size_t quadCount)
{
    vertexArray.reserve(vertexArray.capacity() + quadCount * 4);
    indexArray.reserve(indexArray.capacity() + quadCount * 6);

    VertexSequenceList l;
    for(size_t q = 0; q < quadCount; q++)
    {
        l.push_back(getQuad());
    }
    for(auto& q : l)
    {
        m_spareQuads.insert(make_pair(q.vboOffset, q));
    }
}

template <typename VERTEX>
void VboBuffer<VERTEX>::putQuad(VertexSequence seq)
{
    weak_assert(seq.vertexCount == 4);
    weak_assert(m_spareQuads.find(seq.vboOffset) == m_spareQuads.end());
    weak_assert(static_cast<size_t>(seq.vboOffset + seq.vertexCount) <=
                vertexArray.size());
    m_spareQuads.insert(make_pair(seq.vboOffset, seq));
}

template <typename VERTEX>
void VboBuffer<VERTEX>::pourSome(size_t cnt, SingleVboQuadSink* sink)
{
    for(size_t idx = 0; idx < cnt; idx++)
    {
        sink->putQuad(getQuad());
    }
}

template <typename VERTEX>
void VboBuffer<VERTEX>::resetQuad(VertexSequence seq)
{
    weak_assert(static_cast<size_t>(seq.vboOffset + seq.vertexCount) <=
                vertexArray.size());
    for(size_t idx = 0; idx < static_cast<size_t>(seq.vertexCount); idx++)
        clearVertex<0>(vertexArray[seq.vboOffset + idx]);
}

template <typename VERTEX>
void VboBuffer<VERTEX>::reset()
{
    vertexArray.clear();
    indexArray.clear();
    m_spareQuads.clear();
}

template <typename VERTEX>
VertexSequence VboBuffer<VERTEX>::getQuad()
{
    if(!m_spareQuads.empty())
    {
        VertexSequence result = m_spareQuads.begin()->second;
        m_spareQuads.erase(m_spareQuads.begin());
        weak_assert(m_spareQuads.find(result.vboOffset) == m_spareQuads.end());
        weak_assert(result.vertexCount == 4);
        weak_assert(static_cast<size_t>(result.vboOffset + result.vertexCount) <=
                    vertexArray.size());
        return result;
    }

    VertexSequence result;

    result.vboOffset = vertexArray.size();
    result.vertexCount = 4;

    vertexArray.emplace_back();
    vertexArray.emplace_back();
    vertexArray.emplace_back();
    vertexArray.emplace_back();

    indexArray.emplace_back(result.vboOffset);
    indexArray.emplace_back(result.vboOffset + 1);
    indexArray.emplace_back(result.vboOffset + 3);
    indexArray.emplace_back(result.vboOffset + 3);
    indexArray.emplace_back(result.vboOffset + 1);
    indexArray.emplace_back(result.vboOffset + 2);

    return result;
}

template class VboBuffer<TexturedVertex>;
template class VboBuffer<MaskedTexturedVertex>;
template class VboBuffer<ColoredTexturedVertex>;
template class VboBuffer<AlphaTexturedVertex>;
template class VboBuffer<ColoredAlphaTexturedVertex>;
template class VboBuffer<AlphaVertex>;
