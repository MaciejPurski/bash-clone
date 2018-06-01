#ifndef BASH_CLONE_JOB_H
#define BASH_CLONE_JOB_H

#include <string>
#include <vector>
enum State {Stopped, Running, Done};

class Job {
private:
	enum State state;
	int gpid;
	std::vector<int> pids;
	std::string commandLine;
	bool isBlocking;
public:

	Job(int ngpid, std::vector<int> &npids, std::string &ncommandLine,
	    bool nisBlocking) : gpid(ngpid), pids(npids), commandLine(ncommandLine),
                            isBlocking(nisBlocking) {
		state = Running;
	}

	void setStopped() {

	}
};


#endif //BASH_CLONE_JOB_H
