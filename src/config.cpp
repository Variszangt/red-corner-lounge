#include "config.h"

#include <fstream>

#include "log.h"

/*------------------------------------------------------------------*/

void Config::load(const std::string& filename)
{
    LOG_INFO("loading configuration from '{}'...", filename);

    std::ifstream ifstr { filename };
    if (!ifstr)
    {
        LOG_WARNING("config file could not be opened for loading: {}", filename);
        return;
    }

    try
    {
        nlohmann::json json;
        ifstr >> json;
        *this = json.get<Config>();

        nlohmann::json reverse_json;
        reverse_json = *this;
        LOG_INFO("loaded configuration:\n{}", reverse_json.dump(4));
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("json exception: {}", e.what());
        return;
    }
}

void Config::save(const std::string& filename)
{
    LOG_INFO("saving configuration to '{}'...", filename);

    std::ofstream ofstr { filename };
    if (!ofstr)
    {
        LOG_ERROR("config file could not be opened for saving: {}", filename);
        return;
    }

    try
    {
        nlohmann::json json;
        json = *this;
        ofstr << json.dump(4);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("json exception: {}", e.what());
        return;
    }
}