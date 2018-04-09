#ifndef BASH_CLONE_ENVIRONMENT_H
#define BASH_CLONE_ENVIRONMENT_H
#include "Command.h"

class Environment {
public:
	int setVariable(AssignCommand *a);
};


#endif //BASH_CLONE_ENVIRONMENT_H
