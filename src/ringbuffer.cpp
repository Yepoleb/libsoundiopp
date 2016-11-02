#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"

namespace sio
{
    RingBuffer::RingBuffer()
    {
        m_ringbuffer = nullptr;
    }

    RingBuffer::RingBuffer(RingBuffer&& other)
    {
        m_ringbuffer = other.m_ringbuffer;
        other.m_ringbuffer = nullptr;
    }

    RingBuffer& RingBuffer::operator=(RingBuffer&& other)
    {
        if (&other == this) {
            return *this;
        }
        if (m_ringbuffer != nullptr) {
            soundio_ring_buffer_destroy(m_ringbuffer);
        }
        m_ringbuffer = other.m_ringbuffer;
        other.m_ringbuffer = nullptr;
        return *this;
    }

    RingBuffer::RingBuffer(SoundIoRingBuffer* ringbuffer)
    {
        m_ringbuffer = ringbuffer;
    }

    RingBuffer::~RingBuffer()
    {
        soundio_ring_buffer_destroy(m_ringbuffer);
    }

    int RingBuffer::capacity()
    {
        return soundio_ring_buffer_capacity(m_ringbuffer);
    }

    char* RingBuffer::write_ptr()
    {
        return soundio_ring_buffer_write_ptr(m_ringbuffer);
    }

    void RingBuffer::advance_write_ptr(int count)
    {
        soundio_ring_buffer_advance_write_ptr(m_ringbuffer, count);
    }

    char* RingBuffer::read_ptr()
    {
        return soundio_ring_buffer_read_ptr(m_ringbuffer);
    }

    void RingBuffer::advance_read_ptr(int count)
    {
        soundio_ring_buffer_advance_read_ptr(m_ringbuffer, count);
    }

    int RingBuffer::fill_count()
    {
        return soundio_ring_buffer_fill_count(m_ringbuffer);
    }

    int RingBuffer::free_count()
    {
        return soundio_ring_buffer_free_count(m_ringbuffer);
    }

    void RingBuffer::clear()
    {
        soundio_ring_buffer_clear(m_ringbuffer);
    }
}
