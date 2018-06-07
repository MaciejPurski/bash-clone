#ifndef BASH_CLONE_BASHEXCEPTIONS_H
#define BASH_CLONE_BASHEXCEPTIONS_H

#include <exception>

class BashCloneException : public std::exception
{
protected:
	std::string msg_;

public:

	BashCloneException(const std::string &msg = "")
	{
		msg_ = "BashClone exception! Message: "+ msg;
	}

	virtual const char* what() const noexcept override
	{
		return msg_.c_str();
	}
};

class EnvironmentException : public BashCloneException
{
public:
	EnvironmentException(const std::string& msg = "")
	{
		msg_= "Environment exception! Message: "+ msg;
	}
};

class EmptyPath : public EnvironmentException
{
public:
	EmptyPath(const std::string&msg )
	{
		msg_ = "Empty path given! Message: "+ msg;
	}

};

class UnknownCommand : public EnvironmentException
{
public:
	UnknownCommand(const std::string&msg)
	{
		msg_ = "Unknown command! Message: "+msg;
	}

};

class UnknownProgram : public EnvironmentException
{
public:
	UnknownProgram(const std::string&msg )
	{
		msg_ = "Unknown program! Message: "+ msg;
	}
};


class EngineException : public BashCloneException
{
public:

	EngineException(const std::string&msg ="")
	{
		msg_ = "Engine exception: "+ msg;
	}
};

class ChildProcessException : public BashCloneException
{
public:

	ChildProcessException(const std::string&msg ="")
	{
		msg_ = "Child process exception:"+ msg;
	}
};

class InterpreterException : public BashCloneException
{
public:

	InterpreterException(const std::string&msg ="")
	{
		msg_ = "Interpreter exception! Message: "+ msg;
	}
};

class TerminalException :public BashCloneException
{
public:

	TerminalException(const std::string&msg ="")
	{
		msg_ = "Shell exception! Message: "+ msg;
	}
};

#endif //BASH_CLONE_BASHEXCEPTIONS_H
