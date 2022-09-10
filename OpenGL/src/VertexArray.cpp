#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

// Binds the vertex array and buffer and sets up layout
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();
	vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        /**
        * SPECIFY LAYOUT OF DATA
        */
        //Can define and enable as long a buffer has been bound (as above)
        //index of 0 because first attribute, 2 float represent a single attribute/vertex, type of data = floats, non-normalized as floats are already normalized, stride - # bytes between each vertex/offset to next vertex, offset to the next attribute i.e. texture coord
        GLCall(glEnableVertexAttribArray(i)); //can come before or after glVertexAttribPointer, as long as buffer has been bound
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset)); // LINKS BUFFER WITH VAO
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);;
    }
    
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
