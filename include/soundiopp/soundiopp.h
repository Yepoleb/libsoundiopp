#ifndef SOUNDIOPP_H
#define SOUNDIOPP_H
#include <stdexcept>
#include <vector>
#include <string>
#include <functional>

#include "soundio/soundio.h"

#include "enums.h"

#define WRAP_SOUNDIO_ERROR(f) if (int err = f) throw soundio_error(err)

namespace sio
{
    typedef SoundIoChannelArea ChannelArea;
    typedef SoundIoSampleRateRange SampleRateRange;

    typedef void (*emit_rtprio_callback_t)();
    typedef void (*jack_callback_t)(const char*);

    class soundio_error;
    class Context;
    class ChannelLayout;
    class Device;
    class OutStream;
    class InStream;
    class RingBuffer;

    int get_bytes_per_sample(FormatId format);
    int get_bytes_per_frame(FormatId format, int channel_count);
    int get_bytes_per_second(FormatId format, int channel_count, int sample_rate);
    const char* error_name(ErrorId err);
    const char* format_name(FormatId format);
    const char* backend_name(BackendId backend);
    const char* channel_name(ChannelId channel);
    int backend_count();
    bool have_backend(BackendId backend);
    SoundIoBackend get_backend(int index);

    class soundio_error : public std::exception
    {
    public:
        explicit soundio_error(int err);
        explicit soundio_error(ErrorId err);
        virtual const char* what() const noexcept;
    protected:
        ErrorId m_error;
    };

    class Context
    {
    public:
        Context();
        Context(SoundIo* soundio);
        Context(Context&& other);
        Context& operator=(Context&& other);
        ~Context();
        operator SoundIo*() const;
        void connect();
        void connect_backend(SoundIoBackend backend);
        void disconnect();
        int default_input_device_index();
        int default_output_device_index();
        void flush_events();
        void wait_events();
        void wakeup();
        void force_device_scan();
        int input_device_count();
        int output_device_count();
        Device get_input_device(int index);
        Device get_output_device(int index);
        RingBuffer create_ring_buffer(int requested_capacity);

        void* get_userdata();
        void set_userdata(void* userdata);
        BackendId get_current_backend() const;
        std::string get_app_name() const;
        void set_app_name(const std::string& name);

        std::function<void(Context*)> get_on_devices_change();
        void set_on_devices_change(
            std::function<void(Context*)> on_devices_change);
        std::function<void(Context*, ErrorId)> get_on_backend_disconnect();
        void set_on_backend_disconnect(
            std::function<void(Context*, ErrorId)> on_backend_disconnect);
        std::function<void(Context*)> get_on_events_signal();
        void set_on_events_signal(
            std::function<void(Context*)> on_events_signal);
        emit_rtprio_callback_t get_emit_rtprio_warning();
        void set_emit_rtprio_warning(
            emit_rtprio_callback_t emit_rtprio_warning);
        jack_callback_t get_jack_info_callback();
        void set_jack_info_callback(jack_callback_t jack_info_callback);
        jack_callback_t get_jack_error_callback();
        void set_jack_error_callback(jack_callback_t jack_error_callback);
    private:
        static void on_devices_change_wrapper(SoundIo* soundio);
        static void on_backend_disconnect_wrapper(SoundIo* soundio, int err);
        static void on_events_signal_wrapper(SoundIo* soundio);

        SoundIo* m_soundio;
        std::string m_app_name;
        void* m_userdata;

        std::function<void(Context*)> m_on_devices_change;
        std::function<void(Context*, ErrorId)> m_on_backend_disconnect;
        std::function<void(Context*)> m_on_events_signal;
    };

    class ChannelLayout
    {
    public:
        ChannelLayout();
        ChannelLayout(
            const std::string& name, const std::vector<ChannelId>& channels);
        ChannelLayout(const SoundIoChannelLayout layout);
        operator SoundIoChannelLayout*();
        operator const SoundIoChannelLayout*() const;
        operator SoundIoChannelLayout() const;
        bool equal(const ChannelLayout& other);
        int builtin_count();
        static ChannelLayout get_builtin(int index);
        static ChannelLayout get_default(int channel_count);
        int find_channel(SoundIoChannelId channel);
        bool detect_builtin();
        static ChannelLayout best_matching_channel_layout(
            const std::vector<ChannelLayout>& prefered_layouts,
            const std::vector<ChannelLayout>& available_layouts);
        static ChannelLayout best_matching_channel_layout(
            const ChannelLayout* prefered_layouts,
            size_t prefered_layouts_count,
            const ChannelLayout* available_layouts,
            size_t available_layouts_count);
        static void sort(std::vector<ChannelLayout>& layouts);
        static void sort(ChannelLayout* layouts, size_t layout_count);

        std::string get_name() const;
        void set_name(std::string name);
        int get_channel_count() const;
        void set_channel_count(int channel_count);
        std::vector<ChannelId> get_channels() const;
        void set_channels(const std::vector<ChannelId>& channels);
    private:
        SoundIoChannelLayout m_layout;
        std::string m_name;
    };

    class Device
    {
    public:
        Device();
        Device(SoundIoDevice* device, Context* context);
        Device(const Device& other);
        Device& operator=(const Device& other);
        ~Device();
        operator SoundIoDevice*();
        operator const SoundIoDevice*() const;
        bool equal(const Device& other);
        void sort_channel_layouts();
        bool supports_format(FormatId format);
        bool supports_layout(const ChannelLayout& layout);
        bool supports_sample_rate(int sample_rate);
        int nearest_sample_rate(int sample_rate);
        OutStream create_outstream();
        InStream create_instream();

