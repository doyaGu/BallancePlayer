#include "CmdlineParser.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

bool CmdlineArg::GetValue(int i, std::string &value) const {
    // Validate index
    if (i >= m_Size || i < 0 || !m_Values)
        return false;

    const std::string &val = m_Values[i];
    const size_t sz = val.length();

    // Early out for empty strings
    if (sz == 0) {
        value = "";
        return true;
    }

    // Handle differently based on whether this is a jointed argument
    if (!m_Jointed) {
        // Non-jointed value (separate arguments)
        if (val[0] != '"') {
            // Unquoted value - use as is
            value = val;
            return true;
        }

        // Handle quoted value
        value.clear();
        value.reserve(sz);
        bool inQuote = false;

        for (size_t vi = 0; vi < sz; ++vi) {
            char c = val[vi];

            if (c == '"') {
                inQuote = !inQuote;
                continue;
            }

            // Only include characters inside quotes
            if (inQuote || val[0] != '"')
                value.push_back(c);
        }

        return true;
    } else {
        // Jointed value (--option=value1;value2)

        // Find the position of the equals sign
        size_t equalsPos = val.find('=');
        if (equalsPos == std::string::npos || equalsPos >= sz - 1)
            return false;

        // Find the start position of the requested value
        size_t startPos = equalsPos + 1;
        int valueCount = 0;

        // Skip to the i-th value
        while (valueCount < i && startPos < sz) {
            // Find the next semicolon
            bool inValueQuote = false;
            while (startPos < sz) {
                if (val[startPos] == '"')
                    inValueQuote = !inValueQuote;

                if (val[startPos] == ';' && !inValueQuote)
                    break;

                startPos++;
            }

            if (startPos >= sz)
                return false; // Not enough values

            startPos++; // Skip the semicolon
            valueCount++;
        }

        if (valueCount != i || startPos >= sz)
            return false;

        // Extract the value
        value.clear();
        value.reserve(sz);
        bool inQuote = false;

        // Check if this value is quoted
        if (val[startPos] == '"') {
            inQuote = true;
            startPos++; // Skip opening quote
        }

        // Extract characters until the next semicolon or end of string
        while (startPos < sz) {
            char c = val[startPos++];

            if (c == '"' && inQuote) {
                // End of quoted value
                break;
            }

            if (c == ';' && !inQuote) {
                // End of unquoted value
                break;
            }

            value.push_back(c);
        }

        return true;
    }
}

bool CmdlineArg::GetValue(int i, long &value) const {
    // Validate index and values array
    if (i >= m_Size || i < 0 || !m_Values)
        return false;

    // Get the string value
    std::string strValue;
    if (!GetValue(i, strValue) || strValue.empty())
        return false;

    // Convert to long
    const char *s = strValue.c_str();
    char *endPtr = nullptr;

    // Handle possible leading spaces
    while (*s && isspace(*s)) s++;

    // Check for empty string after trimming
    if (!*s) return false;

    // Parse the number
    value = strtol(s, &endPtr, 10);

    // Ensure the entire string was consumed
    while (endPtr && *endPtr && isspace(*endPtr)) endPtr++;

    if (!endPtr || *endPtr != '\0')
        return false;

    return true;
}

int CmdlineArg::GetSize() const {
    return m_Size;
}

CmdlineParser::CmdlineParser(int argc, char **argv) : m_Index(0) {
    // Skip the program name (argv[0])
    m_Args.reserve(argc > 0 ? argc - 1 : 0);
    for (int i = 1; i < argc; ++i) {
        if (argv[i]) {
            m_Args.push_back(argv[i]);
        }
    }
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount) {
    if (Done())
        return false;

    const std::string &s = m_Args[m_Index];
    const size_t sz = s.length();

    // Argument must start with '-' and have at least one character after it
    if (sz < 2 || s[0] != '-')
        return false;

    bool match = false;
    size_t optLen = 0;

    // Check for short option (-o)
    if (opt != '\0' && std::isalnum(opt) && sz >= 2 && s[1] == opt) {
        match = true;
        optLen = 2;
    }

    // Check for long option (--longopt)
    else if (longopt != nullptr && longopt[0] != '\0') {
        const size_t longOptLen = strlen(longopt);

        // Validate long option format
        if (sz >= longOptLen + 2 && s[1] == '-' &&
            s.compare(2, longOptLen, longopt) == 0) {
            // Ensure this is the full option name (followed by =, end of string, or whitespace)
            if (sz == longOptLen + 2 || s[longOptLen + 2] == '=' || std::isspace(s[longOptLen + 2])) {
                match = true;
                optLen = longOptLen + 2;
            }
        }
    }

    if (!match)
        return false;

    // Handle values based on maxValueCount
    if (maxValueCount != 0) {
        // Check for jointed values (--option=value1;value2)
        if (sz > optLen && s[optLen] == '=') {
            arg = CmdlineArg(&s, 1, true);
            ++m_Index;
            return true;
        }

        // Handle separate values
        if (maxValueCount < 0) {
            // Unlimited values - collect all consecutive non-option arguments
            maxValueCount = static_cast<int>(m_Args.size()) - m_Index - 1;
        }

        // Store the original option
        const std::string *values = &m_Args[m_Index];
        int valueCount = 0;

        // Move past the option
        ++m_Index;

        // Collect values that don't start with '-'
        while (m_Index < static_cast<int>(m_Args.size()) && valueCount < maxValueCount) {
            const std::string &next = m_Args[m_Index];

            // Stop at the next option
            if (!next.empty() && next[0] == '-')
                break;

            ++valueCount;
            ++m_Index;
        }

        // Create the argument with the option and its values
        arg = CmdlineArg(values, valueCount + 1);
        return true;
    }

    // Flag option with no values
    ++m_Index;
    arg = CmdlineArg(&s, 1);
    return true;
}

bool CmdlineParser::Skip() {
    if (m_Index < static_cast<int>(m_Args.size())) {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const {
    return m_Index >= static_cast<int>(m_Args.size());
}

void CmdlineParser::Reset() {
    m_Index = 0;
}
