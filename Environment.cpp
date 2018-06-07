#include "Environment.h"

#include <cstdio>

const char *Environment::getUserName() {
	uid_t uid = geteuid();              // get effective user ID
	struct passwd *pw = getpwuid(uid);  // get pointer to pw struct
	if (pw) {
		return pw->pw_name;
	}

	return "";
}

const char *Environment::getUserHomeDir() {
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw) {
		return pw->pw_dir;
	}

	return "";
}

void Environment::loadUserVariable() {
	const char *userName = getUserName();

	variablesMap_.insert(std::make_pair(std::string("USER"), new EnvironmentVariable(
			"USER", std::string(userName), globalVariable)));
}

void Environment::loadHomeVariable() {
	const char *homeDir = getUserHomeDir();

	variablesMap_.insert(std::make_pair(std::string("HOME"), new EnvironmentVariable(
			"HOME", std::string(homeDir), globalVariable)));

	variablesMap_.insert(std::make_pair(std::string("PWD"), new EnvironmentVariable(
			"PWD", std::string(homeDir), globalVariable)));
}

bool Environment::variableExists(const std::string &name) const {
	auto it = variablesMap_.find(name);

	return it != variablesMap_.end();

}

bool Environment::isGlobalVariable(const std::string &name) const {
	auto it = variablesMap_.find(name);

	return it != variablesMap_.end() && it->second->isGlobal();

}

bool Environment::isLocalVariable(const std::string &name) const {
	auto it = variablesMap_.find(name);

	return it != variablesMap_.end() && !it->second->isGlobal();

}



std::vector<std::string> Environment::getPathPieces() {
	unsigned int index = 0;
	std::string tmpPath = getValue("PATH");
	std::string tmpString;

	std::vector<std::string> pathPieces;

	while (index < tmpPath.length()) {
		if (tmpPath[index] == ':') {
			pathPieces.push_back(tmpString);
			tmpString.clear();
		} else {
			tmpString += tmpPath[index];
		}
		++index;
	}

	return pathPieces;
}

bool Environment::searchInDir(const std::string dirName, const std::string name) const {
	DIR *dir = opendir(dirName.c_str());

	if (dir == NULL) {
		return false;
	}

	struct dirent *sd;
	while ((sd = readdir(dir)) != NULL) {

		if (strcmp(sd->d_name, name.c_str()) == 0) {
			return true;
		}
	}
	closedir(dir);

	return false;
}

bool Environment::checkIfEnvVariableNameIsValid(const std::string name) const {
	if (name.empty())
		return false;

	char tmp = 0;
	unsigned int index = 0;

	tmp = name[index];
	if (!isupper(static_cast<int>(tmp))) {
		return false;
	}
	++index;

	while (index < name.length()) {
		tmp = name[index];
		if (!isupper(static_cast<int>(tmp)) && !isdigit(static_cast<int>(tmp)) && tmp != '_')
			return false;
	}

	return true;
}

std::string Environment::getParentDir(const std::string dirName) const {
	if (dirName == "/") // dir / don't have parent...
		return dirName;

	if (dirName.find("/") == std::string::npos) // is just someting strange... maybe exception?
	{
		return dirName;
	}

	unsigned long index;
	for (index = dirName.length(); index >= 0; --index) {
		if (dirName[index] == '/') {
			break;
		}
	}

	if (index == 0)
		return "/";
	else
		return dirName.substr(0, index);
}

std::string Environment::getPathPiece(std::string path) {
	int index = 1;

	if (path.length() <= 1)
		return "";

	std::string tmp;

	while (index < path.length()) {
		if (path[index] == '/')
			return tmp;
		tmp = tmp + path[index];
		++index;
	}

	return tmp;
}

int Environment::countGlobalVariables() const {
	int n = 0;
	for (auto &var : variablesMap_) {
		if (var.second->isGlobal())
			n++;
	}

	return n;
}

std::string Environment::cutPath(std::string name) {
	int index = 0;
	if (name.length() <= 1)
		return "";

	for (index = 1; index < name.length(); ++index) {
		if (name[index] == '/')
			break;
	}

	if (index == name.length())
		return "";
	else
		return name.substr(index, name.length());
}

Environment::Environment() {
	loadUserVariable();

	loadHomeVariable();

	returnCode = 0;

	const char *homeDir = getUserHomeDir();
	currentDir_ = homeDir;
}

Environment::~Environment() {
	for (auto &it : variablesMap_) {
		delete it.second;
	}
}

