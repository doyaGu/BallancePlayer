#ifndef PLAYER_CMDLINEPARSER_H
#define PLAYER_CMDLINEPARSER_H

#ifdef WIN32
#pragma warning (disable: 4514 4786)
#endif

#include <string>
#include <vector>

/**
 * @brief Represents a command line argument with its values
 */
class CmdlineArg {
public:
    /**
     * @brief Default constructor
     */
    CmdlineArg() : m_Values(nullptr), m_Size(0), m_Jointed(false) {}

    /**
     * @brief Constructor for creating an argument with values
     * @param values Array of strings representing values
     * @param size Number of values in the array
     * @param jointed Whether the values are in a jointed format (--option=value)
     */
    CmdlineArg(const std::string *values, int size, bool jointed = false)
        : m_Values(values), m_Size(size), m_Jointed(jointed) {}

    /**
     * @brief Get a string value at the specified index
     * @param i Index of the value to retrieve
     * @param value Reference to store the retrieved value
     * @return True if successful, false otherwise
     */
    bool GetValue(int i, std::string &value) const;

    /**
     * @brief Get a long integer value at the specified index
     * @param i Index of the value to retrieve
     * @param value Reference to store the retrieved value
     * @return True if successful, false otherwise
     */
    bool GetValue(int i, long &value) const;

    /**
     * @brief Get the number of values in this argument
     * @return Number of values
     */
    int GetSize() const;

private:
    const std::string *m_Values; ///< Array of values
    int m_Size; ///< Number of values
    bool m_Jointed; ///< Whether values are in a jointed format
};

/**
 * @brief Parser for command line arguments
 */
class CmdlineParser {
public:
    /**
     * @brief Constructor
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     */
    CmdlineParser(int argc, char **argv);

    /**
     * @brief Parse the next argument
     * @param arg Reference to store the parsed argument
     * @param longopt Long option name (without leading dashes)
     * @param opt Short option character
     * @param maxValueCount Maximum number of values to expect (0 for flag, -1 for unlimited)
     * @return True if an argument was found, false otherwise
     */
    bool Next(CmdlineArg &arg, const char *longopt, char opt = '\0', int maxValueCount = 0);

    /**
     * @brief Skip the current argument
     * @return True if an argument was skipped, false if at the end
     */
    bool Skip();

    /**
     * @brief Check if all arguments have been processed
     * @return True if all arguments have been processed, false otherwise
     */
    bool Done() const;

    /**
     * @brief Reset the parser to the beginning
     */
    void Reset();

private:
    std::vector<std::string> m_Args; ///< Array of command line arguments
    int m_Index; ///< Current position in the argument array
};

#endif // PLAYER_CMDLINEPARSER_H
