#include "IniFile.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool IsAsciiSpace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\v' || ch == '\f';
}

static XString SubstringLength(const XString &str, XWORD start, XWORD length)
{
    return length == 0 ? XString() : str.Substring(start, length);
}

IniFile::IniFile()
    : m_SectionIndexDirty(true)
{
}

IniFile::Entry::Entry()
    : isComment(false), isEmpty(false)
{
}

IniFile::Section::Section()
    : keyIndexDirty(true)
{
}

IniFile::Section::Section(const XString &sectionName)
    : name(sectionName), keyIndexDirty(true)
{
    headerLine << "[" << sectionName << "]";
}

void IniFile::Section::MarkKeyIndexDirty() const
{
    keyIndexDirty = true;
}

void IniFile::Section::RebuildKeyIndex() const
{
    if (!keyIndexDirty)
        return;

    keyIndex.Clear();
    keyIndex.Reserve(entries.Size());
    for (int i = 0; i < entries.Size(); ++i)
    {
        const Entry &entry = entries[i];
        if (!entry.isComment && !entry.isEmpty && !entry.key.IsEmpty())
            keyIndex.Insert(NormalizeName(entry.key), i);
    }
    keyIndexDirty = false;
}

IniFile::Entry *IniFile::Section::FindKey(const XString &normalizedKey)
{
    RebuildKeyIndex();
    int index = -1;
    return keyIndex.LookUp(normalizedKey, index) && index >= 0 && index < entries.Size() ? &entries[index] : NULL;
}

const IniFile::Entry *IniFile::Section::FindKey(const XString &normalizedKey) const
{
    RebuildKeyIndex();
    int index = -1;
    return keyIndex.LookUp(normalizedKey, index) && index >= 0 && index < entries.Size() ? &entries[index] : NULL;
}

void IniFile::Clear()
{
    m_LeadingLines.Clear();
    m_Sections.Clear();
    m_SectionIndex.Clear();
    m_SectionIndexDirty = true;
}

XString IniFile::TrimAscii(const XString &str)
{
    int first = 0;
    while (first < (int)str.Length() && IsAsciiSpace(str[first]))
        ++first;

    int last = (int)str.Length();
    while (last > first && IsAsciiSpace(str[last - 1]))
        --last;

    return SubstringLength(str, (XWORD)first, (XWORD)(last - first));
}

XString IniFile::NormalizeName(const XString &str)
{
    XString result = TrimAscii(str);
    for (int i = 0; i < (int)result.Length(); ++i)
    {
        unsigned char ch = (unsigned char)result[i];
        if (ch < 0x80)
            result[i] = (char)tolower(ch);
    }
    return result;
}

bool IniFile::IsCommentLine(const XString &line)
{
    XString trimmed = TrimAscii(line);
    return !trimmed.IsEmpty() && (trimmed[0] == ';' || trimmed[0] == '#');
}

bool IniFile::IsEmptyLine(const XString &line)
{
    return TrimAscii(line).IsEmpty();
}

bool IniFile::IsSectionHeader(const XString &line, XString &sectionName)
{
    XString trimmed = TrimAscii(line);
    if (trimmed.Length() < 2 || trimmed[0] != '[')
        return false;

    XWORD end = trimmed.Find(']', 1);
    if (end == XString::NOTFOUND)
        return false;

    sectionName = TrimAscii(SubstringLength(trimmed, 1, end - 1));
    return true;
}

