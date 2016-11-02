#include <string>
#include <functional>
#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"

namespace sio
{
    OutStream::OutStream()
    {
        m_outstream = nullptr;
        m_device = nullptr;
        m_userdata = nullptr;
    }

    OutStream::OutStream(SoundIoOutStream* outstream, Device* device)
    {
        m_outstream = outstream;
        m_device = device;
        m_userdata = m_outstream->userdata;
        m_outstream->userdata = this;
    }

    OutStream::OutStream(OutStream&& other)
    {
        m_outstream = other.m_outstream;
        m_device = other.m_device;
        m_userdata = other.m_userdata;
        m_name = other.m_name;
        m_write_callback = other.m_write_callback;
        m_underflow_callback = other.m_underflow_callback;
        m_error_callback = other.m_error_callback;
        m_outstream->userdata = this;
        other.m_outstream = nullptr;
    }

    OutStream& OutStream::operator=(OutStream&& other)
    {
        if (&other == this) {
            return *this;
        }
        if (m_outstream != nullptr) {
            soundio_outstream_destroy(m_outstream);
        }
        m_outstream = other.m_outstream;
        m_device = other.m_device;
        m_userdata = other.m_userdata;
        m_name = other.m_name;
        m_write_callback = other.m_write_callback;
        m_underflow_callback = other.m_underflow_callback;
        m_error_callback = other.m_error_callback;
        m_outstream->userdata = this;
        other.m_outstream = nullptr;
        return *this;
    }

    OutStream::~OutStream()
    {
        soundio_outstream_destroy(m_outstream);
    }

    OutStream::operator SoundIoOutStream*()
    {
        return m_outstream;
    }

    OutStream::operator const SoundIoOutStream*() const
    {
        return m_outstream;
    }

    void OutStream::open()
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_open(m_outstream));
        if (m_outstream->layout_error) {
            throw soundio_error(m_outstream->layout_error);
        }
    }

    void OutStream::start()
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_start(m_outstream));
    }

    int OutStream::begin_write(ChannelArea*& areas, int frame_count)
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_begin_write(
            m_outstream, &areas, &frame_count));
        // frame_count gets modified by begin_write
        return frame_count;
    }

    void OutStream::end_write()
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_end_write(m_outstream));
    }

    void OutStream::clear_buffer()
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_clear_buffer(m_outstream));
    }

    void OutStream::pause(bool paused)
    {
        WRAP_SOUNDIO_ERROR(soundio_outstream_pause(m_outstream, paused));
    }

    double OutStream::get_latency()
    {
        double latency;
        WRAP_SOUNDIO_ERROR(soundio_outstream_get_latency(
            m_outstream, &latency));
        return latency;
    }

    // Getters/Setters

    Device* OutStream::get_device()
    {
        return m_device;
    }

    FormatId OutStream::get_format() const
    {
        return static_cast<FormatId>(m_outstream->format);
    }

    void OutStream::set_format(FormatId format)
    {
        m_outstream->format = static_cast<SoundIoFormat>(format);
    }

    int OutStream::get_sample_rate() const
    {
        return m_outstream->sample_rate;
    }

    void OutStream::set_sample_rate(int sample_rate)
    {
        m_outstream->sample_rate = sample_rate;
    }

    ChannelLayout OutStream::get_layout() const
    {
        return ChannelLayout(m_outstream->layout);
    }

    void OutStream::set_layout(const ChannelLayout& layout)
    {
        m_outstream->layout = layout;
    }

    double OutStream::get_software_latency() const
    {
        return m_outstream->software_latency;
    }

    void OutStream::set_software_latency(double software_latency)
    {
        m_outstream->software_latency = software_latency;
    }

    void* OutStream::get_userdata()
    {
        return m_userdata;
    }

    void OutStream::set_userdata(void* userdata)
    {
        m_userdata = userdata;
    }

    std::string OutStream::get_name() const
    {
        return std::string(m_outstream->name);
    }

    void OutStream::set_name(std::string name)
    {
        m_name = name;
        m_outstream->name = m_name.c_str();
    }

    bool OutStream::get_non_terminal_hint() const
    {
        return m_outstream->non_terminal_hint;
    }

    void OutStream::set_non_terminal_hint(bool hint)
    {
        m_outstream->non_terminal_hint = hint;
    }

    int OutStream::get_bytes_per_frame() const
    {
        return m_outstream->bytes_per_frame;
    }

    int OutStream::get_bytes_per_sample() const
    {
        return m_outstream->bytes_per_sample;
    }

    std::function<void(OutStream*, int, int)> OutStream::get_write_callback()
    {
        return m_write_callback;
    }

    void OutStream::set_write_callback(
        std::function<void(OutStream*, int, int)> write_callback)
    {
        m_write_callback = write_callback;
        m_outstream->write_callback = write_callback_wrapper;
    }

    std::function<void(OutStream*)> OutStream::get_underflow_callback()
    {
        return m_underflow_callback;
    }

    void OutStream::set_underflow_callback(
        std::function<void(OutStream*)> underflow_callback)
    {
        m_underflow_callback = underflow_callback;
        m_outstream->underflow_callback = underflow_callback_wrapper;
    }

    std::function<void(OutStream*, int)> OutStream::get_error_callback()
    {
        return m_error_callback;
    }

    void OutStream::set_error_callback(
        std::function<void(OutStream*, int)> error_callback)
    {
        m_error_callback = error_callback;
        m_outstream->error_callback = error_callback_wrapper;
    }

    void OutStream::write_callback_wrapper(
        SoundIoOutStream* stream, int frame_count_min, int frame_count_max)
    {
        OutStream* outstream = static_cast<OutStream*>(stream->userdata);
        auto cb = outstream->get_write_callback();
        cb(outstream, frame_count_min, frame_count_max);
    }

    void OutStream::underflow_callback_wrapper(SoundIoOutStream* stream)
    {
        OutStream* outstream = static_cast<OutStream*>(stream->userdata);
        auto cb = outstream->get_underflow_callback();
        cb(outstream);
    }

    void OutStream::error_callback_wrapper(SoundIoOutStream* stream, int err)
    {
        OutStream* outstream = static_cast<OutStream*>(stream->userdata);
        auto cb = outstream->get_error_callback();
        cb(outstream, err);
    }
}
