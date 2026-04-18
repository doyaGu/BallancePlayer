#include "CmdlineParser.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

static std::string RemoveQuotes(const std::string &text)
{
    std::string value;
    bool inQuote = false;

    for (size_t i = 0; i < text.length(); ++i)
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

static bool IsOptionLike(const std::string &text)
{
    if (text.length() < 2 || text[0] != '-')
        return false;

    return !isdigit(static_cast<unsigned char>(text[1]));
}

static int CountJointedValues(const std::string &text, int optLen)
{
    int count = 1;
    int start = optLen + 1;

    for (int i = start; i < (int)text.length(); ++i)
    {
        if (text[i] == ';' && i != start)
        {
            ++count;
            start = i + 1;
        }
    }

    return count;
}

void AppendCommandLineArgs(std::vector<std::string> &args, const char *cmdline)
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

        std::string arg;
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

        args.push_back(arg);
    }
}

bool CmdlineArg::GetValue(int i, std::string &value) const
{
    if (i < 0 || (!m_Jointed && i >= m_Size) || !m_Values)
        return false;

    const std::string &val = m_Jointed ? m_Values[0] : m_Values[i];

    if (!m_Jointed)
    {
        value = RemoveQuotes(val);
    }
    else
    {
        size_t foundPos = val.find('=');
        if (foundPos == std::string::npos)
            return false;

        int n = 0;
        int start = (int)foundPos + 1;
        int end = start;

        for (; end <= (int)val.length(); ++end)
        {
            if (end == (int)val.length() || (val[end] == ';' && end != start))
            {
                if (i == n)
                {
                    value = RemoveQuotes(val.substr(start, end - start));
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

    std::string v;
    if (!GetValue(i, v))
        return false;

    const char *s = v.c_str();
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
        m_Args.push_back(argv[i]);
}

CmdlineParser::CmdlineParser(const char *cmdline) : m_Index(0)
{
    AppendCommandLineArgs(m_Args, cmdline);
}

#ifdef WIN32
CmdlineParser::CmdlineParser(const wchar_t *cmdline) : m_Index(0)
{
    if (!cmdline)
        return;

    int size = WideCharToMultiByte(CP_ACP, 0, cmdline, -1, NULL, 0, NULL, NULL);
    if (size <= 0)
        return;

    std::string buffer;
    buffer.resize(size);
    WideCharToMultiByte(CP_ACP, 0, cmdline, -1, &buffer[0], size, NULL, NULL);
    AppendCommandLineArgs(m_Args, buffer.c_str());
}
#endif

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount)
{
    arg = CmdlineArg();

    if (Done())
        return false;

    const std::string &s = m_Args[m_Index];
    const int sz = (int)s.length();
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

        optLen = (int)strlen(longopt);
        if (optLen > 0)
        {
            for (int l = 0; l < optLen; ++l)
                if (!isalnum(longopt[l]) && longopt[l] != '-' && longopt[l] != '_')
                    return false;
        }

        if (strncmp(s.c_str(), longopt, optLen) != 0)
            return false;

        if (sz != optLen && s[optLen] != '=')
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
        const std::string *values = NULL;
        int valueCount = 0;

        if (maxValueCount == -1)
            maxValueCount = (int)(m_Args.size() - m_Index);

        if (m_Index >= (int)m_Args.size() || maxValueCount == 0)
            arg = CmdlineArg(NULL, 0);
        else
        {
            values = &m_Args[m_Index];
            while (valueCount < maxValueCount && m_Index < (int)m_Args.size())
            {
                const std::string &next = m_Args[m_Index];
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
    if (m_Index < (int)m_Args.size())
    {
        ++m_Index;
        return true;
    }
    return false;
}

bool CmdlineParser::Done() const
{
    return m_Index >= (int)m_Args.size();
}

void CmdlineParser::Reset()
{
    m_Index = 0;
}
