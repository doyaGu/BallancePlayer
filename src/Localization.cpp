#include "Localization.h"

#include <fstream>
#include <sstream>
#include <locale>
#include <ctime>
#include <regex>
#include <cctype>
#include <utility>

Localization::Localization(std::string defaultLanguage)
    : m_CurrentLanguage(std::move(defaultLanguage)) {
    InitDefaultPluralRules();
}

bool Localization::SetLanguage(const std::string &languageCode) {
    if (!HasLanguage(languageCode)) {
        return false;
    }

    m_CurrentLanguage = languageCode;
    return true;
}

bool Localization::RegisterLanguage(const LanguageConfig &config) {
    if (config.code.empty()) {
        return false;
    }

    // Add or update language config
    m_Languages[config.code] = config;

    // Initialize string map for this language if needed
    if (m_Strings.find(config.code) == m_Strings.end()) {
        m_Strings[config.code] = std::unordered_map<std::string, std::string>();
    }

    return true;
}

bool Localization::HasLanguage(const std::string &languageCode) const {
    return m_Languages.find(languageCode) != m_Languages.end();
}

std::vector<std::string> Localization::GetAvailableLanguages() const {
    std::vector<std::string> languages;
    languages.reserve(m_Languages.size());

    for (const auto &pair : m_Languages) {
        languages.push_back(pair.first);
    }

    return languages;
}

void Localization::SetString(const std::string &key, const std::string &value,
                             const std::string &languageCode) {
    // Use provided language code or current language
    std::string targetLanguage = languageCode.empty() ? m_CurrentLanguage : languageCode;

    if (!HasLanguage(targetLanguage)) {
        // Register language with default config if it doesn't exist
        LanguageConfig config;
        config.code = targetLanguage;
        config.name = targetLanguage; // Default name to code
        RegisterLanguage(config);
    }

    m_Strings[targetLanguage][key] = value;
}

std::string Localization::GetRawString(const std::string &key) const {
    // Try current language
    auto langIt = m_Strings.find(m_CurrentLanguage);
    if (langIt != m_Strings.end()) {
        auto strIt = langIt->second.find(key);
        if (strIt != langIt->second.end()) {
            return strIt->second;
        }
    }

    // Try default language if different from current
    if (m_CurrentLanguage != "en") {
        langIt = m_Strings.find("en");
        if (langIt != m_Strings.end()) {
            auto strIt = langIt->second.find(key);
            if (strIt != langIt->second.end()) {
                return strIt->second;
            }
        }
    }

    // Return key as fallback
    return key;
}

bool Localization::HasKey(const std::string &key) const {
    // Check in current language
    auto langIt = m_Strings.find(m_CurrentLanguage);
    if (langIt != m_Strings.end()) {
        if (langIt->second.find(key) != langIt->second.end()) {
            return true;
        }
    }

    // Check default language if different
    if (m_CurrentLanguage != "en") {
        langIt = m_Strings.find("en");
        if (langIt != m_Strings.end()) {
            return langIt->second.find(key) != langIt->second.end();
        }
    }

    return false;
}

std::string Localization::FormatDate(time_t timestamp, const std::string &format) const {
    // Get language config
    auto langIt = m_Languages.find(m_CurrentLanguage);
    if (langIt == m_Languages.end()) {
        langIt = m_Languages.find("en"); // Fallback to English
    }

    // Use provided format or default from language config
    std::string dateFormat = format.empty() ? langIt->second.dateFormat : format;

    // Format the date
    char buffer[128];
    struct tm timeInfo;

#ifdef _WIN32
    localtime_s(&timeInfo, &timestamp);
#else
    localtime_r(&timestamp, &timeInfo);
#endif

    strftime(buffer, sizeof(buffer), dateFormat.c_str(), &timeInfo);
    return std::string(buffer);
}

std::string Localization::FormatNumber(double value, int precision) const {
    // Get language config
    auto langIt = m_Languages.find(m_CurrentLanguage);
    if (langIt == m_Languages.end()) {
        langIt = m_Languages.find("en"); // Fallback to English
    }

    // Use provided precision or default from language config
    std::string format = langIt->second.numberFormat;
    if (precision >= 0) {
        format = "%." + std::to_string(precision) + "f";
    }

    // Format the number
    char buffer[64];
    snprintf(buffer, sizeof(buffer), format.c_str(), value);
    return std::string(buffer);
}

