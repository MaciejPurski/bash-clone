#ifndef BASH_CLONE_ENVIRONMENT_H
#define BASH_CLONE_ENVIRONMENT_H
#include "Command.h"

#include <iostream>
#include <unordered_map>
#include <fstream>

#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <pwd.h>

#include <vector>
#include <memory>

/**
 *	@class Environment
 *
 *	@brief Used by other classes to access environmental data,
 *	especially environmental variables.
 */
class Environment {

	enum TypeOfVariable{
		localVariable,
		globalVariable
	};

	struct EnvironmentVariable
	{
		std::string name_;
		std::string value_;

		EnvironmentVariable(const std::string name, const std::string value="")
				:name_(name), value_(value)
		{};

		std::string getName() const
		{
			return name_;
		}

		std::string getValue() const
		{
			return value_;
		}

		void setValue(const std::string newValue) {
			value_=newValue;
		}
	};

	std::string currentDir_;
	std::vector<std::unique_ptr<EnvironmentVariable>> localVariables_;
	std::vector<std::unique_ptr<EnvironmentVariable>> globalVariables_;

	private:
	void loadUserVariable();
	void loadHomeVariable();
	void loadPathVariable();

	bool variableExists(const std::string &name) const;
	bool isGlobalVariable(const std::string &name) const;
	bool isLocalVariable(const std::string &name) const;

	public:

	/**
	 * @brief Constructor reads ~/.profile and /etc/enviroment to get PATH variables and it use
	 * getpwuid to ged HOME and USER
	 */


	Environment();

	/**
	 * @brief Destructor delete tables
	 */

	~Environment();


	/**
	 * @brief Sets the variable to the given value. If it doesn't exist,
	 * 		  it shall be created
	 *
	 * @param name - must begin with a character and do not contatin
	 * 			any special characters nor a white space
	 * @param value - can be any C-string, can contain any ascii character, it can be empty
	 */
	void setVariable(const std::string &name, const std::string &value);

	/**
	 * @brief Get value of a variable.
	 * @param name - variable name
	 * @return variable value or empty string if the variable doesn't exist or has an empty value
	 */
	std::string getValue(const std::string &name);

	/**
	 * @brief Sets a given variable to be global - visible to bash subprocesses
	 * @param name - variable name
	 */
	void exportVariable(const std::string &name);

	/**
	 * @brief Returns a pointer to an array of environmental variables, which is
	 * 		  then passed to the exec function, when a new process is executed.
	 * @return Null-terminated array of pointers to char*.
	 */
	char **getEnvironment();

	/**
	 * @brief Expands a given path according to environmental variables:
	 * 			HOME, PWD, current_dir
	 *
	 * For example:
	 * Given: "./command", should return "/home/john/command"
	 * Same goes with "../command" or "~/command"
	 *
	 * Might be used to expand argument names
	 * and commands except for commands, which should be
	 * searched in PATH variable.
	 *
	 * @param name - relative or non-relative path
	 * @return expanded path
	 * @throws RuntimeException if the path is invalid
	 */
	std::string expandPath(const std::string &name);

	/**
	 * @brief Searches the PATH variable in order expand command name.
	 * @param name
	 * @return expanded path
	 * @throws RuntimeException if the command does not exist
	 */
	std::string searchPath(const std::string &name);

	void setCurrentDir(const std::string &path);

	std::string getCurrentDir(const std::string &path);

	int getMask();

	void setMask();

};


#endif //BASH_CLONE_ENVIRONMENT_H
