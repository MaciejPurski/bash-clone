#ifndef BASH_CLONE_BASHEXCEPTIONS_H
#define BASH_CLONE_BASHEXCEPTIONS_H

#include <exception>

class BashCloneException : public std::exception
{
protected:
	std::string msg_;

public:

	BashCloneException(const std::string&msg )
		 	:msg_(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "BashClone exception! Message: "+ msg_;
		return tmp.c_str();
	}
};

class EnvironmentException : public BashCloneException
{
public:
	EnvironmentException(const std::string&msg )
			:BashCloneException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Environment exception! Message: "+ msg_;
		return tmp.c_str();
	}
};

class EmptyPath : public EnvironmentException
{
public:
	EmptyPath(const std::string&msg )
			:EnvironmentException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Empty path given! Message: "+ msg_;
		return tmp.c_str();
	}
};

class UnknownProgram : public EnvironmentException
{
public:
	UnknownProgram(const std::string&msg )
			:EnvironmentException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Unknown program! Message: "+ msg_;
		return tmp.c_str();
	}
};

class EngineException : public BashCloneException
{
public:

	EngineException(const std::string&msg )
			:BashCloneException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Engine exception! Message: "+ msg_;
		return tmp.c_str();
	}

};

class InterpreterException : public BashCloneException
{
public:

	InterpreterException(const std::string&msg )
			:BashCloneException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Interpreter exception! Message: "+ msg_;
		return tmp.c_str();
	}

};

class TerminalException :public BashCloneException
{
public:

	TerminalException(const std::string&msg )
			:BashCloneException(msg)
	{}

	virtual const char* what() const noexcept override
	{
		std::string tmp = "Terminal exception! Message: "+ msg_;
		return tmp.c_str();
	}

};

#endif //BASH_CLONE_BASHEXCEPTIONS_H
