#include "schedule.h"

namespace vnpge {

// Nullary function that's handy for use as a breakpoint hook. In the debugger, set a breakpoint on it and step until you've returned
// If using gdb, typing 'break brkpoint()' will accomplish this
// NOLINTNEXTLINE(misc-definitions-in-headers)
void brkpoint() {
	// Keeping a copy of this here in case I need some good ol' print-debugging on which execution path is taken
	std::cout << __FILE__ << " " << __LINE__ << std::endl;
	return;
}
}