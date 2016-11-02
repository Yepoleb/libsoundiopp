#include <string>
#include <vector>
#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"
#include "templates.h"

namespace sio
{
    Device::Device()
    {
        m_device = nullptr;
        m_context = nullptr;
    }

    Device::Device(SoundIoDevice* device, Context* context)
    {
        if (device->probe_error) {
            throw soundio_error(device->probe_error);
        }

        m_device = device;
        m_context = context;
    }

    Device::Device(const Device& other)
    {
        m_device = other.m_device;
        m_context = other.m_context;
        soundio_device_ref(m_device);
    }

    Device& Device::operator=(const Device& other)
    {
        if (&other == this) {
            return *this;
        }
        if (m_device != nullptr) {
            soundio_device_unref(m_device);
        }
        m_device = other.m_device;
        m_context = other.m_context;
        soundio_device_ref(m_device);
        return *this;
    }

    Device::~Device()
    {
        soundio_device_unref(m_device);
    }

    Device::operator SoundIoDevice*()
    {
        return m_device;
    }

    Device::operator const SoundIoDevice*() const
    {
        return m_device;
    }

    bool Device::equal(const Device& other)
    {
        return soundio_device_equal(m_device, other);
    }

    void Device::sort_channel_layouts()
    {
        soundio_device_sort_channel_layouts(m_device);
    }

    bool Device::supports_format(FormatId format)
    {
        return soundio_device_supports_format(
            m_device, static_cast<SoundIoFormat>(format));
    }

    bool Device::supports_layout(const ChannelLayout& layout)
    {
        return soundio_device_supports_layout(m_device, layout);
    }

    bool Device::supports_sample_rate(int sample_rate)
    {
        return soundio_device_supports_sample_rate(m_device, sample_rate);
    }

    int Device::nearest_sample_rate(int sample_rate)
    {
        return soundio_device_nearest_sample_rate(m_device, sample_rate);
    }

    OutStream Device::create_outstream()
    {
        return OutStream(soundio_outstream_create(m_device), this);
    }

    InStream Device::create_instream()
    {
        return InStream(soundio_instream_create(m_device), this);
    }

    // Getters/Setters

    Context* Device::get_context()
    {
        return m_context;
    }

    std::string Device::get_id() const
    {
        return m_device->id;
    }

    std::string Device::get_name() const
    {
        return m_device->name;
    }

    DeviceAimId Device::get_aim() const
    {
        return static_cast<DeviceAimId>(m_device->aim);
    }

    std::vector<ChannelLayout> Device::get_layouts()
    {
        return array_to_vector<ChannelLayout>(
            m_device->layouts, m_device->layout_count);
    }

    void Device::sort_layouts()
    {
        soundio_sort_channel_layouts(m_device->layouts, m_device->layout_count);
    }

    int Device::get_layout_count() const
    {
        return m_device->layout_count;
    }

    ChannelLayout Device::get_current_layout() const
    {
        return ChannelLayout(m_device->current_layout);
    }

    std::vector<FormatId> Device::get_formats() const
    {
        return array_to_vector<FormatId>(
            m_device->formats, m_device->format_count);
    }

    int Device::get_format_count() const
    {
        return m_device->format_count;
    }

    FormatId Device::get_current_format() const
    {
        return static_cast<FormatId>(m_device->current_format);
    }

    std::vector<SampleRateRange> Device::get_sample_rates() const
    {
        return array_to_vector<SampleRateRange>(
            m_device->sample_rates, m_device->sample_rate_count);
    }

    int Device::get_sample_rate_count() const
    {
        return m_device->sample_rate_count;
    }

    int Device::get_sample_rate_current() const
    {
        return m_device->sample_rate_count;
    }

    double Device::get_software_latency_min() const
    {
        return m_device->software_latency_min;
    }

    double Device::get_software_latency_max() const
    {
        return m_device->software_latency_max;
    }

    double Device::get_software_latency_current() const
    {
        return m_device->software_latency_current;
    }

    bool Device::is_raw() const
    {
        return m_device->is_raw;
    }
}
