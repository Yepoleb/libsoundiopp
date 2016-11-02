#include <string>
#include <vector>
#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"
#include "templates.h"

namespace sio
{
    ChannelLayout::ChannelLayout()
    {
        m_layout = {nullptr, 0, {}};
    }

    ChannelLayout::ChannelLayout(
        const std::string& name, const std::vector<ChannelId>& channels)
    {
        set_name(name);
        set_channels(channels);
    }

    ChannelLayout::ChannelLayout(const SoundIoChannelLayout layout)
    {
        m_layout = layout;
    }

    ChannelLayout::operator SoundIoChannelLayout*()
    {
        return &m_layout;
    }

    ChannelLayout::operator const SoundIoChannelLayout*() const
    {
        return &m_layout;
    }

    ChannelLayout::operator SoundIoChannelLayout() const
    {
        return m_layout;
    }

    bool ChannelLayout::equal(const ChannelLayout& other)
    {
        return soundio_channel_layout_equal(
            &m_layout,
            static_cast<const SoundIoChannelLayout*>(other)
        );
    }

    int ChannelLayout::builtin_count()
    {
        return soundio_channel_layout_builtin_count();
    }

    ChannelLayout ChannelLayout::get_builtin(int index)
    {
        return ChannelLayout(*soundio_channel_layout_get_builtin(index));
    }

    ChannelLayout ChannelLayout::get_default(int channel_count)
    {
        return ChannelLayout(
            *soundio_channel_layout_get_default(channel_count)
        );
    }

    int ChannelLayout::find_channel(SoundIoChannelId channel)
    {
        return soundio_channel_layout_find_channel(&m_layout, channel);
    }

    bool ChannelLayout::detect_builtin()
    {
        return soundio_channel_layout_detect_builtin(&m_layout);
    }

    ChannelLayout ChannelLayout::best_matching_channel_layout(
        const std::vector<ChannelLayout>& prefered_layouts,
        const std::vector<ChannelLayout>& available_layouts)
    {
        // There's no simple way to wrap this because the layouts have to
        // be converted and stored as an array somewhere
        std::vector<SoundIoChannelLayout> prefered_structs(
            prefered_layouts.begin(), prefered_layouts.end());
        std::vector<SoundIoChannelLayout> available_structs(
            available_layouts.begin(), available_layouts.end());

        return ChannelLayout(*soundio_best_matching_channel_layout(
            prefered_structs.data(), prefered_structs.size(),
            available_structs.data(), available_structs.size()));
    }

    ChannelLayout ChannelLayout::best_matching_channel_layout(
        const ChannelLayout* prefered_layouts,
        size_t prefered_layouts_count,
        const ChannelLayout* available_layouts,
        size_t available_layouts_count)
    {
        auto prefered_structs(array_to_vector<SoundIoChannelLayout>(
            prefered_layouts, prefered_layouts_count));
        auto available_structs(array_to_vector<SoundIoChannelLayout>(
            available_layouts, available_layouts_count));

        return ChannelLayout(*soundio_best_matching_channel_layout(
            prefered_structs.data(), prefered_layouts_count,
            available_structs.data(), available_layouts_count));
    }

    void ChannelLayout::sort(
        std::vector<ChannelLayout>& layouts)
    {
        // Ugly
        std::vector<SoundIoChannelLayout> layout_structs(
            layouts.begin(), layouts.end());
        soundio_sort_channel_layouts(
            layout_structs.data(), layout_structs.size());
        for (size_t i = 0; i < layouts.size(); i++) {
            layouts[i] = ChannelLayout(layout_structs[i]);
        }
    }

    void ChannelLayout::sort(
        ChannelLayout* layouts, size_t layout_count)
    {
        auto layout_structs(array_to_vector<SoundIoChannelLayout>(
            layouts, layout_count));
        soundio_sort_channel_layouts(
            layout_structs.data(), layout_structs.size());
        for (size_t i = 0; i < layout_count; i++) {
            layouts[i] = ChannelLayout(layout_structs[i]);
        }
    }

    // Getters/Setters

    std::string ChannelLayout::get_name() const
    {
        return std::string(m_layout.name);
    }

    void ChannelLayout::set_name(std::string name)
    {
        m_name = name;
        m_layout.name = m_name.c_str();
    }

    int ChannelLayout::get_channel_count() const
    {
        return m_layout.channel_count;
    }

    void ChannelLayout::set_channel_count(int channel_count)
    {
        m_layout.channel_count = channel_count;
    }

    std::vector<ChannelId> ChannelLayout::get_channels() const
    {
        return array_to_vector<ChannelId>(
            m_layout.channels, m_layout.channel_count);
    }

    void ChannelLayout::set_channels(const std::vector<ChannelId>& channels)
    {
        vector_to_array(channels, m_layout.channels, SOUNDIO_MAX_CHANNELS);
        m_layout.channel_count = channels.size();
    }
}
