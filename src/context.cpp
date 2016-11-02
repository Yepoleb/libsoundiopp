#include <string>
#include <functional>
#include "soundio/soundio.h"
#include "soundiopp/soundiopp.h"

namespace sio
{
    Context::Context()
    {
        m_soundio = soundio_create();
        if (m_soundio == nullptr) {
            throw std::bad_alloc();
        }
        m_userdata = nullptr;
    }

    Context::Context(SoundIo* soundio)
    {
        m_soundio = soundio;
        m_userdata = m_soundio->userdata;
        m_soundio->userdata = this;
    }

    Context::Context(Context&& other)
    {
        m_soundio = other.m_soundio;
        m_app_name = other.m_app_name;
        m_userdata = other.m_userdata;
        m_on_devices_change = other.m_on_devices_change;
        m_on_backend_disconnect = other.m_on_backend_disconnect;
        m_on_events_signal = other.m_on_events_signal;
        m_soundio->userdata = this;
        other.m_soundio = nullptr;
    }

    Context& Context::operator=(Context&& other)
    {
        if (&other == this) {
            return *this;
        }
        if (m_soundio != nullptr) {
            // Destroy already existing object
            soundio_destroy(m_soundio);
        }

        m_soundio = other.m_soundio;
        m_app_name = other.m_app_name;
        m_userdata = other.m_userdata;
        m_on_devices_change = other.m_on_devices_change;
        m_on_backend_disconnect = other.m_on_backend_disconnect;
        m_on_events_signal = other.m_on_events_signal;
        m_soundio->userdata = this;
        other.m_soundio = nullptr;
        return *this;
    }

    Context::~Context()
    {
        if (m_soundio != nullptr) {
            soundio_destroy(m_soundio);
        }
    }

    Context::operator SoundIo*() const {
        return m_soundio;
    }

    void Context::connect()
    {
        WRAP_SOUNDIO_ERROR(soundio_connect(m_soundio));
    }

    void Context::connect_backend(SoundIoBackend backend)
    {
        WRAP_SOUNDIO_ERROR(soundio_connect_backend(m_soundio, backend));
    }

    void Context::disconnect()
    {
        soundio_disconnect(m_soundio);
    }

    int Context::default_input_device_index()
    {
        return soundio_default_input_device_index(m_soundio);
    }

    int Context::default_output_device_index()
    {
        return soundio_default_output_device_index(m_soundio);
    }

    void Context::flush_events()
    {
        soundio_flush_events(m_soundio);
    }

    void Context::wait_events()
    {
        soundio_wait_events(m_soundio);
    }

    void Context::wakeup()
    {
        soundio_wakeup(m_soundio);
    }

    void Context::force_device_scan()
    {
        soundio_force_device_scan(m_soundio);
    }

    int Context::input_device_count()
    {
        return soundio_input_device_count(m_soundio);
    }

    int Context::output_device_count()
    {
        return soundio_output_device_count(m_soundio);
    }

    Device Context::get_input_device(int index)
    {
        return Device(soundio_get_input_device(m_soundio, index), this);
    }

    Device Context::get_output_device(int index)
    {
        return Device(soundio_get_output_device(m_soundio, index), this);
    }

    RingBuffer Context::create_ring_buffer(int requested_capacity)
    {
        return RingBuffer(soundio_ring_buffer_create(
            m_soundio, requested_capacity));
    }

    // Getters/Setters

    void* Context::get_userdata()
    {
        return m_userdata;
    }

    void Context::set_userdata(void* userdata)
    {
        m_userdata = userdata;
    }

    BackendId Context::get_current_backend() const
    {
        return static_cast<BackendId>(m_soundio->current_backend);
    }

    std::string Context::get_app_name() const
    {
        return m_soundio->app_name;
    }

    void Context::set_app_name(const std::string& name)
    {
        m_app_name = name;
        m_soundio->app_name = name.c_str();
    }

    std::function<void(Context*)> Context::get_on_devices_change()
    {
        return m_on_devices_change;
    }

    void Context::set_on_devices_change(
        std::function<void(Context*)> on_devices_change)
    {
        m_on_devices_change = on_devices_change;
        m_soundio->on_devices_change = on_devices_change_wrapper;
    }

    std::function<void(Context*, ErrorId)> Context::get_on_backend_disconnect()
    {
        return m_on_backend_disconnect;
    }

    void Context::set_on_backend_disconnect(
        std::function<void(Context*, ErrorId)> on_backend_disconnect)
    {
        m_on_backend_disconnect = on_backend_disconnect;
        m_soundio->on_backend_disconnect = on_backend_disconnect_wrapper;
    }

    std::function<void(Context*)> Context::get_on_events_signal()
    {
        return m_on_events_signal;
    }

    void Context::set_on_events_signal(
        std::function<void(Context*)> on_events_signal)
    {
        m_on_events_signal = on_events_signal;
        m_soundio->on_events_signal = on_events_signal_wrapper;
    }

    emit_rtprio_callback_t Context::get_emit_rtprio_warning()
    {
        return m_soundio->emit_rtprio_warning;
    }

    void Context::set_emit_rtprio_warning(
        emit_rtprio_callback_t emit_rtprio_warning)
    {
        m_soundio->emit_rtprio_warning = emit_rtprio_warning;
    }

    jack_callback_t Context::get_jack_info_callback()
    {
        return m_soundio->jack_info_callback;
    }

    void Context::set_jack_info_callback(
        jack_callback_t jack_info_callback)
    {
        m_soundio->jack_info_callback = jack_info_callback;
    }

    jack_callback_t Context::get_jack_error_callback()
    {
        return m_soundio->jack_error_callback;
    }

    void Context::set_jack_error_callback(
        jack_callback_t jack_error_callback)
    {
        m_soundio->jack_error_callback = jack_error_callback;
    }

    void Context::on_devices_change_wrapper(SoundIo* soundio)
    {
        Context* context = static_cast<Context*>(soundio->userdata);
        auto cb = context->get_on_devices_change();
        cb(context);
    }

    void Context::on_backend_disconnect_wrapper(SoundIo* soundio, int err)
    {
        Context* context = static_cast<Context*>(soundio->userdata);
        auto cb = context->get_on_backend_disconnect();
        cb(context, static_cast<ErrorId>(err));
    }

    void Context::on_events_signal_wrapper(SoundIo* soundio)
    {
        Context* context = static_cast<Context*>(soundio->userdata);
        auto cb = context->get_on_events_signal();
        cb(context);
    }
}
