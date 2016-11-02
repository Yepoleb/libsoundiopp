#include <string>
#include <functional>
#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"

namespace sio
{
    InStream::InStream()
    {
        m_instream = nullptr;
        m_device = nullptr;
        m_userdata = nullptr;
    }

    InStream::InStream(SoundIoInStream* instream, Device* device)
    {
        m_instream = instream;
        m_device = device;
        m_userdata = m_instream->userdata;
        m_instream->userdata = this;
    }

    InStream::InStream(InStream&& other)
    {
        m_instream = other.m_instream;
        m_device = other.m_device;
        m_userdata = other.m_userdata;
        m_name = other.m_name;
        m_read_callback = other.m_read_callback;
        m_overflow_callback = other.m_overflow_callback;
        m_error_callback = other.m_error_callback;
        m_instream->userdata = this;
        other.m_instream = nullptr;
    }

    InStream& InStream::operator=(InStream&& other)
    {
        if (&other == this) {
            return *this;
        }
        if (m_instream != nullptr) {
            soundio_instream_destroy(m_instream);
        }
        m_instream = other.m_instream;
        m_device = other.m_device;
        m_userdata = other.m_userdata;
        m_name = other.m_name;
        m_read_callback = other.m_read_callback;
        m_overflow_callback = other.m_overflow_callback;
        m_error_callback = other.m_error_callback;
        m_instream->userdata = this;
        other.m_instream = nullptr;
        return *this;
    }

    InStream::~InStream()
    {
        soundio_instream_destroy(m_instream);
    }

    InStream::operator SoundIoInStream*()
    {
        return m_instream;
    }

    InStream::operator const SoundIoInStream*() const
    {
        return m_instream;
    }

    void InStream::open()
    {
        WRAP_SOUNDIO_ERROR(soundio_instream_open(m_instream));
        if (m_instream->layout_error) {
            throw soundio_error(m_instream->layout_error);
        }
    }

    void InStream::start()
    {
        WRAP_SOUNDIO_ERROR(soundio_instream_start(m_instream));
    }

    int InStream::begin_read(ChannelArea*& areas, int frame_count)
    {
        WRAP_SOUNDIO_ERROR(soundio_instream_begin_read(
            m_instream, &areas, &frame_count));
        // frame_count gets modified by begin_read
        return frame_count;
    }

    void InStream::end_read()
    {
        WRAP_SOUNDIO_ERROR(soundio_instream_end_read(m_instream));
    }

    void InStream::pause(bool paused)
    {
        WRAP_SOUNDIO_ERROR(soundio_instream_pause(m_instream, paused));
    }

    double InStream::get_latency()
    {
        double latency;
        WRAP_SOUNDIO_ERROR(soundio_instream_get_latency(
            m_instream, &latency));
        return latency;
    }

    // Getters/Setters

    Device* InStream::get_device()
    {
        return m_device;
    }

    FormatId InStream::get_format() const
    {
        return static_cast<FormatId>(m_instream->format);
    }

    void InStream::set_format(FormatId format)
    {
        m_instream->format = static_cast<SoundIoFormat>(format);
    }

    int InStream::get_sample_rate() const
    {
        return m_instream->sample_rate;
    }

    void InStream::set_sample_rate(int sample_rate)
    {
        m_instream->sample_rate = sample_rate;
    }

    ChannelLayout InStream::get_layout() const
    {
        return ChannelLayout(m_instream->layout);
    }

    void InStream::set_layout(const ChannelLayout& layout)
    {
        m_instream->layout = layout;
    }

    double InStream::get_software_latency() const
    {
        return m_instream->software_latency;
    }

    void InStream::set_software_latency(double software_latency)
    {
        m_instream->software_latency = software_latency;
    }

    void* InStream::get_userdata()
    {
        return m_userdata;
    }

    void InStream::set_userdata(void* userdata)
    {
        m_userdata = userdata;
    }

    std::string InStream::get_name() const
    {
        return std::string(m_instream->name);
    }

    void InStream::set_name(std::string name)
    {
        m_name = name;
        m_instream->name = m_name.c_str();
    }

    bool InStream::get_non_terminal_hint() const
    {
        return m_instream->non_terminal_hint;
    }

    void InStream::set_non_terminal_hint(bool hint)
    {
        m_instream->non_terminal_hint = hint;
    }

    int InStream::get_bytes_per_frame() const
    {
        return m_instream->bytes_per_frame;
    }

    int InStream::get_bytes_per_sample() const
    {
        return m_instream->bytes_per_sample;
    }

    std::function<void(InStream*, int, int)> InStream::get_read_callback()
    {
        return m_read_callback;
    }

    void InStream::set_read_callback(
        std::function<void(InStream*, int, int)> read_callback)
    {
        m_read_callback = read_callback;
        m_instream->read_callback = read_callback_wrapper;
    }

    std::function<void(InStream*)> InStream::get_overflow_callback()
    {
        return m_overflow_callback;
    }

    void InStream::set_overflow_callback(
        std::function<void(InStream*)> overflow_callback)
    {
        m_overflow_callback = overflow_callback;
        m_instream->overflow_callback = overflow_callback_wrapper;
    }

    std::function<void(InStream*, int)> InStream::get_error_callback()
    {
        return m_error_callback;
    }

    void InStream::set_error_callback(
        std::function<void(InStream*, int)> error_callback)
    {
        m_error_callback = error_callback;
        m_instream->error_callback = error_callback_wrapper;
    }

    void InStream::read_callback_wrapper(
        SoundIoInStream* stream, int frame_count_min, int frame_count_max)
    {
        InStream* instream = static_cast<InStream*>(stream->userdata);
        auto cb = instream->get_read_callback();
        cb(instream, frame_count_min, frame_count_max);
    }

    void InStream::overflow_callback_wrapper(SoundIoInStream* stream)
    {
        InStream* instream = static_cast<InStream*>(stream->userdata);
        auto cb = instream->get_overflow_callback();
        cb(instream);
    }

    void InStream::error_callback_wrapper(SoundIoInStream* stream, int err)
    {
        InStream* instream = static_cast<InStream*>(stream->userdata);
        auto cb = instream->get_error_callback();
        cb(instream, err);
    }
}
