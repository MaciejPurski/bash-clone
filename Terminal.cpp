//
// Created by maciej on 18.04.18.
//

#include <algorithm>
#include "Terminal.h"

void Terminal::start() {
	// init script
	run(initFile, false);

	if (interactive)
		run(std::cin, true);
	else
		run(scriptFile, false);
}

void Terminal::run(std::istream &input, bool interactiveMode) {
	std::string line;

	while (true) {
		if (interactiveMode)
			std::cout << env.getValue("USER") << ":" << env.getValue("PWD") << "$ ";

		if (!std::getline(input, line))
			break;

		// if a continuation is needed
		if (!line.empty()) {
			while (line.back() == '\\'
			       || ((std::count(line.begin(), line.end(), '\'') % 2) != 0)) {
				std::string continuation;

				if (interactiveMode)
					std::cout << ">  ";
				std::getline(input, continuation);
				line.append(continuation);
			}
		}

		try {
			CommandLine c = interpreter.nextCommandLine(line);

			engine.executeCommandLine(c);
		} catch (std::exception &e) {
			// TODO exception handling
			e.what();
		}

	}

}
