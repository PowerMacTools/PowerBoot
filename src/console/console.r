#include "Dialogs.r"
#include "Processes.r"
#include "MacTypes.r"
#include "Finder.r"
#include "Menus.r"
#include "Controls.r"
#include "ControlDefinitions.r"

resource 'SIZE' (-1) {
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	is32BitCompatible,
	notHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	2*1024*1024,
	2*1024*1024
};