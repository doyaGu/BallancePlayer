#include <gtest/gtest.h>

#include "CmdlineParser.h"

static void ExpectXStringEq(const XString &value, const char *expected) {
    EXPECT_STREQ(value.CStr(), expected);
}

TEST(CmdlineArgTest, DefaultConstructorHasNoValues) {
    CmdlineArg arg;

    EXPECT_EQ(arg.GetSize(), 0);

    XString stringValue;
    EXPECT_FALSE(arg.GetValue(0, stringValue));

    long longValue;
    EXPECT_FALSE(arg.GetValue(0, longValue));
}

TEST(CmdlineArgTest, ReadsIndexedValues) {
    XString values[3] = {"value1", "value2", "value3"};
    CmdlineArg arg(values, 3);

    EXPECT_EQ(arg.GetSize(), 3);

    XString value;
    EXPECT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value1");
    EXPECT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "value2");
    EXPECT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "value3");
    EXPECT_FALSE(arg.GetValue(3, value));
}

TEST(CmdlineArgTest, RemovesQuotesFromValues) {
    XString values[3] = {"\"quoted value\"", "regular value", "\"another quoted\""};
    CmdlineArg arg(values, 3);

    XString value;
    EXPECT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "quoted value");
    EXPECT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "regular value");
    EXPECT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "another quoted");
}

TEST(CmdlineArgTest, ReadsJointedValues) {
    XString values[1] = {"opt=value1;value2;\"quoted value\""};
    CmdlineArg arg(values, 3, true);

    EXPECT_EQ(arg.GetSize(), 3);

    XString value;
    EXPECT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value1");
    EXPECT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "value2");
    EXPECT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "quoted value");
    EXPECT_FALSE(arg.GetValue(3, value));
}

TEST(CmdlineArgTest, ReadsLongValues) {
    XString values[3] = {"123", "-456", "not_a_number"};
    CmdlineArg arg(values, 3);

    long value;
    EXPECT_TRUE(arg.GetValue(0, value));
    EXPECT_EQ(value, 123);
    EXPECT_TRUE(arg.GetValue(1, value));
    EXPECT_EQ(value, -456);
    EXPECT_FALSE(arg.GetValue(2, value));
}

TEST(CmdlineArgTest, RejectsNegativeIndexes) {
    XString values[3] = {"value1", "value2", "value3"};
    CmdlineArg arg(values, 3);

    XString stringValue;
    EXPECT_FALSE(arg.GetValue(-1, stringValue));

    long longValue;
    EXPECT_FALSE(arg.GetValue(-1, longValue));
}

TEST(CmdlineParserTest, ArgvConstructorSkipsProgramName) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("arg1"),
        const_cast<char *>("arg2"),
        const_cast<char *>("arg3")
    };

    CmdlineParser parser(4, argv);

    EXPECT_FALSE(parser.Done());
}

TEST(CmdlineParserTest, ReadsShortAndLongOptions) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-a"),
        const_cast<char *>("value1"),
        const_cast<char *>("-b"),
        const_cast<char *>("value2"),
        const_cast<char *>("--long"),
        const_cast<char *>("value3")
    };
    CmdlineParser parser(7, argv);

    CmdlineArg arg;
    XString value;

    ASSERT_TRUE(parser.Next(arg, NULL, 'a', 1));
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value1");

    ASSERT_TRUE(parser.Next(arg, NULL, 'b', 1));
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value2");

    ASSERT_TRUE(parser.Next(arg, "--long", '\0', 1));
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value3");

    EXPECT_TRUE(parser.Done());
}

TEST(CmdlineParserTest, ReadsJointedOptionValues) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("--option=value1;value2;value3")
    };
    CmdlineParser parser(2, argv);

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, "--option", '\0', 0));

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value1");
    ASSERT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "value2");
    ASSERT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "value3");

    EXPECT_TRUE(parser.Done());
}

TEST(CmdlineParserTest, SkipAdvancesArgumentCursor) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-a"),
        const_cast<char *>("value1"),
        const_cast<char *>("-b"),
        const_cast<char *>("value2")
    };
    CmdlineParser parser(5, argv);

    ASSERT_TRUE(parser.Skip());
    ASSERT_TRUE(parser.Skip());

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, NULL, 'b', 1));

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value2");
    EXPECT_TRUE(parser.Done());
}

TEST(CmdlineParserTest, ResetReturnsToFirstArgument) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-a"),
        const_cast<char *>("value1"),
        const_cast<char *>("-b"),
        const_cast<char *>("value2")
    };
    CmdlineParser parser(5, argv);

    ASSERT_TRUE(parser.Skip());
    ASSERT_TRUE(parser.Skip());

    parser.Reset();

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, NULL, 'a', 1));

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "value1");
}

TEST(CmdlineParserTest, HandlesNoArguments) {
    char *argv[] = {const_cast<char *>("program")};
    CmdlineParser parser(1, argv);

    EXPECT_TRUE(parser.Done());

    CmdlineArg arg;
    EXPECT_FALSE(parser.Next(arg, "--option", '\0', 0));
}

TEST(CmdlineParserTest, ReadsFixedValueCount) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-m"),
        const_cast<char *>("val1"),
        const_cast<char *>("val2"),
        const_cast<char *>("val3"),
        const_cast<char *>("-n")
    };
    CmdlineParser parser(6, argv);

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, NULL, 'm', 3));
    EXPECT_EQ(arg.GetSize(), 3);

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "val1");
    ASSERT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "val2");
    ASSERT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "val3");
    EXPECT_FALSE(parser.Done());
}

TEST(CmdlineParserTest, ReadsUnlimitedValuesUntilNextOption) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-m"),
        const_cast<char *>("val1"),
        const_cast<char *>("val2"),
        const_cast<char *>("val3"),
        const_cast<char *>("-n")
    };
    CmdlineParser parser(6, argv);

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, NULL, 'm', -1));
    EXPECT_EQ(arg.GetSize(), 3);

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "val1");
    ASSERT_TRUE(arg.GetValue(1, value));
    ExpectXStringEq(value, "val2");
    ASSERT_TRUE(arg.GetValue(2, value));
    ExpectXStringEq(value, "val3");
    EXPECT_FALSE(parser.Done());
}

TEST(CmdlineParserTest, ReadsEmptyStringValues) {
    char *argv[] = {
        const_cast<char *>("program"),
        const_cast<char *>("-e"),
        const_cast<char *>(""),
        const_cast<char *>("-f"),
        const_cast<char *>("something")
    };
    CmdlineParser parser(5, argv);

    CmdlineArg arg;
    ASSERT_TRUE(parser.Next(arg, NULL, 'e', 1));

    XString value;
    ASSERT_TRUE(arg.GetValue(0, value));
    ExpectXStringEq(value, "");
}
