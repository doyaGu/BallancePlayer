#include "CmdlineParser.h"

#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

static XString RemoveQuotes(const XString &text)
{
    XString value;
    bool inQuote = false;

    for (int i = 0; i < (int)text.Length(); ++i)
    {
        if (text[i] == '"')
        {
            inQuote = !inQuote;
            continue;
        }
        value += text[i];
    }

    return value;
}

static bool IsOptionLike(const XString &text)
{
    if (text.Length() < 2 || text[0] != '-')
        return false;

    return !isdigit(static_cast<unsigned char>(text[1]));
}

static int CountJointedValues(const XString &text, int optLen)
{
    int count = 1;
    int start = optLen + 1;

    for (int i = start; i < (int)text.Length(); ++i)
    {
        if (text[i] == ';' && i != start)
        {
            ++count;
            start = i + 1;
        }
    }

    return count;
}

static void AppendCommandLineArgs(XClassArray<XString> &args, const char *cmdline)
{
    if (!cmdline)
        return;

    const char *p = cmdline;
    while (*p)
    {
        while (*p && isspace(static_cast<unsigned char>(*p)))
            ++p;

        if (!*p)
            break;

        XString arg;
        bool inQuote = false;

        while (*p)
        {
            if (*p == '"')
            {
                inQuote = !inQuote;
                ++p;
                continue;
            }

            if (!inQuote && isspace(static_cast<unsigned char>(*p)))
                break;

            arg += *p;
            ++p;
        }

        args.PushBack(arg);
    }
}

bool CmdlineArg::GetValue(int i, XString &value) const
{
    if (i < 0 || (!m_Jointed && i >= m_Size) || !m_Values)
        return false;

    const XString &val = m_Jointed ? m_Values[0] : m_Values[i];

    if (!m_Jointed)
    {
        value = RemoveQuotes(val);
    }
    else
    {
        XWORD foundPos = val.Find('=');
        if (foundPos == XString::NOTFOUND)
            return false;

        int n = 0;
        int start = (int)foundPos + 1;
        int end = start;

        for (; end <= (int)val.Length(); ++end)
        {
            if (end == (int)val.Length() || (val[end] == ';' && end != start))
            {
                if (i == n)
                {
                    value = RemoveQuotes(val.Substring((XWORD)start, (XWORD)(end - start)));
                    return true;
                }
                ++n;
                start = end + 1;
            }
        }

        return false;
    }

    return true;
}

bool CmdlineArg::GetValue(int i, long &value) const
{
    if (i < 0 || (!m_Jointed && i >= m_Size) || !m_Values)
        return false;

    XString v;
    if (!GetValue(i, v))
        return false;

    const char *s = v.CStr();
    char *e = NULL;
    long val = strtol(s, &e, 10);
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
        m_Args.PushBack(XString(argv[i]));
}

CmdlineParser::CmdlineParser(const char *cmdline) : m_Index(0)
{
    AppendCommandLineArgs(m_Args, cmdline);
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount)
{
    arg = CmdlineArg();

    if (Done())
        return false;

    const XString &s = m_Args[m_Index];
    const int sz = (int)s.Length();
    if (sz < 2 || s[0] != '-')
        return false;

    bool match = false;

    int optLen = 0;
    if (opt != '\0' && isalnum(opt) &&
        s[0] == '-' && s[1] == opt &&
        (sz == 2 || s[2] == '='))
    {
        match = true;
        optLen = 2;
    }

    if (!match)
    {
        if (!longopt) return false;

        size_t longOptLen = strlen(longopt);
        if (longOptLen > static_cast<size_t>(INT_MAX) || s.Length() < longOptLen)
            return false;
        optLen = static_cast<int>(longOptLen);
        if (longOptLen > 0)
        {
            for (size_t l = 0; l < longOptLen; ++l)
                if (!isalnum(longopt[l]) && longopt[l] != '-' && longopt[l] != '_')
                    return false;
        }

        if (strncmp(s.CStr(), longopt, longOptLen) != 0)
            return false;

        if (s.Length() != longOptLen && s[(int)longOptLen] != '=')
            return false;

        match = true;
    }

    if (sz > optLen && s[optLen] == '=')
    {
        arg = CmdlineArg(&m_Args[m_Index], CountJointedValues(s, optLen), true);
        ++m_Index;
        return true;
    }

    ++m_Index; // Skip the option itself

    if (maxValueCount != 0)
    {
        const XString *values = NULL;
        int valueCount = 0;

        if (maxValueCount == -1)
            maxValueCount = (int)(m_Args.Size() - m_Index);

        if (m_Index >= (int)m_Args.Size() || maxValueCount == 0)
            arg = CmdlineArg(NULL, 0);
        else
        {
            values = &m_Args[m_Index];
            while (valueCount < maxValueCount && m_Index < (int)m_Args.Size())
            {
                const XString &next = m_Args[m_Index];
                if (IsOptionLike(next))
                    break;
                ++m_Index;
                ++valueCount;
            }
            arg = CmdlineArg(values, valueCount);
        }
    }

    return true;
}

bool CmdlineParser::Skip()
{
    if (m_Index < (int)m_Args.Size())
    {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const
{
    return m_Index >= (int)m_Args.Size();
}

void CmdlineParser::Reset()
{
    m_Index = 0;
}
