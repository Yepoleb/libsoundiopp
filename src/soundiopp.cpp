#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"

namespace sio
{
    int get_bytes_per_sample(FormatId format)
    {
        return soundio_get_bytes_per_sample(static_cast<SoundIoFormat>(format));
    }

    int get_bytes_per_frame(FormatId format, int channel_count)
    {
        return soundio_get_bytes_per_frame(
            static_cast<SoundIoFormat>(format), channel_count);
    }

    int get_bytes_per_second(
        FormatId format, int channel_count, int sample_rate)
    {
        return soundio_get_bytes_per_second(
            static_cast<SoundIoFormat>(format), channel_count, sample_rate);
    }

    const char* error_name(ErrorId err)
    {
        return soundio_strerror(static_cast<int>(err));
    }

    const char* format_name(FormatId format)
    {
        return soundio_format_string(static_cast<SoundIoFormat>(format));
    }

    const char* backend_name(BackendId backend)
    {
        return soundio_backend_name(static_cast<SoundIoBackend>(backend));
    }

    const char* channel_name(ChannelId channel)
    {
        return soundio_get_channel_name(static_cast<SoundIoChannelId>(channel));
    }

    int backend_count()
    {
        // Work around api bug
        return soundio_backend_count(nullptr);
    }

    bool have_backend(BackendId backend)
    {
        return soundio_have_backend(static_cast<SoundIoBackend>(backend));
    }

    SoundIoBackend get_backend(int index)
    {
        // Work around api bug
        return soundio_get_backend(nullptr, index);
    }



    soundio_error::soundio_error(int err)
    {
        m_error = static_cast<ErrorId>(err);;
    }

    soundio_error::soundio_error(ErrorId err)
    {
        m_error = err;
    }

    const char* soundio_error::what() const noexcept
    {
        return soundio_strerror(static_cast<SoundIoError>(m_error));
    }
}
