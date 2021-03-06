#ifndef BASH_CLONE_ENVIRONMENT_H
#define BASH_CLONE_ENVIRONMENT_H
#include "Command.h"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <cctype>

#include <vector>
#include <memory>
#include <unordered_map>

#include "BashExceptions.h"

/**
 *	@class Environment
 *
 *	@brief Used by other classes to access environmental data,
 *	especially environmental variables.
 */

class Environment {
private:

	enum TypeOfVariable {
		localVariable,
		globalVariable
	};

	struct EnvironmentVariable {
		std::string name_;
		std::string value_;
		int variableType_;

		EnvironmentVariable(const std::string name, const std::string value, const int variableType)
				:name_(name), value_(value), variableType_(variableType)
		{};

		std::string getName() const
		{
			return name_;
		}

		std::string getValue() const
		{
			return value_;
		}

		void setValue(const std::string &newValue) {
			value_ = newValue;
		}

		void setGlobal(){
			variableType_ = globalVariable;
		}

		bool isGlobal() const
		{
			return variableType_ == globalVariable;
		}
	};

	std::unordered_map<std::string, EnvironmentVariable*> variablesMap_;
	std::string currentDir_;
	int returnCode;

private:

	/**
	 * @brief Get actual user name by function getpwuid
	 * @return user name
	 */

	const char *getUserName();

	/**
	 * @brief Get user home dir by function getpwuid
	 * @return user home dir name
	 */

	const char *getUserHomeDir();

	/**
	 * @brief Insert into variables map "USER" env variable
	 */

	void loadUserVariable();

	/**
	 * @brief Insert into variables map "HOME" and "PWD" env variable
	 */

	void loadHomeVariable();

	/**
	 * @brief Check if exist env variable with given name
	 * @param name env variable name
	 * @return true if exist, false if not exist
	 */

	bool variableExists(const std::string &name) const;

	/**
	 * @brief Check if env variable with given name is global
	 * @param name env variable name
	 * @return true if is global, false if not
	 */

	bool isGlobalVariable(const std::string &name) const;

	/**
	 * @brief Check if env variable with given name is local
	 * @param name env variable name
	 * @return trus if is global, false if is local
	 */

	bool isLocalVariable(const std::string &name) const;

	/**
	 * @brief 'Cuts' the "PATH" variable into pieces - directories names
	 * @return vector of "PATH" pieces
	 */

	std::vector<std::string> getPathPieces();

	/**
	 * @brief It use dirent.h API to search in given directory file
	 * @param dirName directory name
	 * @param name wanted file name
	 * @return file path or empty string if is not in that directory
	 */

	bool searchInDir(const std::string dirName, std::string name) const;

	/**
	 * @brief Check if given env variable name is valid with Std 1003.1-2001
	 * @param name name to check
	 * @return true if is valid, false if is bot valid
	 */

	bool checkIfEnvVariableNameIsValid(const std::string name ) const;

	/**
	 * @brief Get parent dir for given directory
	 * @return parent directory name
	 */

	std::string getParentDir(const std::string dirName) const;

	/**
	 * @brief Function use by expand path. It give first 'piece' of path - which mean
	 * string beetwen '/' and '/' or '/' and end of file
	 * @param path path to get piece
	 * @return piece of path
	 */

	std::string getPathPiece(std::string path);


	/**
	 * @brief Function cuts the first from left dir path. Name must(!) start with "/"!
	 * @param name path to cut
	 * @return cut path
	 */

	std::string cutPath(std::string name);

public:
	/**
	 * @brief Constructor reads ~/.profile and /etc/enviroment to get PATH variables and it use
	 * getpwuid to ged HOME and USER
	 */

	Environment();

	/**
	 * @brief Destructor clear variables map
	 */

	~Environment();


	/**
	 * @brief Sets the variable to the given value. If it doesn't exist,
	 * 		  it shall be created
	 *
	 * @param name - must begin with a character and do not contain
	 * 			any special characters nor a white space
	 * @param value - can be any C-string, can contain any ascii character, it can be empty
	 */

	void setVariable(const std::string &name, const std::string &value);

	/**
	 * @brief Get value of a variable.
	 * @param name - variable name
	 * @return variable value or empty string if the variable doesn't exist or has an empty value
	 */

	std::string getValue(const std::string &name) const;

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

	char **getEnvironment() const;

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

	/**
	 * @brief Change current dir to given argument.
	 * @param path new current dir path
	 */

	void setCurrentDir(const std::string &path);

	/**
	 * @brief Returns current path.
	 * @return current path
	 */

	std::string getCurrentDir() const;

	/**
	 * @brief Checks if dir with given exists. It simply use closedir function.
	 * @param path absolute path to directory
	 * @return true if exists, false if not exists
	 */

	bool checkIfDirExists(std::string path) const;

	/**
	 * @brief Check if exist command with given name.
	 * @param command name of command
	 * @return true if exist, false if not exist
	 */

	std::string resolveCommand(std::string &command);

	/**
	 * @brief Get the value of return value from last program
	 * @return return value of last program
	 */

	int getReturnCode() const;

	/**
	 * @brief Set the value of return value from last program
	 * @param returnCode value to set
	 */

	void setReturnCode(int returnCode);

	/**
	 * @brief Counts the number of global variables in variablesMap.
 	* @return number of global variables
	 */
	int countGlobalVariables() const;

};


#endif //BASH_CLONE_ENVIRONMENT_H
