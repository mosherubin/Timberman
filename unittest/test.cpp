#include "gtest/gtest.h"
#include "FunctionTrace/FunctionTrace.h"
#include "Logger/Logger.h"
#include <cstdlib>

using namespace mvs;

static bool StringMatchesPattern(std::string s, const char *p, int offset)
{
	return (s.compare(offset, strlen(p), p) == 0);
}

static bool StringMatchesPattern(std::string s, const char *p, const char *offsetString)
{
	int offset = s.find(offsetString, 0);

	if (offset == std::string::npos)
		return false;

	return (s.compare(offset, strlen(p), p) == 0);
}

static bool ReturnTemporaryFilePath(const char *filename, std::string &path)
{
	char *p = 0;

#if defined (_WIN32)
	p = std::getenv("TMP");
	if (p == 0)
		p = std::getenv("TEMP");
#else
	p = (char *) "/tmp";
#endif  /* _WIN32 */

	if (p)
	{
		// <p> points to a temporary folder, append <filename> to it and return
		path.assign(p);
		path.append(PATH_SEPARATOR_STRING_UTF8);
		path.append(filename);
		return true;
	}

	return false;
}

static bool ReturnLinesFromFile(const char *path, std::vector<std::string> &vecLines)
{
	vecLines.clear();

	FILE *f;

	if ((f = fopen(path, "rt")) == 0)
	{
		return false;
	}

	char buffer[1024 * 10];

	while (fgets(buffer, sizeof(buffer), f) != 0)
	{
		vecLines.push_back(std::string(buffer));
	}

	fclose(f);

	return true;
}

class TestFixture : public ::testing::Test
{
public:
	void SetUp()
	{
	}

	void TearDown()
	{
	}

	bool SetUpOutputFile(const char *filename)
	{
		if (!ReturnTemporaryFilePath(filename, path))
			return false;
		AnyStringToUtf8(utf8PathIn, path.c_str());
		Logger::SetOutputFile(utf8PathIn.c_str());
		Logger::GetOutputFile(utf8PathOut);
		if (utf8PathOut.compare(utf8PathIn) != 0)
			return false;
		return true;
	}

	std::string path;
	std::string utf8PathIn;
	std::string utf8PathOut;
};

TEST_F(TestFixture, Set_and_Check_Options)
{
	Logger::AddTraceMask("all");
	Logger::AddTraceMask("-foo");
	Logger::AddTraceMask("-bar");
	Logger::AddTraceMask("baz snow");

	std::string traceMaskString;

	Logger::GetTraceMaskString(traceMaskString);

	ASSERT_TRUE(traceMaskString.compare("all baz snow -foo -bar") == 0);
}

TEST_F(TestFixture, Set_and_Check_TracingEnabledDisabled)
{
	Logger::SetTracingEnabled(true);
	ASSERT_TRUE(Logger::IsTracingEnabled());

	Logger::SetTracingEnabled(false);
	ASSERT_FALSE(Logger::IsTracingEnabled());

	Logger::SetTracingEnabled(true);
	ASSERT_TRUE(Logger::IsTracingEnabled());
}

TEST_F(TestFixture, Set_and_Check_OutputFile)
{
	Logger::SetTracingEnabled(true);

	ASSERT_TRUE(SetUpOutputFile("set_and_check_outputfile.txt"));
	Logger::Cleanup();      /* Terminate Logger activity */

	std::vector<std::string> vecLines;
	ASSERT_TRUE(ReturnLinesFromFile(utf8PathOut.c_str(), vecLines));

	ASSERT_TRUE(vecLines.size() == 3);
	ASSERT_TRUE(vecLines[0] == "Starting logging ...\n");
	ASSERT_TRUE(vecLines[1] == "--------------------\n");
	ASSERT_TRUE(vecLines[2] == "\n");
}

void TestFunction_3()
{
	FUNCTIONTRACE("TestFunction_3");
}

void TestFunction_2()
{
	FUNCTIONTRACE("TestFunction_2");

	TestFunction_3();
}

void TestFunction_1()
{
	FUNCTIONTRACE("TestFunction_1");

	TestFunction_2();
}

TEST_F(TestFixture, Test_Simple_FunctionTrace)
{
	Logger::SetTracingEnabled(true);
	Logger::AddTraceMask("functiontrace");

	SetUpOutputFile("Test_Simple_FunctionTrace.txt");

	TestFunction_1();
	Logger::Cleanup();      /* Terminate Logger activity */

	std::vector<std::string> vecLines;

	ASSERT_TRUE(ReturnLinesFromFile(utf8PathOut.c_str(), vecLines));

	ASSERT_TRUE(vecLines.size() == 9);
	ASSERT_TRUE(StringMatchesPattern(vecLines[3], "[functiontrace] > TestFunction_1 (test.cpp:", "[functiontrace]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[4], "[functiontrace]  > TestFunction_2 (test.cpp:", "[functiontrace]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[5], "[functiontrace]   > TestFunction_3 (test.cpp:", "[functiontrace]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[6], "[functiontrace]   < TestFunction_3", "[functiontrace]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[7], "[functiontrace]  < TestFunction_2", "[functiontrace]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[8], "[functiontrace] < TestFunction_1", "[functiontrace]"));
}

TEST_F(TestFixture, Test_LoggerAssert)
{
	Logger::SetTracingEnabled(true);
	Logger::AddTraceMask("functiontrace");
	Logger::AddTraceMask("assert");

	SetUpOutputFile("Test_LoggerAssert.txt");
	LOGGER_ASSERT(false);
	Logger::Cleanup();      /* Terminate Logger activity */

	std::vector<std::string> vecLines;

	ASSERT_TRUE(ReturnLinesFromFile(utf8PathOut.c_str(), vecLines));

	ASSERT_TRUE(vecLines.size() == 4);
	ASSERT_TRUE(StringMatchesPattern(vecLines[3], "[assert] LOGGER_ASSERT(false) failed", "[assert]"));
}

TEST_F(TestFixture, Test_HexDumpTrace)
{
	Logger::SetTracingEnabled(true);
	Logger::AddTraceMask("packets");

	SetUpOutputFile("Test_HexDumpTrace.txt");
	const char *p = "1234567890qwertyuiop";
	HEXDUMPTRACE(("packets", p, strlen(p), "Test_HexDumpTrace\n"));
	Logger::Cleanup();      /* Terminate Logger activity */

	std::vector<std::string> vecLines;

	ASSERT_TRUE(ReturnLinesFromFile(utf8PathOut.c_str(), vecLines));

	ASSERT_TRUE(vecLines.size() == 7);
	ASSERT_TRUE(StringMatchesPattern(vecLines[3], "[packets] Test_HexDumpTrace", "[packets]"));
	ASSERT_TRUE(StringMatchesPattern(vecLines[4], "000000: 31 32 33 34 35 36 37 38 39 30 71 77 65 72 74 79  1234567890qwerty", 0));
	ASSERT_TRUE(StringMatchesPattern(vecLines[5], "000010: 75 69 6f 70                                      uiop", 0));
}
