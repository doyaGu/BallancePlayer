#include "CmdlineParser.h"

#include <string.h>

bool CmdlineArg::GetValue(int i, std::string &value) const
{
    if (i >= m_Size || i < 0)
        return false;

    const std::string &val = m_Values[i];
    const size_t sz = val.size();

    bool inQuote = false;

    if (!m_Jointed)
    {
        if (val[0] != '"')
        {
            value = val;
        }
        else
        {
            value = "";
            value.resize(sz, '\0');
            size_t k = 0;

            for (size_t vi = 0; vi < sz; ++vi)
            {
                {
                    if (val[vi] == '"')
                    {
                        inQuote = !inQuote;
                        continue;
                    }
                    if (inQuote)
                        value[k++] = val[vi];
                }
            }

            value.resize(k);
        }
    }
    else
    {
        value = "";
        value.resize(sz, '\0');
        size_t k = 0;

        int n = 0;
        size_t j = val.find('=') + 1;
        for (size_t vi = j; vi < sz; ++vi)
        {
            if (val[vi] == ';' && vi != j)
            {
                ++n;
                j = vi + 1;
                continue;
            }

            if (i == n)
            {
                if (val[vi] != '"')
                {
                    for (; val[vi] != ';' && vi < sz; ++vi)
                        value[k++] = val[vi];
                }
                else
                {
                    for (; val[vi] != ';' && vi < sz; ++vi)
                    {
                        if (val[vi] == '"')
                        {
                            inQuote = !inQuote;
                            continue;
                        }
                        if (inQuote)
                            value[k++] = val[vi];
                    }
                }
            }
        }

        value.resize(k);
    }

    return true;
}

bool CmdlineArg::GetValue(int i, long &value) const
{
    if (i >= m_Size || i < 0)
        return false;

    const std::string &v = m_Values[i];
    const char *s = v.c_str();
    const char *e = s + v.size();
    long val = strtol(s, const_cast<char **>(&e), 10);
    if (s == e)
        return false;
    value = val;
    return true;
}

int CmdlineArg::GetSize() const
{
    return m_Size;
}

CmdlineParser::CmdlineParser(int argc, char **argv) : m_Index(0)
{
    for (int i = 1; i < argc; ++i)
        m_Args.push_back(argv[i]);
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount)
{
    if (Done())
        return false;

    const std::string &s = m_Args[m_Index];
    const size_t sz = s.size();
    if (sz < 2 || s[0] != '-')
        return false;

    bool match = false;

    size_t optLen = 2;
    if (opt != '\0' && !isalnum(opt) &&
        s[0] == '-' && s[1] == opt)
        match = true;

    if (!match)
    {
        if (!longopt) return false;

        optLen = strlen(longopt);
        if (optLen > 0)
        {
            for (size_t l = 0; l < optLen; ++l)
                if (!isalnum(longopt[l]) && longopt[l] != '-' && longopt[l] != '_')
                    return false;
        }

        if (strncmp(s.c_str(), longopt, optLen) != 0)
            return false;
    }

    if (maxValueCount != 0)
    {
        const std::string *values = NULL;
        int valueCount = 0;

        if (sz > optLen + 1 && s[optLen] == '=')
        {
            values = &s;
            ++valueCount;
            size_t j = optLen + 1;
            for (size_t vi = j; vi < sz; ++vi)
            {
                if (s[vi] == ';' && vi != j)
                {
                    ++valueCount;
                    j = vi + 1;
                }
            }
            arg = CmdlineArg(values, valueCount, true);
        }
        else
        {
            if (maxValueCount == -1)
                maxValueCount = (int)(m_Args.size() - m_Index);

            values = &m_Args[m_Index];
            while (valueCount < maxValueCount)
            {
                const std::string &next = m_Args[m_Index];
                if (!next.empty() && next[0] == '-')
                    break;
                ++m_Index;
                ++valueCount;
            }
            arg = CmdlineArg(values, valueCount);
        }
    }

    ++m_Index;
    return true;
}

bool CmdlineParser::Skip()
{
    if (m_Index < m_Args.size())
    {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const
{
    return m_Index >= m_Args.size();
}

void CmdlineParser::Reset()
{
    m_Index = 0;
}
