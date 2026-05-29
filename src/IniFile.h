#ifndef PLAYER_INIFILE_H
#define PLAYER_INIFILE_H

#include <stddef.h>

#include "XClassArray.h"
#include "XHashFun.h"
#include "XHashTable.h"
#include "XString.h"

class IniFile {
public:
    IniFile();

    struct Entry {
        XString key;
        XString value;
        XString inlineComment;
        XString originalLine;
        bool isComment;
        bool isEmpty;

        Entry();
    };

    struct Section {
        XString name;
        XString headerLine;
        XClassArray<Entry> entries;
        mutable XHashTable<int, XString, XHashFunXStringI, XEqualXStringI> keyIndex;
        mutable bool keyIndexDirty;

        Section();
        explicit Section(const XString &sectionName);

        void MarkKeyIndexDirty() const;
        void RebuildKeyIndex() const;
        Entry *FindKey(const XString &normalizedKey);
        const Entry *FindKey(const XString &normalizedKey) const;
    };

    bool ParseFromString(const XString &content);
    bool ParseFromFile(const char *filename);
    XString WriteToString() const;
    bool WriteToFile(const char *filename) const;

    XString GetValue(const XString &sectionName, const XString &key, const XString &defaultValue = XString()) const;
    bool SetValue(const XString &sectionName, const XString &key, const XString &value);

    void Clear();

private:
    XClassArray<XString> m_LeadingLines;
    XClassArray<Section> m_Sections;
    mutable XHashTable<int, XString, XHashFunXStringI, XEqualXStringI> m_SectionIndex;
    mutable bool m_SectionIndexDirty;

    static XString TrimAscii(const XString &str);
    static XString NormalizeName(const XString &str);
    static bool IsCommentLine(const XString &line);
    static bool IsEmptyLine(const XString &line);
    static bool IsSectionHeader(const XString &line, XString &sectionName);
    static bool ParseKeyValue(const XString &line, XString &key, XString &value, XString &comment);
    static XString FormatKeyValue(const XString &key, const XString &value, const XString &comment);

    void RebuildSectionIndex() const;
    Section *FindSection(const XString &sectionName);
    const Section *FindSection(const XString &sectionName) const;
    Section *AddSection(const XString &sectionName);

    IniFile(const IniFile &);
    IniFile &operator=(const IniFile &);
};

#endif // PLAYER_INIFILE_H
