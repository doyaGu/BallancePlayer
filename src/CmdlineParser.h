#ifndef PLAYER_CMDLINEPARSER_H
#define PLAYER_CMDLINEPARSER_H

#ifdef WIN32
#pragma warning (disable: 4514 4786)
#endif

#include "XString.h"
#include "XClassArray.h"

class CmdlineArg {
public:
    CmdlineArg() : m_Values(NULL), m_Size(0), m_Jointed(false) {}

    CmdlineArg(const XString *values, int size, bool jointed = false)
        : m_Values(values), m_Size(size), m_Jointed(jointed) {}

    bool GetValue(int i, XString &value) const;
    bool GetValue(int i, long &value) const;

    int GetSize() const;

private:
    const XString *m_Values;
    int m_Size;
    bool m_Jointed;
};

class CmdlineParser {
public:
    CmdlineParser(int argc, char **argv);

    bool Next(CmdlineArg &arg, const char *longopt, char opt = '\0', int maxValueCount = 0);
    bool Skip();
    bool Done() const;
    void Reset();

private:
    XClassArray<XString> m_Args;
    int m_Index;
};

#endif // PLAYER_CMDLINEPARSER_H
