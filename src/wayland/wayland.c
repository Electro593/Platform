/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define WAYLAND_FUNCS \
	EXPORT(b08, Wayland_CreateWindow, c08 *Title, usize Width, usize Height)

#endif

#ifdef INCLUDE_SOURCE

internal b08
Wayland_CreateWindow(c08 *Title, usize Width, usize Height)
{
	return FALSE;
}

#endif
