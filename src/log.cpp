#include "log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/details/fmt_helper.h>
#include <spdlog/details/os.h>

#include <chrono>
#include <filesystem>
#include <map>
#include <fstream>

#include "utility.h"

namespace fs = std::filesystem;

/*------------------------------------------------------------------*/
// Constants:

const std::string LOGS_DIR = "logs";

/*------------------------------------------------------------------*/

void remove_old_logs()
{
    static constexpr int logs_to_keep = 10;


    if (fs::exists(LOGS_DIR) && fs::is_directory(LOGS_DIR))
    {
        using file_time_t = std::chrono::system_clock::rep;

        std::map<file_time_t, fs::path> time_sorted_logs;

        fs::directory_iterator end;
        for (fs::directory_iterator it { LOGS_DIR }; it != end; ++it)
        {
            if (fs::is_regular_file(it->status()))
            {
                const file_time_t write_time =
                    fs::last_write_time(it->path()).time_since_epoch().count();
                time_sorted_logs.insert(std::make_pair(write_time, *it));
            }
        }

        const int log_count = static_cast<int>(time_sorted_logs.size());
        const int logs_to_remove =  log_count - logs_to_keep;
        auto it = time_sorted_logs.begin();
        for (int i = 0; i < logs_to_remove; ++i)
        {
            try
            {
                fs::remove(it->second);
                ++it;
            }
            catch (const std::exception& e)
            {
                LOG_WARNING("old log could not be removed: {}; exception: {}", it->second.string(), e.what());
            }
        }
    }
}

/*------------------------------------------------------------------*/
// Custom formatter:

namespace spdlog
{
// This formatter is a modified version of:
// \spdlog\pattern_formatter-inl.h -> class full_formatter;
// Pattern: [time] [type] [optional_trace] message
class DefaultFormatter final : public formatter
{
public:
    void format(const details::log_msg& msg, memory_buf_t& dest) override
    {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        using std::chrono::seconds;

        // cache the date/time part for the next second.
        const auto duration = msg.time.time_since_epoch();
        const auto secs = duration_cast<seconds>(duration);

        if (cache_timestamp_ != secs || cached_datetime_.size() == 0)
        {
            cached_tm_ = details::os::localtime(log_clock::to_time_t(msg.time));
            cache_timestamp_ = secs;

            cached_datetime_.clear();
            cached_datetime_.push_back('[');

            // date
            // fmt_helper::append_int(tm_time.tm_year + 1900, cached_datetime_);
            // cached_datetime_.push_back('-');

            // fmt_helper::pad2(tm_time.tm_mon + 1, cached_datetime_);
            // cached_datetime_.push_back('-');

            // fmt_helper::pad2(tm_time.tm_mday, cached_datetime_);
            // cached_datetime_.push_back(' ');

            // time
            details::fmt_helper::pad2(cached_tm_.tm_hour, cached_datetime_);
            cached_datetime_.push_back(':');

            details::fmt_helper::pad2(cached_tm_.tm_min, cached_datetime_);
            cached_datetime_.push_back(':');

            details::fmt_helper::pad2(cached_tm_.tm_sec, cached_datetime_);
            cached_datetime_.push_back('.');
        }
        dest.append(cached_datetime_.begin(), cached_datetime_.end());

        const auto millis = details::fmt_helper::time_fraction<milliseconds>(msg.time);
        details::fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
        dest.push_back(']');
        dest.push_back(' ');

        // logger name
        // if (msg.logger_name.size() > 0)
        // {
        //     dest.push_back('[');
        //     details::fmt_helper::append_string_view(msg.logger_name, dest);
        //     dest.push_back(']');
        //     dest.push_back(' ');
        // }

        msg.color_range_start = dest.size();

        // level
        dest.push_back('[');
        details::fmt_helper::append_string_view(level::to_string_view(msg.level), dest);
        dest.push_back(']');
        dest.push_back(' ');

        // source location
        if (!msg.source.empty())
        {
            dest.push_back('[');
            const char* filename = std::strrchr(msg.source.filename, details::os::folder_sep) + 1;
            details::fmt_helper::append_string_view(filename, dest);
            dest.push_back(':');
            details::fmt_helper::append_int(msg.source.line, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }

        msg.color_range_end = dest.size();

        // message
        details::fmt_helper::append_string_view(msg.payload, dest);
        dest.push_back('\n');
    }

    std::unique_ptr<formatter> clone() const override
    {
        return std::unique_ptr<formatter>(static_cast<formatter*>(new DefaultFormatter()));
    }

private:
    std::tm cached_tm_;
    std::chrono::seconds cache_timestamp_ { 0 };
    memory_buf_t cached_datetime_;
};
}

/*------------------------------------------------------------------*/

// Returns a sink to a file. File will be placed in LOGS_DIR and named using system time.
// Returns an empty pointer if no sink could be opened.
auto open_file_sink()
{
    std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;

    // Try to open a sink using system time as filename:
    try
    {
        const auto time = std::time(nullptr);
        const auto local_time = localtime_xp(time);
        char buf[64];
        std::string timestamp = { buf, std::strftime(buf, sizeof(buf), "%F_%T", &local_time) };
        std::replace(timestamp.begin(), timestamp.end(), ':', '_');
        std::replace(timestamp.begin(), timestamp.end(), '-', '_');

        std::filesystem::path path = LOGS_DIR;
        path /= timestamp + ".txt";

        const auto path_str = path.string();
        file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path_str, true);
        LOG_INFO("file sink successfully opened; log will be saved to '{}'", path_str);
    }
    // Try to open a sink using a predefined string as filename:
    catch (const std::exception& e)
    {
        try
        {
            std::filesystem::path path = LOGS_DIR;
            path /= "unknown_timestamp.txt";

            const auto path_str = path.string();
            file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path_str, true);

            LOG_ERROR("file sink with timestamp could not be opened; log will be saved to '{}' instead; exception: {}",
                path_str, e.what());
        }
        // Sink could not be opened. Create an empty file to indicate failure:
        catch (const std::exception& e2)
        {
            std::filesystem::path path = LOGS_DIR;
            path /= "FILE_SINK_COULD_NOT_BE_OPENED";

            const auto path_str = path.string();
            std::ofstream o { path_str };

            LOG_ERROR("file sink could not be opened; no log will be saved to drive; exception 1: {}; exception 2: {}",
                e.what(), e2.what());
        }
    }

    return file_sink;
}

void init_logger()
{
    remove_old_logs();

    auto default_logger = std::make_shared<spdlog::logger>("default");
    default_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(default_logger);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_formatter(std::make_unique<spdlog::DefaultFormatter>());
    default_logger->sinks().push_back(console_sink);

    auto file_sink = open_file_sink();
    if (file_sink)
    {
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_formatter(std::make_unique<spdlog::DefaultFormatter>());
        default_logger->sinks().push_back(file_sink);
    }
}