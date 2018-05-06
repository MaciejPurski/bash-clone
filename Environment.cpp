#include "Environment.h"


const char *getUserName()
{
	uid_t uid = geteuid();              // get effective user ID
	struct passwd *pw = getpwuid(uid);  // get pointer to pw struct
	if (pw)
	{
		return pw->pw_name;
	}

	return "";
}

const char *getUserHomeDir()
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if(pw)
	{
		return pw->pw_dir;
	}

	return "";
}

Environment::Environment()
{
	loadUserVariable();

	loadHomeVariable();

	loadPathVariable();

	const char *homeDir = getUserHomeDir();
	currentDir_= homeDir;
}

Environment::~Environment()
{

}

void Environment::loadUserVariable() {
	const char *userName = getUserName();

	globalVariables_.push_back(std::make_unique<EnvironmentVariable>("USER", std::string(userName)));
}

void Environment::loadHomeVariable() {
	const char *homeDir = getUserHomeDir();

	globalVariables_.push_back(std::make_unique<EnvironmentVariable>("HOME", std::string(homeDir)));

}

void Environment::loadPathVariable() {
	std::fstream pathFile;
	std::string lineFromFile;

	std::string pathFromEnvironment;
	std::string pathFromProfile;
	std::string completePathValue;


	// /etc/environment:

	pathFile.open("/etc/environment", std::ios_base::in);

	while(std::getline(pathFile, lineFromFile))
	{
		if(lineFromFile.find("PATH=") == 0) {
			break;
		}
	}
	pathFile.close();
	pathFromEnvironment = lineFromFile.substr(6, lineFromFile.length()-7);


	// HOME/.profile

	pathFile.open( std::string(getValue("HOME")+"/.profile"), std::ios_base::in);

	while(std::getline(pathFile, lineFromFile))
	{
		if(lineFromFile.find("PATH") == 0) {
			break;
		}
	}
	pathFile.close();
	pathFromProfile = lineFromFile.substr(6, lineFromFile.length()-12);

	int homeIndex;
	while( (homeIndex=pathFromProfile.find("$HOME") )!= std::string::npos )
	{
		pathFromProfile = pathFromProfile.substr(0, homeIndex) + getValue("HOME") + pathFromProfile.substr(homeIndex + 5);
	}

	completePathValue = pathFromProfile + pathFromEnvironment;

	globalVariables_.push_back(std::make_unique<EnvironmentVariable>("PATH", completePathValue));
}

bool Environment::isGlobalVariable(const std::string &name) const {
	for(auto it = globalVariables_.begin(); it!=globalVariables_.end(); ++it)
	{
		if((*it)->getName() == name)
			return true;
	}

	return false;
}

bool Environment::isLocalVariable(const std::string &name) const {

	for(auto it = localVariables_.begin(); it!=localVariables_.end(); ++it)
	{
		if((*it)->getName() == name)
			return true;
	}

	return false;
}

bool Environment::variableExists(const std::string &name) const {
	return (isLocalVariable(name) || isGlobalVariable(name));
}


void Environment::setVariable(const std::string &name, const std::string &value) {
	if(variableExists(name))   // variable exists
	{
		if(isGlobalVariable(name)) // is global
		{
			for(auto it = globalVariables_.begin(); it!=globalVariables_.end(); ++it)
			{
				if((*it)->getName() == name)
					(*it)->setValue(value);
			}
		}
		else                        // is local
		{
			for(auto it = localVariables_.begin(); it!=localVariables_.end(); ++it)
			{
				if((*it)->getName() == name)
					(*it)->setValue(value);
			}
		}

	}

	else                        // variables not exists
	{
		localVariables_.push_back(std::make_unique<EnvironmentVariable>(name, value));
	}
}

std::string Environment::getValue(const std::string &name)
{

	if(variableExists(name))
	{
		if(isGlobalVariable(name)) // is global
		{
			for(auto it = globalVariables_.begin(); it!=globalVariables_.end(); ++it)
			{
				if((*it)->getName() == name)
					return (*it)->getValue();
			}
		}
		else                        // is local
		{
			for(auto it = localVariables_.begin(); it!=localVariables_.end(); ++it)
			{
				if((*it)->getName() == name)
					return (*it)->getValue();
			}
		}
	}

	return std::__cxx11::string();
}

void Environment::exportVariable(const std::string &name) {
	if(isLocalVariable(name))
	{
		for(auto it = localVariables_.begin(); it!=localVariables_.end(); ++it)
		{
			if((*it)->getName() == name) {
				globalVariables_.push_back(std::move(*it));
				localVariables_.erase(it);
				break;
			}
		}
	}
}



char **Environment::getEnvironment() {
	char **tmp= new char*[globalVariables_.size() + 1]; // +1 because of /0 as last one

	int i=0;
	for(auto it = globalVariables_.begin(); it!=globalVariables_.end(); ++it)
	{
		tmp[i] = new char[(*it)->getName().length() + (*it)->getValue().length() + 2]; // +2 because of '=' and '\0'
		std::string tmpStr = (*it)->getName() + "=" + (*it)->getValue();
		strcpy(tmp[i], tmpStr.c_str());
		++i;
	}
	tmp[i]=new char[1];
	tmp[i][0]='\0';

	return tmp;
}

std::string Environment::expandPath(const std::string &name) {
	return std::__cxx11::string();
}

void Environment::setCurrentDir(const std::string &path) {
	currentDir_=path;
}

std::string Environment::getCurrentDir(const std::string &path) {
	return currentDir_;
}

int Environment::getMask() {
	return 0;
}

void Environment::setMask() {

}

std::string Environment::searchPath(const std::string &name) {
	return std::__cxx11::string();
}
