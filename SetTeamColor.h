
#ifdef SETTEAMCOLOR_EXPORTS
#define SETTEAMCOLOR_API __declspec(dllexport)
#else
#define SETTEAMCOLOR_API __declspec(dllimport)
#endif

namespace SetTeamColor {
	bool init();
}