void Environment::setVariable(const std::string &name, const std::string &value) {
	auto it = variablesMap_.find(name);

	if (it != variablesMap_.end()) {
		it->second->setValue(value);
	} else {
		variablesMap_.insert(std::make_pair(name, new EnvironmentVariable(
				EnvironmentVariable(name, value, localVariable))));
	}
}

std::string Environment::getValue(const std::string &name) const {

	if(name=="?")
		return std::to_string(getReturnCode());

	auto it = variablesMap_.find(name);

	if (it != variablesMap_.end())
		return it->second->getValue();

	return std::__cxx11::string();
}

void Environment::exportVariable(const std::string &name) {
	auto it = variablesMap_.find(name);
	if (it != variablesMap_.end())
		it->second->setGlobal();
}

char **Environment::getEnvironment() const {
	int nGlobals = countGlobalVariables();

	char **tmp = new char *[nGlobals + 1]; // +1 because of /0 as last one

	int i = 0;
	for (auto &it : variablesMap_) {
		if (!it.second->isGlobal())
			continue;
		tmp[i] = new char[it.second->getName().length() + it.second->getValue().length() +
		                  2]; // +2 because of '=' and '\0'
		std::string tmpStr = it.second->getName() + "=" + it.second->getValue();
		strcpy(tmp[i], tmpStr.c_str());
		++i;
	}
	tmp[i] = NULL;

	return tmp;
}


std::string Environment::expandPath(const std::string &name) {
	if (name.empty()) {
		throw EmptyPath("Throw when try to use function expandPath");
	}

	std::string tmp = name;
	std::string toReturn = "/";

	if(tmp[0] == '~')
	{
		tmp = getValue("HOME")+"/"+tmp.substr(1, tmp.length());
	}
	else if(name.length()>=2 && (tmp[0] == '.' && tmp[1] == '.'))
	{
		tmp = currentDir_+"/"+tmp.substr(0, tmp.length());
	}
	else if(tmp[0] == '.')
	{
		tmp = getParentDir(currentDir_) + "/" + tmp.substr(2, tmp.length());
	}
	else if(tmp[0] != '/')
	{
		tmp = currentDir_+"/"+tmp;
	}

	while (!tmp.empty()) {
		std::string pathPiece = getPathPiece(tmp);
		if (!pathPiece.empty() && pathPiece != ".") {
			if (pathPiece == "..") {
				toReturn = getParentDir(toReturn.substr(0, toReturn.length() - 1));
			} else if (pathPiece == "~") {
				toReturn = toReturn + getValue("HOME");
			} else {
				toReturn = toReturn + pathPiece;
			}


			if (toReturn != "/") {
				std::string parentDir = getParentDir(toReturn);
				if (!searchInDir(parentDir, pathPiece)) {
					return "";
				}
			}

			toReturn += "/";
		}

		tmp = cutPath(tmp);
	}


	if (checkIfDirExists(toReturn)) {
		if(toReturn == "//")
			return "/";
		return toReturn;
	}
		else
		return toReturn.substr(0, toReturn.length() - 1);

}

std::string Environment::searchPath(const std::string &name) {
	std::vector<std::string> pathPieces = getPathPieces();
	std::string resultPath;

	for (auto &pathPiece : pathPieces) {
		if (searchInDir(pathPiece, name)) {
			resultPath = pathPiece + "/" + name;
			break;
		}
	}

	return resultPath;
}

void Environment::setCurrentDir(const std::string &path) {
	currentDir_ = path;

	setVariable("PWD", path);
}

std::string Environment::getCurrentDir() const {
	if (currentDir_.back() != '/')
		return currentDir_ + '/';
	else
		return currentDir_;
}

bool Environment::checkIfDirExists(std::string path) const {
	DIR *dir = opendir(path.c_str());
	if (dir) {
		/* Directory exists. */
		closedir(dir);
		return true;
	}

	return false;
}

std::string Environment::resolveCommand(std::string &command) {
	std::string fullPath;

	if (command.front() == '/' || command.front() == '.' ||
	    command.front() == '~' || command.substr(0, 2) == "..") {
		fullPath = expandPath(command);
	} else {
		fullPath = searchPath(command);
		if (fullPath.empty())
			throw UnknownCommand("Program name: " + command);
	}

	return fullPath;
}

int Environment::getReturnCode() const {
	return returnCode;
}

void Environment::setReturnCode(int returnCode) {
	Environment::returnCode = returnCode;
}
