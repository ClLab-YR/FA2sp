#include "../FA2sp.h"

#include <MFC/ppmfc_cstring.h>

#include <string>

DEFINE_HOOK(4E5F30, String_To_Waypoint, 5)
{
    if (!ExtConfigs::ExtWaypoints)
        return 0;

	GET_STACK(const char*, pString, 0x4);

	auto process = [](const char* s)
	{
		int n = 0;
		int len = strlen(s);
		for (int i = len - 1, j = 1; i >= 0; i--, j *= 26)
		{
			int c = toupper(s[i]);
			if (c < 'A' || c > 'Z') return 0;
			n += ((int)c - 64) * j;
		}
		if (n <= 0)
			return -1;
		return n - 1;
	};

	R->EAX(process(pString));

	return 0x4E5F87;
}

DEFINE_HOOK(4E5F90, Waypoint_To_String, 7)
{
	if (!ExtConfigs::ExtWaypoints)
		return 0;

	GET_STACK(ppmfc::CString*, pString, 0x4);
	GET_STACK(int, nWaypoint, 0x8);

	auto process = [](int nWaypoint) -> const char*
	{
		static char buffer[8]{ '\0' };

		if (nWaypoint < 0)
			return "0";
		else if (nWaypoint == INT_MAX)
			return "FXSHRXX";
		else
		{
			++nWaypoint;
			int pos = 7;
			while (nWaypoint > 0)
			{
				--pos;
				char m = nWaypoint % 26;
				if (m == 0) m = 26;
				buffer[pos] = m + '@'; // '@' = 'A' - 1
				nWaypoint = (nWaypoint - m) / 26;
			}
			return buffer + pos;
		}
	};

	new(pString) ppmfc::CString(process(nWaypoint));

	R->EAX(pString);

	return 0x4E60FA;
}

//DEFINE_HOOK(4E97F4, CTeamTypes_OnCBCurrentTeamSelectChanged_String_To_Waypoint_1, 5)
//{
//	if (!ExtConfigs::ExtWaypoints)
//		return 0;
//
//	GET(CTeamTypes*, pThis, EBP);
//	REF_STACK(ppmfc::CString, lpWaypoint, STACK_OFFS(0x178, 0x160));
//
//	const int wp = ExtWaypoint::String_To_Waypoint(lpWaypoint);
//	if (wp != -1)
//		pThis->CString_Waypoint.Format("%d", wp);
//	else
//		pThis->CString_Waypoint = "";
//
//	return 0x4E9890;
//}
//
//DEFINE_HOOK(4E9959, CTeamTypes_OnCBCurrentTeamSelectChanged_String_To_Waypoint_2, 5)
//{
//	if (!ExtConfigs::ExtWaypoints)
//		return 0;
//
//	GET(CTeamTypes*, pThis, EBP);
//	REF_STACK(ppmfc::CString, lpWaypoint, STACK_OFFS(0x178, 0x128));
//
//	const int wp = ExtWaypoint::String_To_Waypoint(lpWaypoint);
//	if (wp != -1)
//		pThis->CString_TransportWaypoint.Format("%d", wp);
//	else
//		pThis->CString_TransportWaypoint = "None";
//
//	return 0x4E9A44;
//}