        Context* get_context();
        std::string get_id() const;
        std::string get_name() const;
        DeviceAimId get_aim() const;
        std::vector<ChannelLayout> get_layouts();
        void sort_layouts();
        // TODO: set_layouts?
        int get_layout_count() const;
        ChannelLayout get_current_layout() const;
        std::vector<FormatId> get_formats() const;
        int get_format_count() const;
        FormatId get_current_format() const;
        std::vector<SampleRateRange> get_sample_rates() const;
        int get_sample_rate_count() const;
        int get_sample_rate_current() const;
        double get_software_latency_min() const;
        double get_software_latency_max() const;
        double get_software_latency_current() const;
        bool is_raw() const;
    private:
        SoundIoDevice* m_device;
        Context* m_context;
    };

    class OutStream
    {
    public:
        OutStream();
        OutStream(SoundIoOutStream* outstream, Device* device);
        OutStream(OutStream&& other);
        OutStream& operator=(OutStream&& other);
        ~OutStream();
        operator SoundIoOutStream*();
        operator const SoundIoOutStream*() const;
        void open();
        void start();
        int begin_write(ChannelArea*& areas, int frame_count);
        void end_write();
        void clear_buffer();
        void pause(bool paused);
        double get_latency();

        Device* get_device();
        FormatId get_format() const;
        void set_format(FormatId format);
        int get_sample_rate() const;
        void set_sample_rate(int sample_rate);
        ChannelLayout get_layout() const;
        void set_layout(const ChannelLayout& layout);
        double get_software_latency() const;
        void set_software_latency(double software_latency);
        void* get_userdata();
        void set_userdata(void* userdata);
        std::string get_name() const;
        void set_name(std::string name);
        bool get_non_terminal_hint() const;
        void set_non_terminal_hint(bool hint);
        int get_bytes_per_frame() const;
        int get_bytes_per_sample() const;

        std::function<void(OutStream*, int, int)> get_write_callback();
        void set_write_callback(
            std::function<void(OutStream*, int, int)> write_callback);
        std::function<void(OutStream*)> get_underflow_callback();
        void set_underflow_callback(
            std::function<void(OutStream*)> underflow_callback);
        std::function<void(OutStream*, int)> get_error_callback();
        void set_error_callback(
            std::function<void(OutStream*, int)> error_callback);
    private:
        static void write_callback_wrapper(
            SoundIoOutStream* stream, int frame_count_min, int frame_count_max);
        static void underflow_callback_wrapper(
            SoundIoOutStream* stream);
        static void error_callback_wrapper(
            SoundIoOutStream* stream, int err);

        SoundIoOutStream* m_outstream;
        Device* m_device;
        void* m_userdata;
        std::string m_name;
        std::function<void(OutStream*, int, int)> m_write_callback;
        std::function<void(OutStream*)> m_underflow_callback;
        std::function<void(OutStream*, int)> m_error_callback;
    };

    class InStream
    {
    public:
        InStream();
        InStream(SoundIoInStream* instream, Device* device);
        InStream(InStream&& other);
        InStream& operator=(InStream&& other);
        ~InStream();
        operator SoundIoInStream*();
        operator const SoundIoInStream*() const;
        void open();
        void start();
        int begin_read(ChannelArea*& areas, int frame_count);
        void end_read();
        void pause(bool paused);
        double get_latency();

        Device* get_device();
        FormatId get_format() const;
        void set_format(FormatId format);
        int get_sample_rate() const;
        void set_sample_rate(int sample_rate);
        ChannelLayout get_layout() const;
        void set_layout(const ChannelLayout& layout);
        double get_software_latency() const;
        void set_software_latency(double software_latency);
        void* get_userdata();
        void set_userdata(void* userdata);
        std::string get_name() const;
        void set_name(std::string name);
        bool get_non_terminal_hint() const;
        void set_non_terminal_hint(bool hint);
        int get_bytes_per_frame() const;
        int get_bytes_per_sample() const;

        std::function<void(InStream*, int, int)> get_read_callback();
        void set_read_callback(
            std::function<void(InStream*, int, int)> read_callback);
        std::function<void(InStream*)> get_overflow_callback();
        void set_overflow_callback(
            std::function<void(InStream*)> overflow_callback);
        std::function<void(InStream*, int)> get_error_callback();
        void set_error_callback(
            std::function<void(InStream*, int)> error_callback);
    private:
        static void read_callback_wrapper(
            SoundIoInStream* stream, int frame_count_min, int frame_count_max);
        static void overflow_callback_wrapper(
            SoundIoInStream* stream);
        static void error_callback_wrapper(
            SoundIoInStream* stream, int err);

        SoundIoInStream* m_instream;
        Device* m_device;
        void* m_userdata;
        std::string m_name;
        std::function<void(InStream*, int, int)> m_read_callback;
        std::function<void(InStream*)> m_overflow_callback;
        std::function<void(InStream*, int)> m_error_callback;
    };

    class RingBuffer
    {
    public:
        RingBuffer();
        RingBuffer(SoundIoRingBuffer* ringbuffer);
        RingBuffer(RingBuffer&& other);
        RingBuffer& operator=(RingBuffer&& other);
        ~RingBuffer();
        int capacity();
        char* write_ptr();
        void advance_write_ptr(int count);
        char* read_ptr();
        void advance_read_ptr(int count);
        int fill_count();
        int free_count();
        void clear();
    private:
        SoundIoRingBuffer* m_ringbuffer;
    };
}

#endif // SOUNDIOPP_H