bool IniFile::ParseKeyValue(const XString &line, XString &key, XString &value, XString &comment)
{
    XWORD eq = line.Find('=');
    if (eq == XString::NOTFOUND)
        return false;

    key = TrimAscii(SubstringLength(line, 0, eq));
    if (key.IsEmpty())
        return false;

    XString valueAndComment = line.Substring(eq + 1);
    bool inQuotes = false;
    XWORD commentPos = XString::NOTFOUND;
    for (int i = 0; i < (int)valueAndComment.Length(); ++i)
    {
        char ch = valueAndComment[i];
        if (ch == '"' && (i == 0 || valueAndComment[i - 1] != '\\'))
        {
            inQuotes = !inQuotes;
            continue;
        }
        if (!inQuotes && (ch == ';' || ch == '#') &&
            (i == 0 || IsAsciiSpace(valueAndComment[i - 1])))
        {
            commentPos = (XWORD)i;
            break;
        }
    }

    if (commentPos == XString::NOTFOUND)
    {
        value = TrimAscii(valueAndComment);
        comment.Clear();
    }
    else
    {
        value = TrimAscii(SubstringLength(valueAndComment, 0, commentPos));
        comment = TrimAscii(valueAndComment.Substring(commentPos));
    }
    return true;
}

XString IniFile::FormatKeyValue(const XString &key, const XString &value, const XString &comment)
{
    XString result;
    result << key << " = " << value;
    if (!comment.IsEmpty())
        result << " " << comment;
    return result;
}

bool IniFile::ParseFromString(const XString &content)
{
    Clear();

    XString normalized;
    normalized.Reserve(content.Length());
    for (int i = 0; i < (int)content.Length(); ++i)
    {
        char ch = content[i];
        if (ch == '\r')
        {
            if (i + 1 < (int)content.Length() && content[i + 1] == '\n')
                continue;
            normalized << '\n';
        }
        else
        {
            normalized << ch;
        }
    }

    if (normalized.Length() >= 3 &&
        (unsigned char)normalized[0] == 0xEF &&
        (unsigned char)normalized[1] == 0xBB &&
        (unsigned char)normalized[2] == 0xBF)
    {
        normalized.Cut(0, 3);
    }

    Section *currentSection = NULL;
    XWORD lineStart = 0;
    while (lineStart < normalized.Length())
    {
        XWORD lineEnd = normalized.Find('\n', lineStart);
        XString line = lineEnd == XString::NOTFOUND
            ? normalized.Substring(lineStart)
            : SubstringLength(normalized, lineStart, lineEnd - lineStart);

        XString sectionName;
        if (IsSectionHeader(line, sectionName))
        {
            m_Sections.Expand();
            currentSection = &m_Sections.Back();
            currentSection->name = sectionName;
            currentSection->headerLine = TrimAscii(line);
            m_SectionIndexDirty = true;
        }
        else if (!currentSection)
        {
            m_LeadingLines.PushBack(line);
        }
        else
        {
            Entry entry;
            entry.originalLine = line;
            entry.isEmpty = IsEmptyLine(line);
            entry.isComment = !entry.isEmpty && IsCommentLine(line);
            if (!entry.isComment && !entry.isEmpty)
            {
                ParseKeyValue(line, entry.key, entry.value, entry.inlineComment);
                if (!entry.key.IsEmpty())
                    currentSection->MarkKeyIndexDirty();
            }
            currentSection->entries.PushBack(entry);
        }

        if (lineEnd == XString::NOTFOUND)
            break;
        lineStart = lineEnd + 1;
    }

    RebuildSectionIndex();
    return true;
}

bool IniFile::ParseFromFile(const char *filename)
{
    if (!filename || filename[0] == '\0')
        return false;

    FILE *file = fopen(filename, "rb");
    if (!file)
        return false;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return false;
    }

    long length = ftell(file);
    if (length < 0 || length > XString::MAX_LENGTH || fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return false;
    }

    char *buffer = new char[(size_t)length + 1];
    size_t readCount = length > 0 ? fread(buffer, 1, (size_t)length, file) : 0;
    fclose(file);

    if (readCount != (size_t)length)
    {
        delete[] buffer;
        return false;
    }

    buffer[length] = '\0';
    XString content(buffer, (int)length);
    delete[] buffer;
    return ParseFromString(content);
}

