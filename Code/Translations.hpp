#pragma once
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include "Logger.hpp"
#include "GTAV-Classes-master/rage/joaat.hpp"
#include "RobinHood.hpp"

namespace base {
    class TranslationManager {
    public:
        explicit TranslationManager() {
            try {
                if (!std::filesystem::exists(m_TranslationDir))
                    std::filesystem::create_directory(m_TranslationDir);
            }
            catch (const std::filesystem::filesystem_error& ex) {
                std::cerr << "Error: " << ex.what() << '\n';
            }
        }

        void LoadTranslations(const std::string& name) {
            m_Translations.clear(); // Clear translations
           ankerl::unordered_dense::map<std::uint32_t, std::string> translations;
            try {
                std::string filename = name + ".json";
                std::filesystem::path filePath = std::filesystem::path(m_TranslationDir) / filename;
                std::ifstream file(filePath);
                if (file.is_open()) {
                    g_logger->send(Darkcyan, "Translation", "Loading {} translation.", name);
                    std::stringstream str;
                    str << file.rdbuf();
                    nlohmann::json json = nlohmann::json::parse(str);
                    translations.reserve(json.size()); // Reserve space for expected translations
                    for (const auto& [key, value] : json.items()) {
                        translations.emplace(rage::joaat(key.c_str()), value.get<std::string>());
                    }
                }
                else {
                    throw std::runtime_error("Failed to open: " + name + " translation");
                }
                m_Translations = std::move(translations); // Move translations to member variable
            }
            catch (const std::exception& e) {
                g_logger->send(Darkred, "Translation", "Failed to parse/find {} translation. To fix this issue try to re-open " shortBrandingName " Launcher and let it check for updates!", name);
            }
        }

        const char* GetTranslation(std::uint32_t label) {
            try {
                auto it = m_Translations.find(label);
                if (it != m_Translations.end()) {
                    return it->second.c_str();
                }
                else {
                    return std::format("Failed to load (0x{:08X})", label).c_str();
                }
            }
            catch (const std::exception& ex) {
                g_logger->send(red, "Translation", "{}", ex.what());
                return nullptr;
            }
        }

        ~TranslationManager() noexcept = default;
        TranslationManager(TranslationManager const&) = delete;
        TranslationManager& operator=(TranslationManager&&) = delete;

    private:
        ankerl::unordered_dense::map<std::uint32_t, std::string> m_Translations{};
        std::string m_TranslationDir = R"(C:\LightningFiles\Translations)";
    };

    inline std::unique_ptr<TranslationManager> g_TranslationManager;
}

#define TRANSLATE_IMPL(label) (::base::g_TranslationManager->GetTranslation(constexprJoaat(label)))
#define TRANSLATE(label) TRANSLATE_IMPL(label)
