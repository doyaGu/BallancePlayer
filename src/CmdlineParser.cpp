#include "CmdlineParser.h"

#include <ctype.h>
#include <string.h>

bool CmdlineArg::GetValue(int i, XString &value) const
{
    if (i >= m_Size || i < 0)
        return false;

    const XString &val = m_Values[i];
    const int sz = val.Length();

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
            value.Resize(sz);
            int k = 0;

            for (int vi = 0; vi < sz; ++vi)
            {
                if (val[vi] == '"')
                {
                    inQuote = !inQuote;
                    continue;
                }
                if (inQuote)
                    value[k++] = val[vi];
            }

            value.Resize(k);
        }
    }
    else
    {
        value = "";
        value.Resize(sz);
        int k = 0;

        int n = 0;
        int foundPos = val.Find('=');
        if (foundPos == XString::NOTFOUND)
            return false;
        int j = foundPos + 1;
        for (int vi = j; vi < sz; ++vi)
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

        value.Resize(k);
    }

    return true;
}

bool CmdlineArg::GetValue(int i, long &value) const
{
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

int CmdlineArg::GetSize() const
{
    return m_Size;
}

CmdlineParser::CmdlineParser(int argc, char **argv) : m_Index(0)
{
    for (int i = 1; i < argc; ++i)
        m_Args.PushBack(argv[i]);
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount)
{
    if (Done())
        return false;

    const XString &s = m_Args[m_Index];
    const int sz = s.Length();
    if (sz < 2 || s[0] != '-')
        return false;

    bool match = false;

    int optLen = 2;
    if (opt != '\0' && isalnum(opt) &&
        s[0] == '-' && s[1] == opt)
        match = true;

    if (!match)
    {
        if (!longopt) return false;

        optLen = (int)strlen(longopt);
        if (optLen > 0)
        {
            for (int l = 0; l < optLen; ++l)
                if (!isalnum(longopt[l]) && longopt[l] != '-' && longopt[l] != '_')
                    return false;
        }

        if (strncmp(s.CStr(), longopt, optLen) != 0)
            return false;
    }

    if (maxValueCount != 0)
    {
        const XString *values = NULL;
        int valueCount = 0;

        if (sz > optLen + 1 && s[optLen] == '=')
        {
            values = &s;
            ++valueCount;
            int j = optLen + 1;
            for (int vi = j; vi < sz; ++vi)
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
                maxValueCount = (int)(m_Args.Size() - m_Index);

            ++m_Index; // Skip the option itself
            if (m_Index >= m_Args.Size() || maxValueCount == 0)
            {
                arg = CmdlineArg(NULL, 0);
            }
            else
            {
                values = &m_Args[m_Index]; // Start from after the option
                while (valueCount < maxValueCount && m_Index < m_Args.Size())
                {
                    const XString &next = m_Args[m_Index];
                    if (next.Length() != 0 && next[0] == '-')
                        break;
                    ++m_Index;
                    ++valueCount;
                }
                arg = CmdlineArg(values, valueCount);
            }
        }
    }

    ++m_Index;
    return true;
}

bool CmdlineParser::Skip()
{
    if (m_Index < m_Args.Size())
    {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const
{
    return m_Index >= m_Args.Size();
}

void CmdlineParser::Reset()
{
    m_Index = 0;
}
