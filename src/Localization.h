#ifndef BP_LOCALIZATION_H
#define BP_LOCALIZATION_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <functional>

/**
 * @brief Advanced localization system with pluralization and formatting support
 */
class Localization {
public:
    /**
     * @brief String format specification
     */
    enum class FormatType {
        String, ///< Regular string
        Integer, ///< Integer value
        Float, ///< Floating point value
        Date, ///< Date value
        Currency ///< Currency value
    };

    /**
     * @brief Plural rule function type
     * @param n The number to check for plurality
     * @return Index of plural form to use
     */
    using PluralRule = std::function<int(int)>;

    /**
     * @brief Localization config for a language
     */
    struct LanguageConfig {
        std::string code; ///< Language code (e.g., "en", "fr")
        std::string name; ///< Language name (e.g., "English", "French")
        std::string dateFormat = "%x"; ///< Default date format
        std::string timeFormat = "%X"; ///< Default time format
        std::string numberFormat = "%.2f"; ///< Default number format
        PluralRule pluralRule; ///< Plural rule function
    };

    /**
     * @brief Constructor with optional default language
     * @param defaultLanguage Default language code
     */
    explicit Localization(std::string defaultLanguage = "en");

    /**
     * @brief Destructor
     */
    ~Localization() = default;

    /**
     * @brief Set active language
     * @param languageCode Language code to set
     * @return true if language exists and was set
     */
    bool SetLanguage(const std::string &languageCode);

    /**
     * @brief Get active language code
     * @return Current language code
     */
    std::string GetLanguage() const { return m_CurrentLanguage; }

    /**
     * @brief Register a new language
     * @param config Language configuration
     * @return true if added successfully
     */
    bool RegisterLanguage(const LanguageConfig &config);

    /**
     * @brief Check if a language is registered
     * @param languageCode Language code to check
     * @return true if language is registered
     */
    bool HasLanguage(const std::string &languageCode) const;

    /**
     * @brief Get list of available languages
     * @return Vector of language codes
     */
    std::vector<std::string> GetAvailableLanguages() const;

    /**
     * @brief Add or update a localized string
     * @param key String key
     * @param value Localized value
     * @param languageCode Optional language code (uses current if empty)
     */
    void SetString(const std::string &key, const std::string &value,
                   const std::string &languageCode = "");

    /**
     * @brief Get localized string, falls back to key if not found
     * @param key String key
     * @param args Format arguments (if any)
     * @return Localized and formatted string
     */
    template <typename... Args>
    std::string GetString(const std::string &key, Args &&... args) const {
        // Get the raw string
        std::string value = GetRawString(key);

        // If no formatting args, return as is
        if (sizeof...(Args) == 0) {
            return value;
        } else {
            // Format the string with arguments
            return Format(value, std::forward<Args>(args)...);
        }
    }

    /**
     * @brief Get raw string without formatting
     * @param key String key
     * @return Raw localized string
     */
    std::string GetRawString(const std::string &key) const;

    /**
     * @brief Translate shorthand (alias for GetString)
     * @param key String key
     * @param args Format arguments (if any)
     * @return Translated string
     */
    template <typename... Args>
    std::string Tr(const std::string &key, Args &&... args) const {
        return GetString(key, std::forward<Args>(args)...);
    }

    /**
     * @brief Format with pluralization support
     * @param key Base key
     * @param count Item count
     * @param args Additional format arguments
     * @return Plural-aware formatted string
     */
    template <typename... Args>
    std::string Plural(const std::string &key, int count, Args &&... args) const {
        // Get plural form index based on count
        int pluralForm = GetPluralForm(count);

        // Try to get the specific plural form key
        std::string pluralKey = key + "." + std::to_string(pluralForm);
        if (HasKey(pluralKey)) {
            return GetString(pluralKey, count, std::forward<Args>(args)...);
        }

        // Fall back to the base key
        return GetString(key, count, std::forward<Args>(args)...);
    }

    /**
     * @brief Check if a key exists in current language
     * @param key String key to check
     * @return true if key exists
     */
    bool HasKey(const std::string &key) const;

    /**
     * @brief Format a date according to locale settings
     * @param timestamp Unix timestamp
     * @param format Optional custom format (uses locale default if empty)
     * @return Formatted date string
     */
    std::string FormatDate(time_t timestamp, const std::string &format = "") const;

    /**
     * @brief Format a number according to locale settings
     * @param value Number to format
     * @param precision Decimal precision
     * @return Formatted number string
     */
    std::string FormatNumber(double value, int precision = -1) const;

    /**
     * @brief Load strings from a file
     * @param filePath Path to the localization file
     * @param languageCode Optional language code (autodetects if empty)
     * @return true if loaded successfully
     */
    bool LoadFromFile(const std::string &filePath, const std::string &languageCode = "");

    /**
     * @brief Save strings to a file
     * @param filePath Path to save to
     * @param languageCode Language to save (current if empty)
     * @return true if saved successfully
     */
    bool SaveToFile(const std::string &filePath, const std::string &languageCode = "") const;

private:
    // Current language
    std::string m_CurrentLanguage;

    // Registered languages and their configs
    std::unordered_map<std::string, LanguageConfig> m_Languages;

    // Localized strings by language and key
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings;

    /**
     * @brief Get full key with language prefix
     * @param key Base key
     * @param languageCode Language code (uses current if empty)
     * @return Full key
     */
    std::string GetFullKey(const std::string &key, const std::string &languageCode = "") const;

    /**
     * @brief Get plural form index for a number
     * @param n Number to check
     * @return Plural form index
     */
    int GetPluralForm(int n) const;

    /**
     * @brief Format a string with arguments (recursive variadic template)
     * @param format Format string
     * @param arg First argument
     * @param args Remaining arguments
     * @return Formatted string
     */
    template <typename T, typename... Args>
    std::string Format(const std::string &format, T &&arg, Args &&... args) const {
        // Find the first format specifier
        size_t pos = format.find("{}");
        if (pos == std::string::npos) {
            return format;
        }

        // Replace with the arg
        std::string result = format.substr(0, pos) +
            ToString(std::forward<T>(arg)) +
            format.substr(pos + 2);

        // Continue with remaining args
        if (sizeof...(Args) > 0) {
            return Format(result, std::forward<Args>(args)...);
        } else {
            return result;
        }
    }

    /**
     * @brief Format base case (no more arguments)
     * @param format Format string
     * @return Unmodified format string
     */
    std::string Format(const std::string &format) const {
        return format;
    }

    /**
     * @brief Convert value to string
     * @param value Value to convert
     * @return String representation
     */
    template <typename T>
    std::string ToString(const T &value) const {
        // Use stringstream for general types
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    /**
     * @brief Initialize default plural rules
     */
    void InitDefaultPluralRules();

    /**
     * @brief Parse a file to detect the language code
     * @param filePath Path to the file
     * @return Detected language code
     */
    std::string DetectLanguageFromFile(const std::string &filePath) const;
};

// Specializations for common types
template <>
inline std::string Localization::ToString(const std::string &value) const {
    return value;
}

template <>
inline std::string Localization::ToString(const char *const&value) const {
    return value;
}

template <>
inline std::string Localization::ToString(const double &value) const {
    return FormatNumber(value);
}

template <>
inline std::string Localization::ToString(const float &value) const {
    return FormatNumber(value);
}

#endif // BP_LOCALIZATION_H
