#include "utils/json.hpp"

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace b2h::utils::json
{
    std::string dump(const rapidjson::Document& document) noexcept
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        return std::string(buffer.GetString(), buffer.GetSize());
    }

    rapidjson::Document parse(std::string_view str) noexcept
    {
        rapidjson::Document result;
        result.Parse(str.data(), str.length());
        return result;
    }

    rapidjson::Document parse_file(const char* path) noexcept
    {
        rapidjson::Document file;
        std::ifstream ifs(path);

        if (!ifs)
        {
            return file;
        }

        {
            rapidjson::IStreamWrapper json_stream(ifs);
            file.ParseStream(json_stream);
        }

        return file;
    }
} // namespace b2h::utils::json