bool Localization::LoadFromFile(const std::string &filePath, const std::string &languageCode) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    // Determine language code to use
    std::string targetLanguage = languageCode;
    if (targetLanguage.empty()) {
        targetLanguage = DetectLanguageFromFile(filePath);
    }

    // Ensure language exists
    if (!HasLanguage(targetLanguage)) {
        LanguageConfig config;
        config.code = targetLanguage;
        config.name = targetLanguage;
        RegisterLanguage(config);
    }

    // Load strings
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Find key-value separator (either = or :)
        size_t separatorPos = line.find('=');
        if (separatorPos == std::string::npos) {
            separatorPos = line.find(':');
            if (separatorPos == std::string::npos) {
                continue; // Skip lines without separator
            }
        }

        // Extract key and value
        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Handle escape sequences in value
        size_t pos = 0;
        while ((pos = value.find("\\n", pos)) != std::string::npos) {
            value.replace(pos, 2, "\n");
            pos += 1;
        }

        pos = 0;
        while ((pos = value.find("\\t", pos)) != std::string::npos) {
            value.replace(pos, 2, "\t");
            pos += 1;
        }

        // Store the string
        SetString(key, value, targetLanguage);
    }

    return true;
}

bool Localization::SaveToFile(const std::string &filePath, const std::string &languageCode) const {
    // Determine language to save
    std::string targetLanguage = languageCode.empty() ? m_CurrentLanguage : languageCode;

    // Check if language exists
    auto langIt = m_Strings.find(targetLanguage);
    if (langIt == m_Strings.end()) {
        return false;
    }

    // Open file for writing
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    // Write header
    file << "# " << targetLanguage << " language file" << std::endl;
    file << "# Generated on " << FormatDate(time(nullptr), "%Y-%m-%d %H:%M:%S") << std::endl;
    file << std::endl;

    // Write strings
    for (const auto &pair : langIt->second) {
        std::string value = pair.second;

        // Escape newlines and tabs
        size_t pos = 0;
        while ((pos = value.find('\n', pos)) != std::string::npos) {
            value.replace(pos, 1, "\\n");
            pos += 2;
        }

        pos = 0;
        while ((pos = value.find('\t', pos)) != std::string::npos) {
            value.replace(pos, 1, "\\t");
            pos += 2;
        }

        file << pair.first << " = " << value << std::endl;
    }

    return true;
}

std::string Localization::GetFullKey(const std::string &key, const std::string &languageCode) const {
    std::string targetLanguage = languageCode.empty() ? m_CurrentLanguage : languageCode;
    return targetLanguage + "." + key;
}

int Localization::GetPluralForm(int n) const {
    // Get plural rule for current language
    auto langIt = m_Languages.find(m_CurrentLanguage);
    if (langIt != m_Languages.end() && langIt->second.pluralRule) {
        return langIt->second.pluralRule(n);
    }

    // Default English rule (singular/plural)
    return n == 1 ? 0 : 1;
}

void Localization::InitDefaultPluralRules() {
    // Register English (default)
    LanguageConfig en;
    en.code = "en";
    en.name = "English";
    en.pluralRule = [](int n) -> int { return n == 1 ? 0 : 1; };
    RegisterLanguage(en);

    // Register French
    LanguageConfig fr;
    fr.code = "fr";
    fr.name = "Français";
    fr.pluralRule = [](int n) -> int { return n <= 1 ? 0 : 1; };
    RegisterLanguage(fr);

    // Register German
    LanguageConfig de;
    de.code = "de";
    de.name = "Deutsch";
    de.pluralRule = [](int n) -> int { return n == 1 ? 0 : 1; };
    RegisterLanguage(de);

    // Register Spanish
    LanguageConfig es;
    es.code = "es";
    es.name = "Español";
    es.pluralRule = [](int n) -> int { return n == 1 ? 0 : 1; };
    RegisterLanguage(es);

    // Register Russian (has 3 plural forms)
    LanguageConfig ru;
    ru.code = "ru";
    ru.name = "Русский";
    ru.pluralRule = [](int n) -> int {
        if (n % 10 == 1 && n % 100 != 11) return 0;
        if (n % 10 >= 2 && n % 10 <= 4 && (n % 100 < 10 || n % 100 >= 20)) return 1;
        return 2;
    };
    RegisterLanguage(ru);

    // Register Japanese (no plurals)
    LanguageConfig ja;
    ja.code = "ja";
    ja.name = "日本語";
    ja.pluralRule = [](int) -> int { return 0; };
    RegisterLanguage(ja);

    // Register Chinese (no plurals)
    LanguageConfig zh;
    zh.code = "zh";
    zh.name = "中文";
    zh.pluralRule = [](int) -> int { return 0; };
    RegisterLanguage(zh);
}

std::string Localization::DetectLanguageFromFile(const std::string &filePath) const {
    // Try to detect language from file content or name

    // Extract filename without path
    std::string filename = filePath;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    // Check filename for language code
    std::regex langPattern("^([a-z]{2})(?:[_-]|\\.)");
    std::smatch match;
    if (std::regex_search(filename, match, langPattern)) {
        return match[1].str();
    }

    // Read first few lines to detect language
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string line;
        for (int i = 0; i < 5 && std::getline(file, line); i++) {
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                // Check comment for language info
                std::regex commentPattern("language[: =]+([a-z]{2})");
                if (std::regex_search(line, match, commentPattern)) {
                    return match[1].str();
                }
            }
        }
    }

    // Default to English if detection fails
    return "en";
}
