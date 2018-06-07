#define BOOST_TEST_MODULE ENGINE_TEST
#include <boost/test/included/unit_test.hpp>
#include <exception>
#include "../ExecutionEngine.h"

#include <unistd.h>

struct Engine
{
	static Environment env;
	static ExecutionEngine engine;
	static std::string currentDir;

	Engine()
	{
		char dirName[1024];

		getcwd(dirName, sizeof(dirName));
		printf("%s\n", dirName);
		currentDir = std::string(dirName);
	}

	~Engine()
	{
	}

};

Environment Engine::env;
ExecutionEngine Engine::engine(env);
std::string Engine::currentDir;

BOOST_TEST_GLOBAL_FIXTURE( Engine );

BOOST_AUTO_TEST_SUITE( ENGINE_TEST_SUITE)
	BOOST_AUTO_TEST_CASE(change_dir_to_current)
	{

		Command cmd;
		cmd.command = "cd";
		cmd.args.push_back(Engine::currentDir);

		std::vector<Command> commandLine({cmd});
		Engine::engine.executeCommandLine(commandLine);

		BOOST_CHECK_EQUAL(Engine::env.getCurrentDir(), Engine::currentDir + '/');
	}

	BOOST_AUTO_TEST_CASE(change_dir_to_tests)
	{
		std::string dirString;

		dirString = Engine::currentDir.substr(0, Engine::currentDir.size() - 17);
		dirString += "tests/";

		Command cmd;
		cmd.command = "cd";

		cmd.args.push_back(dirString);

		std::vector<Command> commandLine({cmd});
		Engine::engine.executeCommandLine(commandLine);

		BOOST_CHECK_EQUAL(Engine::env.getCurrentDir(), dirString);
	}

	BOOST_AUTO_TEST_CASE(change_dir_to_non_existing)
	{
		Command cmd;
		cmd.command = "cd";
		cmd.args.push_back(Engine::currentDir + "/non-existing-dir");
		std::vector<Command> commandLine({cmd});

		BOOST_CHECK_THROW( Engine::engine.executeCommandLine(commandLine), std::exception );
	}

	BOOST_AUTO_TEST_CASE(change_dir_to_non_existing2)
	{
		Command cmd;
		cmd.command = "cd";
		cmd.args.push_back("non-existing-dir");
		std::vector<Command> commandLine({cmd});

		BOOST_CHECK_THROW( Engine::engine.executeCommandLine(commandLine), std::exception );
	}

	BOOST_AUTO_TEST_CASE(execute_non_existing_command)
	{
		Command cmd;
		cmd.command = "cd";
		cmd.args.push_back("./non-existing-command");
		std::vector<Command> commandLine({cmd});
		

		BOOST_CHECK_THROW( Engine::engine.executeCommandLine(commandLine), std::exception );
	}

	BOOST_AUTO_TEST_CASE(return_code_test)
	{
		Command cmd;
		cmd.command = "./return-test";
		cmd.args.push_back("4");
		std::vector<Command> commandLine({cmd});

		Engine::engine.executeCommandLine(commandLine);
		BOOST_CHECK_EQUAL( Engine::env.getReturnCode(), 4 );
	}

	BOOST_AUTO_TEST_CASE(return_code_pipe_test)
	{
		std::vector<Command> commandLine;
		for (int i = 0; i < 5; i++) {
			Command cmd;
			cmd.command = "./return-test";
			cmd.args.push_back(std::to_string(i));
			commandLine.push_back(cmd);
		}

		Engine::engine.executeCommandLine(commandLine);
		BOOST_CHECK_EQUAL( Engine::env.getReturnCode(), 4 );
	}

	BOOST_AUTO_TEST_CASE(env_test)
	{
		std::vector<Command> commandLine;
		Command cmd;

		cmd.command = "./env-test";
		cmd.redirections.emplace_back(Command::Redirection(1, false, false, "test-env.txt"));
		commandLine.push_back(cmd);

		char **environ = Engine::env.getEnvironment();

		Engine::engine.executeCommandLine(commandLine);

		BOOST_CHECK_EQUAL( Engine::env.getReturnCode(), 0 );

		std::ifstream in;
		in.open("../tests/test-env.txt");

		BOOST_CHECK_EQUAL( in.is_open(), true );

		for (int i = 0; i < Engine::env.countGlobalVariables(); i++) {
			std::string line;

			std::getline(in, line);
			BOOST_CHECK_EQUAL(strcmp(line.c_str(), environ[i]), 0);
		}

		in.close();
	}


	BOOST_AUTO_TEST_CASE(env_test_add) {
		std::vector<Command> commandLine;
		Command exp;

		exp.command = "export";
		exp.args.push_back("a=abcd");
		exp.term = Command::TERM;
		commandLine.push_back(exp);

		Command cmd;
		cmd.command = "./env-test";
		cmd.redirections.emplace_back(Command::Redirection(1, false, false, "test-env.txt"));
		commandLine.push_back(cmd);


		Engine::engine.executeCommandLine(commandLine);

		BOOST_CHECK_EQUAL(Engine::env.getReturnCode(), 0);
	}

	BOOST_AUTO_TEST_CASE(export_path_test)
	{
		std::vector<Command> commandLine;
		Command exp;

		exp.command = "export";
		exp.args.push_back("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games");
		exp.term = Command::TERM;
		commandLine.push_back(exp);

		Command cmd;
		cmd.command = "./env-test";
		cmd.redirections.emplace_back(Command::Redirection(1, false, false, "test-env.txt"));
		commandLine.push_back(cmd);

		Engine::engine.executeCommandLine(commandLine);

		std::ifstream in;
		in.open("../tests/test-env.txt");

		BOOST_CHECK_EQUAL( in.is_open(), true );


		char **environ = Engine::env.getEnvironment();
		for (int i = 0; i < Engine::env.countGlobalVariables(); i++) {
			std::string line;

			std::getline(in, line);
			BOOST_CHECK_EQUAL(strcmp(line.c_str(), environ[i]), 0);
		}

		in.close();
	}

	BOOST_AUTO_TEST_CASE(test_ls_command)
	{
		Command cmd;
		std::vector<Command> commandLine;
		cmd.command = "ls";
		cmd.redirections.push_back(Command::Redirection(1, false, false, "/dev/null"));
		commandLine.push_back(cmd);

		Engine::engine.executeCommandLine(commandLine);
		BOOST_CHECK_EQUAL(Engine::env.getReturnCode(), 0);
	}



	BOOST_AUTO_TEST_CASE(unknown_command_test)
	{
		std::vector<Command> commandLine;
		Command exp;

		Command cmd;
		cmd.command = "lssssss";
		commandLine.push_back(cmd);

		BOOST_CHECK_THROW(Engine::engine.executeCommandLine(commandLine), std::exception);
	}


	BOOST_AUTO_TEST_CASE(input_redirection_test)
	{
		std::vector<Command> commandLine;

		Command ech;
		ech.command = "echo";
		ech.args.push_back("abc");
		ech.redirections.push_back(Command::Redirection(1, false, false, "abc.txt"));
		ech.term = Command::TERM;
		commandLine.push_back(ech);

		Command cat;
		cat.command = "cat";
		cat.redirections.push_back(Command::Redirection(0, true, false, "abc.txt"));
		cat.redirections.push_back(Command::Redirection(1, false, false, "abc2.txt"));
		cat.term = Command::AMPER;
		commandLine.push_back(cat);
		Engine::engine.executeCommandLine(commandLine);

		while (Engine::engine.getNJobs() != 0) {
			Engine::engine.update();
			Engine::engine.jobsCleanup();
		}
		std::ifstream in;
		in.open("../tests/abc2.txt");

		BOOST_CHECK_EQUAL( in.is_open(), true );

		std::string line;

		std::getline(in, line);
		in.close();
		BOOST_CHECK_EQUAL(line, "abc");
	}


	BOOST_AUTO_TEST_CASE(input_redirection_background_test)
	{
		std::vector<Command> commandLine;

		Command ech;
		ech.command = "echo";
		ech.args.push_back("abc");
		ech.redirections.push_back(Command::Redirection(1, false, false, "abc.txt"));
		ech.term = Command::TERM;
		commandLine.push_back(ech);

		Command cat;
		cat.command = "cat";
		cat.redirections.push_back(Command::Redirection(0, true, false, "abc.txt"));
		cat.redirections.push_back(Command::Redirection(1, false, false, "abc2.txt"));
		commandLine.push_back(cat);

		Engine::engine.executeCommandLine(commandLine);

		std::ifstream in;
		in.open("../tests/abc2.txt");

		BOOST_CHECK_EQUAL( in.is_open(), true );

		std::string line;

		std::getline(in, line);
		in.close();
		BOOST_CHECK_EQUAL(line, "abc");
	}


BOOST_AUTO_TEST_SUITE_END()