XString IniFile::WriteToString() const
{
    XString result;
    for (int i = 0; i < m_LeadingLines.Size(); ++i)
    {
        result << m_LeadingLines[i];
        result << "\n";
    }

    for (int sectionIndex = 0; sectionIndex < m_Sections.Size(); ++sectionIndex)
    {
        const Section &section = m_Sections[sectionIndex];
        if (!section.name.IsEmpty())
        {
            if (!result.IsEmpty() && result[(int)result.Length() - 1] != '\n')
                result << "\n";
            if (section.headerLine.IsEmpty())
                result << "[" << section.name << "]";
            else
                result << section.headerLine;
            result << "\n";
        }

        for (int entryIndex = 0; entryIndex < section.entries.Size(); ++entryIndex)
        {
            result << section.entries[entryIndex].originalLine;
            result << "\n";
        }
    }
    return result;
}

bool IniFile::WriteToFile(const char *filename) const
{
    if (!filename || filename[0] == '\0')
        return false;

    FILE *file = fopen(filename, "wb");
    if (!file)
        return false;

    XString content = WriteToString();
    size_t length = content.Length();
    bool ok = length == 0 || fwrite(content.CStr(), 1, length, file) == length;
    ok = fclose(file) == 0 && ok;
    return ok;
}

XString IniFile::GetValue(const XString &sectionName, const XString &key, const XString &defaultValue) const
{
    const Section *section = FindSection(sectionName);
    if (!section)
        return defaultValue;

    const Entry *entry = section->FindKey(NormalizeName(key));
    return entry ? entry->value : defaultValue;
}

bool IniFile::SetValue(const XString &sectionName, const XString &key, const XString &value)
{
    XString trimmedKey = TrimAscii(key);
    if (trimmedKey.IsEmpty())
        return false;

    Section *section = FindSection(sectionName);
    if (!section)
        section = AddSection(sectionName);
    if (!section)
        return false;

    Entry *entry = section->FindKey(NormalizeName(trimmedKey));
    if (entry)
    {
        entry->key = trimmedKey;
        entry->value = value;
        entry->originalLine = FormatKeyValue(trimmedKey, value, entry->inlineComment);
        section->MarkKeyIndexDirty();
        return true;
    }

    Entry newEntry;
    newEntry.key = trimmedKey;
    newEntry.value = value;
    newEntry.originalLine = FormatKeyValue(trimmedKey, value, XString());
    section->entries.PushBack(newEntry);
    section->MarkKeyIndexDirty();
    return true;
}

void IniFile::RebuildSectionIndex() const
{
    if (!m_SectionIndexDirty)
        return;

    m_SectionIndex.Clear();
    m_SectionIndex.Reserve(m_Sections.Size());
    for (int i = 0; i < m_Sections.Size(); ++i)
    {
        XString normalizedName = NormalizeName(m_Sections[i].name);
        int existingIndex = -1;
        if (!m_SectionIndex.LookUp(normalizedName, existingIndex))
            m_SectionIndex.Insert(normalizedName, i);
    }
    m_SectionIndexDirty = false;
}

IniFile::Section *IniFile::FindSection(const XString &sectionName)
{
    RebuildSectionIndex();
    int index = -1;
    return m_SectionIndex.LookUp(NormalizeName(sectionName), index) && index >= 0 && index < m_Sections.Size() ? &m_Sections[index] : NULL;
}

const IniFile::Section *IniFile::FindSection(const XString &sectionName) const
{
    RebuildSectionIndex();
    int index = -1;
    return m_SectionIndex.LookUp(NormalizeName(sectionName), index) && index >= 0 && index < m_Sections.Size() ? &m_Sections[index] : NULL;
}

IniFile::Section *IniFile::AddSection(const XString &sectionName)
{
    XString trimmedName = TrimAscii(sectionName);
    m_Sections.Expand();
    m_SectionIndexDirty = true;
    Section &section = m_Sections.Back();
    section.name = trimmedName;
    section.headerLine.Clear();
    section.headerLine << "[" << trimmedName << "]";
    section.entries.Clear();
    section.keyIndex.Clear();
    section.keyIndexDirty = true;
    return &m_Sections.Back();
}
