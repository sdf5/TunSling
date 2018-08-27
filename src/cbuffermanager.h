#ifndef CBUFFERMANAGER_H
#define CBUFFERMANAGER_H

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

class cBufferManager;

class cBuffer {
    public:
        cBuffer(size_t buffer_size, cBufferManager * parent);
        cBuffer(const cBuffer &) = delete;
        cBuffer(cBuffer &&) = delete;
        void reserve() noexcept;
        void release() noexcept;
        bool is_reserved() const noexcept;
        unsigned char * data();
        size_t size() const noexcept;
    private:
        std::vector<unsigned char> m_buffer;
        std::atomic<bool> m_is_reserved;
        cBufferManager * m_parent;
};

class cBufferManager {
    friend class cBuffer;
    public:
        cBufferManager(size_t number_of_buffers, size_t buffer_size);
        cBuffer & get_free_buffer_or_wait();
    private:
        std::vector<std::unique_ptr<cBuffer>> m_buffers;
        std::mutex m_buffers_mtx;
        std::condition_variable m_buffers_reserved_cv;

};

#endif // CBUFFERMANAGER_H
