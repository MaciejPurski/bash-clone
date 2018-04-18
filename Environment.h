#ifndef BASH_CLONE_ENVIRONMENT_H
#define BASH_CLONE_ENVIRONMENT_H
#include "Command.h"

/**
 *	@class Environment
 *
 *	@brief Used by other classes to access environmental data,
 *	especially environmental variables.
 */
class Environment {

	char **table;

	public:
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

	Environment() {}

};


#endif //BASH_CLONE_ENVIRONMENT_H
