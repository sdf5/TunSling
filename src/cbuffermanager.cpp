#include "cbuffermanager.h"

cBuffer::cBuffer(size_t buffer_size, cBufferManager * parent)
    :
      m_buffer(buffer_size),
      m_is_reserved(false),
      m_parent(parent)
{}

void cBuffer::reserve() noexcept {
    m_is_reserved = true;
}

void cBuffer::release() noexcept {
    m_is_reserved = false;
    m_parent->m_buffers_reserved_cv.notify_one();
}

bool cBuffer::is_reserved() const noexcept {
    return m_is_reserved;
}

unsigned char *cBuffer::data() {
    return &m_buffer.front();
}

size_t cBuffer::size() const noexcept {
    return m_buffer.size();
}

cBufferManager::cBufferManager(size_t number_of_buffers, size_t buffer_size)
    :
      m_buffers(number_of_buffers)
{
    for (auto & buffer : m_buffers)
        buffer = std::make_unique<cBuffer>(buffer_size, this);
}

cBuffer & cBufferManager::get_free_buffer_or_wait() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_buffers_mtx);
        for (auto & buff : m_buffers) {
            if (!buff->is_reserved()) {
                buff->reserve();
                return *buff;
            }
        }
        m_buffers_reserved_cv.wait(lock);
    }
}
