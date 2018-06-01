#include "Environment.h"

#include <stdio.h>


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

	variablesMap_.insert(std::make_pair(std::string("USER"), std::make_unique<EnvironmentVariable>(
			"USER", std::string(userName), globalVariable)));
}

void Environment::loadHomeVariable() {
	const char *homeDir = getUserHomeDir();

	variablesMap_.insert(std::make_pair(std::string("HOME"), std::make_unique<EnvironmentVariable>(
			"HOME", std::string(homeDir), globalVariable)));

	variablesMap_.insert(std::make_pair(std::string("PWD"), std::make_unique<EnvironmentVariable>(
			"PWD", std::string(homeDir), globalVariable)));
}

void Environment::loadPathVariable() {
	std::fstream pathFile;
	std::string lineFromFile;

	std::string pathFromEnvironment;
	std::string pathFromProfile;
	std::string completePathValue;


	// /etc/environment:

	pathFile.open("/etc/environment", std::ios_base::in);

	while (std::getline(pathFile, lineFromFile)) {
		if (lineFromFile.find("PATH=") == 0) {
			break;
		}
	}
	pathFile.close();
	pathFromEnvironment = lineFromFile.substr(6, lineFromFile.length() - 7);


	// HOME/.profile

	pathFile.open(std::string(getValue("HOME") + "/.profile"), std::ios_base::in);

	while (std::getline(pathFile, lineFromFile)) {
		if (lineFromFile.find("PATH") == 0) {
			break;
		}
	}
	pathFile.close();
	pathFromProfile = lineFromFile.substr(6, lineFromFile.length() - 12);

	int homeIndex;
	while ((homeIndex = pathFromProfile.find("$HOME")) != std::string::npos) {
		pathFromProfile =
				pathFromProfile.substr(0, homeIndex) + getValue("HOME") + pathFromProfile.substr(homeIndex + 5);
	}

	completePathValue = pathFromProfile + pathFromEnvironment;

	variablesMap_.insert(std::make_pair(std::string("PATH"), std::make_unique<EnvironmentVariable>(
			"PATH", std::string(completePathValue), globalVariable)));
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

std::vector<std::string> Environment::getPathPieces() const {
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

bool Environment::searchInDir(const std::string dirName, const std::string name) {
	DIR *dir = opendir(dirName.c_str());

	if(dir == NULL)
	{
		return false;
	}

	struct dirent * sd;
	while((sd = readdir(dir)) != NULL){

		if(strcmp(sd->d_name, name.c_str()) == 0)
		{
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

	return dirName.substr(0, index);
}

std::string Environment::getPathPiece(std::string path)
{
	int index = 1;

	if(path.length() <= 1)
		return "";

	std::string tmp;

	while(index<path.length())
	{
		if(path[index] == '/')
			return tmp;
		tmp = tmp + path[index];
		++index;
	}

	return tmp;
}

Environment::Environment() {
	loadUserVariable();

	loadHomeVariable();

	loadPathVariable();

	const char *homeDir = getUserHomeDir();
	currentDir_ = homeDir;
}

Environment::~Environment() {
	variablesMap_.clear();
}

void Environment::setVariable(const std::string &name, const std::string &value) {
	auto it = variablesMap_.find(name);

	if(it!=variablesMap_.end()) {
		it->second->setValue(value);
	} else {
		variablesMap_.insert(std::make_pair(name, std::unique_ptr<EnvironmentVariable>(new EnvironmentVariable(name, value, localVariable))));
	}
}

std::string Environment::getValue(const std::string &name) const {
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

char **Environment::getEnvironment() {
	char **tmp = new char *[variablesMap_.size() + 1]; // +1 because of /0 as last one

	int i = 0;
	for (auto &it : variablesMap_) {
		tmp[i] = new char[it.second->getName().length() + it.second->getValue().length() + 2]; // +2 because of '=' and '\0'
		std::string tmpStr = it.second->getName() + "=" + it.second->getValue();
		strcpy(tmp[i], tmpStr.c_str());
		++i;
	}
	tmp[i] = NULL;

	return tmp;
}

std::string cutPath(std::string name){
	int index = 0;
	if(name.length()<=1)
		return "";

	for(index = 1; index<name.length(); ++index)
	{
		if(name[index] == '/')
			break;
	}

	if(index == name.length())
		return "";
	else
		return name.substr(index, name.length());
}

std::string Environment::expandPath(const std::string &name) {
	if(name.empty())
	{
		throw EmptyPath("Throw when try to use function expandPath");
	}

	std::string tmp = name;
	std::string toReturn = "/";

	if(tmp[0] == '~')
	{
		tmp = getValue("HOME")+"/"+tmp.substr(1, tmp.length());
	}
	else if(tmp[0] == '.')
	{
		tmp = currentDir_+"/"+tmp.substr(1, tmp.length());
	}
	else if(tmp[0] == '.' && tmp[1] == '.')
	{
		tmp = getParentDir(currentDir_) + "/" + tmp.substr(2, tmp.length());
	}
	else if(tmp[0] != '/')
	{
		tmp = currentDir_+"/"+tmp;
	}

	while(!tmp.empty())
	{
		std::string pathPiece = getPathPiece(tmp);
		if(!pathPiece.empty() && pathPiece!=".")
		{
			if(pathPiece == "..")
			{
				toReturn = getParentDir(toReturn.substr(0,toReturn.length()-1));
			}
			else if(pathPiece == "~")
			{
				toReturn = toReturn+getValue("HOME");
			}
			else
			{
				toReturn =toReturn+pathPiece;
			}
			toReturn+="/";
		}

		if(toReturn!="/" && !checkIfDirExists(toReturn.substr(0, toReturn.length()-1)) )
		{
			return "";
		}

		tmp = cutPath(tmp);
	}


	return toReturn;

}

std::string Environment::searchPath(const std::string &name) {
	std::vector<std::string> pathPieces = getPathPieces();
	std::string tmp;

	for (auto &pathPiece : pathPieces) {
		if (searchInDir(pathPiece, name)) {
			tmp += pathPiece;
			break;
		}
	}

	if(tmp.empty())
	{
		throw UnknownProgram("Program name "+ name);
	}

	if (!tmp.empty()) {
		tmp += "/";
		tmp += name;
	}

	return tmp;
}

void Environment::setCurrentDir(const std::string &path) {
	currentDir_ = path;
}

std::string Environment::getCurrentDir() {
	return currentDir_;
}

bool Environment::checkIfDirExists(std::string path) {
	DIR* dir = opendir(path.c_str());
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
		return true;
	}
	else if (ENOENT == errno)
	{
		/* Directory does not exist. */
		return false;
	}
		/* opendir() failed for some other reason. */
		return false;
}
