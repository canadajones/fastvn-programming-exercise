#ifndef DEBUG_TOOLS_HEADER
#define DEBUG_TOOLS_HEADER
#include <iostream>
#define DEBUG_EXEC_HERE std::cout << "Executed line " << __LINE__ << ", in function " << __func__ << ", in file " << __FILE__ << std::endl;


// Nullary function that's handy for use as a breakpoint hook. In the debugger, set a breakpoint on it and step until you've returned
// If using gdb, typing 'break brkpoint()' will accomplish this
void inline brkpoint() {
	// Keeping a copy of this here in case I need some good ol' print-debugging on which execution path is taken
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}


#endif // !DEBUG_TOOLS_HEADER
