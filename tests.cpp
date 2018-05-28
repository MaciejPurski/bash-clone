//
// Created by papryczka on 28.05.18.
//]
#define BOOST_TEST_MODULE First_TestSuite
#include <boost/test/included/unit_test.hpp>
#include <exception>
#include "Environment.h"

BOOST_AUTO_TEST_CASE( SetVatiableTest )
{
	Environment env;

	std::string name = "NEW";
	std::string value = "newValue";

	env.setVariable(name, value);

	BOOST_CHECK_EQUAL(env.getValue(name), value);
}

BOOST_AUTO_TEST_CASE(ChangeVariableName)
{
	Environment env;

	std::string name = "NEW";
	std::string value = "oldValue";
	std::string newValue = "newValue";
	env.setVariable(name, value);
	env.setVariable(name, newValue);

	BOOST_CHECK_EQUAL(env.getValue(name), newValue);
}

BOOST_AUTO_TEST_CASE(UserVariable)
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	char * userName = pw->pw_name;

	Environment env;
	std::string tmp = env.getValue("USER");

	BOOST_CHECK_EQUAL(tmp.c_str(), userName );
}

BOOST_AUTO_TEST_CASE(HomeVariable)
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	char * homeDir =  pw->pw_dir;

	Environment env;
	std::string tmp = env.getValue("HOME");

	BOOST_CHECK_EQUAL(tmp.c_str(), homeDir);
}

BOOST_AUTO_TEST_CASE(VimCheck)
{
	Environment env;
	std::string vimPath = env.getValue("HOME") + "/bin/vim";

	BOOST_CHECK_EQUAL(vimPath, env.searchPath("vim"));
}

BOOST_AUTO_TEST_CASE(LastElementNull)
{
	Environment env;
	char ** tmp = env.getEnvironment();


	BOOST_CHECK_EQUAL(tmp[3][0], '\0');
}