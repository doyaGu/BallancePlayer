#include "CmdlineParser.h"

#include <ctype.h>
#include <string.h>

bool CmdlineArg::GetValue(int i, XString &value) const {
    if (i >= m_Size || i < 0)
        return false;

    const XString &val = m_Values[i];
    const int sz = val.Length();

    bool inQuote = false;

    if (!m_Jointed) {
        // Non-jointed value
        if (sz == 0 || val[0] != '"') {
            value = val;
        } else {
            value = "";
            value.Resize(sz);
            int k = 0;

            for (int vi = 0; vi < sz; ++vi) {
                if (val[vi] == '"') {
                    inQuote = !inQuote;
                    continue;
                }
                if (inQuote)
                    value[k++] = val[vi];
            }

            value.Resize(k);
        }
    } else {
        // Jointed value (--option=value1;value2)
        value = "";
        value.Resize(sz);
        int k = 0;

        // Find the position after '='
        int equalsPos = val.Find('=');
        if (equalsPos < 0) return false;
        int pos = equalsPos + 1;

        // Find the i-th value
        int valueIdx = 0;
        while (valueIdx < i && pos < sz) {
            if (val[pos] == ';') {
                valueIdx++;
            }
            pos++;
        }

        if (valueIdx != i || pos >= sz) return false;

        // Extract the value
        if (val[pos] == '"') {
            // Quoted value
            pos++; // Skip the opening quote
            inQuote = true;

            while (pos < sz && (inQuote || val[pos] != ';')) {
                if (val[pos] == '"') {
                    inQuote = !inQuote;
                    pos++;
                } else if (inQuote) {
                    value[k++] = val[pos++];
                } else {
                    pos++; // Skip characters between closing quote and semicolon
                }
            }
        } else {
            // Unquoted value
            while (pos < sz && val[pos] != ';') {
                value[k++] = val[pos++];
            }
        }

        value.Resize(k);
    }

    return true;
}

bool CmdlineArg::GetValue(int i, long &value) const {
    if (i >= m_Size || i < 0)
        return false;

    const XString &v = m_Values[i];
    const char *s = v.CStr();
    const char *e = s + v.Length();
    long val = strtol(s, const_cast<char **>(&e), 10);
    if (s == e)
        return false;
    value = val;
    return true;
}

int CmdlineArg::GetSize() const {
    return m_Size;
}

CmdlineParser::CmdlineParser(int argc, char **argv) : m_Index(0) {
    for (int i = 1; i < argc; ++i)
        m_Args.PushBack(argv[i]);
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount) {
    if (Done())
        return false;

    const XString &s = m_Args[m_Index];
    const int sz = s.Length();

    // Argument must start with '-'
    if (sz < 2 || s[0] != '-')
        return false;

    bool match = false;
    int optLen = 0;

    if (opt != '\0' && isalnum(opt) && s[1] == opt) {
        // Check for short option match (-o)
        match = true;
        optLen = 2;
    } else if (longopt != NULL) {
        // Check for long option match (--longopt)
        int longOptLen = (int) strlen(longopt);

        // Validate long option name
        for (int l = 0; l < longOptLen; ++l) {
            if (!isalnum(longopt[l]) && longopt[l] != '-' && longopt[l] != '_')
                return false;
        }

        // Check if it's a long option (--longopt)
        if (sz >= longOptLen + 2 && s[1] == '-' && strncmp(s.CStr() + 2, longopt, longOptLen) == 0) {
            match = true;
            optLen = longOptLen + 2;
        }
    }

    if (!match)
        return false;

    // Handle values
    if (maxValueCount != 0) {
        // Case 1: Values are joined with the option (--option=value1;value2)
        if (sz > optLen && s[optLen] == '=') {
            arg = CmdlineArg(&s, 1, true);
            ++m_Index;
            return true;
        }

        // Case 2: Values follow the option as separate arguments
        if (maxValueCount == -1)
            maxValueCount = m_Args.Size() - m_Index - 1;

        const XString *values = &m_Args[m_Index];
        int valueCount = 0;

        ++m_Index; // Skip the option itself

        // Collect values that don't start with '-'
        while (m_Index < m_Args.Size() && valueCount < maxValueCount) {
            const XString &next = m_Args[m_Index];
            if (next.Length() > 0 && next[0] == '-')
                break;

            ++valueCount;
            ++m_Index;
        }

        arg = CmdlineArg(values, valueCount + 1);
        return true;
    }

    ++m_Index;
    return true;
}

bool CmdlineParser::Skip() {
    if (m_Index < m_Args.Size()) {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const {
    return m_Index >= m_Args.Size();
}

void CmdlineParser::Reset() {
    m_Index = 0;
}
