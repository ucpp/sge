#ifndef _SGE_CONFIG_H_
#define _SGE_CONFIG_H_

#include "pch.h"
#include "json.hpp"
#include "sge_application_settings.h"
#include "sge_helpers.h"
#include "sge_logger.h"
#include <type_traits>

namespace SGE
{
    template <typename T, typename = void>
    struct has_to_json : std::false_type {};

    template <typename T>
    struct has_to_json<T, std::void_t<decltype(to_json(std::declval<nlohmann::json&>(), std::declval<T>()))>> : std::true_type {};

    template <typename T, typename = void>
    struct has_from_json : std::false_type {};

    template <typename T>
    struct has_from_json<T, std::void_t<decltype(from_json(std::declval<const nlohmann::json&>(), std::declval<T&>()))>> : std::true_type {};

    template <typename T>
    struct IsSerializable : std::integral_constant<bool, has_to_json<T>::value && has_from_json<T>::value> {};

    class Config
    {
    public:
        template <typename T>
        static bool Load(const std::string& filePath, T& data)
        {
            static_assert(IsSerializable<T>::value, "Type does not support serialization");

            std::ifstream inputFile(filePath);
            Verify(inputFile, "Failed to open config file: " + filePath);
            Verify(inputFile.peek() != std::ifstream::traits_type::eof(), "Config file is empty: " + filePath);

            nlohmann::json jsonData;
            try
            {
                inputFile >> jsonData;
                data = jsonData.get<T>();
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Failed to load config file: {}", filePath);
                LOG_ERROR(e.what());
                return false;
            }

            LOG_INFO("Config file loaded: {}", filePath);
            return true;
        }

        template <typename T>
        static bool Save(const std::string& filePath, const T& data)
        {
            static_assert(IsSerializable<T>::value, "Type does not support serialization");

            std::ofstream outputFile(filePath);
            Verify(outputFile, "Failed to open config file: " + filePath);

            nlohmann::json jsonData = data;
            try
            {
                outputFile << jsonData.dump(4);
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Failed to save config file: {}", filePath);
                LOG_ERROR(e.what());
                return false;
            }

            LOG_INFO("Config file saved: {}", filePath);
            return true;
        }
    };

    void to_json(nlohmann::json& j, const WindowSettings& settings);
    void to_json(nlohmann::json& j, const RenderSettings& settings);
    void to_json(nlohmann::json& j, const ApplicationSettings& settings);
    void to_json(nlohmann::json& j, const EditorSettings& settings);

    void from_json(const nlohmann::json& j, WindowSettings& settings);
    void from_json(const nlohmann::json& j, RenderSettings& settings);
    void from_json(const nlohmann::json& j, ApplicationSettings& settings);
    void from_json(const nlohmann::json& j, EditorSettings& settings);

    void to_json(nlohmann::json& j, const AssetBase& settings);
    void from_json(const nlohmann::json& j, AssetBase& settings);
    void to_json(nlohmann::json& j, const ProjectAssets& settings);
    void from_json(const nlohmann::json& j, ProjectAssets& settings);
}

#endif // !_SGE_CONFIG_H_
