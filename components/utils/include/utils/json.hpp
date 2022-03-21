// Copyright 2022 Borys Chyliński

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef B2H_UTILS_JSON_HPP
#define B2H_UTILS_JSON_HPP

#include <fstream>
#include <string>
#include <string_view>

#include "rapidjson/document.h"

namespace b2h::utils::json
{
    /**
     * @brief Dump JSON to std::string.
     *
     * @param document
     * @return std::string
     */
    std::string dump(const rapidjson::Document& document) noexcept;

    /**
     * @brief Parse JSON from std::string_view object.
     *
     * @param str
     * @return rapidjson::Document
     */
    rapidjson::Document parse(std::string_view str) noexcept;

    /**
     * @brief Parse JSON from file.
     *
     * @param path
     * @return rapidjson::Document
     */
    rapidjson::Document parse_file(const char* path) noexcept;

    /**
     * @brief Namespace for JSON predicates.
     *
     */
    namespace pred
    {
        /**
         * @brief Check if given JSON has parse error.
         *
         * @param document
         * @return true
         * @return false
         */
        inline bool has_parse_error(
            const rapidjson::Document& document) noexcept
        {
            return document.HasParseError();
        }

        /**
         * @brief Check if given JSON is an array.
         *
         * @param document
         * @return true
         * @return false
         */
        inline bool is_array(const rapidjson::Document& document) noexcept
        {
            return document.IsArray();
        }

        /**
         * @brief Check if given JSON is an object.
         *
         * @param document
         * @return true
         * @return false
         */
        inline bool is_object(const rapidjson::Document& document) noexcept
        {
            return document.IsObject();
        }

        /**
         * @brief Create predicate for object containing a given key.
         *
         * @param key
         * @return lambda
         */
        inline auto get_has_key(std::string_view key) noexcept
        {
            return
                [key{ std::string(key) }](const rapidjson::Document& document) {
                    assert(is_object(document));
                    return document.HasMember(key);
                };
        }
    } // namespace pred
} // namespace b2h::utils::json

#endif