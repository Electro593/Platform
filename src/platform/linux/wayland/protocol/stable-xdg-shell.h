/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "core.h"

#define XDG_WM_BASE                                                 \
	((XdgWmBase, xdg_wm_base, 7),                                   \
		D((Destroy)),                                               \
		R((CreatePositioner), N(XDG_POSITIONER)),                   \
		R((GetXdgSurface), N(XDG_SURFACE), O(Surface, WL_SURFACE)), \
		R((Pong), U(Serial)),                                       \
		E((Ping), U(Serial)))

typedef enum xdg_wm_base_error : u32 {
	XDG_WM_BASE_ERROR_ROLE					= 0,
	XDG_WM_BASE_ERROR_DEFUNCT_SURFACES		= 1,
	XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP = 2,
	XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT	= 3,
	XDG_WM_BASE_ERROR_INVALID_SURFACE_STATE = 4,
	XDG_WM_BASE_ERROR_INVALID_POSITIONER	= 5,
	XDG_WM_BASE_ERROR_UNRESPONSIVE			= 6,
} xdg_wm_base_error;

#define XDG_POSITIONER                                                      \
	((XdgPositioner, xdg_positioner, 7),                                    \
		D((Destroy)),                                                       \
		R((SetSize), I(Width), I(Height)),                                  \
		R((SetAnchorRect), I(X), I(Y), I(Width), I(Height)),                \
		R((SetAnchor), U(Anchor, xdg_positioner_anchor)),                   \
		R((SetGravity), U(Gravity, xdg_positioner_gravity)),                \
		R((SetConstraintAdjustment),                                        \
			U(ConstraintAdjustment, xdg_positioner_constraint_adjustment)), \
		R((SetOffset), I(X), I(Y)),                                         \
		R((SetReactive, 3)),                                                \
		R((SetParentSize, 3), I(ParentWidth), I(ParentHeight)),             \
		R((SetParentConfigure, 3), U(Serial)))

typedef enum xdg_positioner_error : u32 {
	XDG_POSITIONER_ERROR_INVALID_INPUT = 0
} xdg_positioner_error;

typedef enum xdg_positioner_anchor : u32 {
	XDG_POSITIONER_ANCHOR_NONE		   = 0,
	XDG_POSITIONER_ANCHOR_TOP		   = 1,
	XDG_POSITIONER_ANCHOR_BOTTOM	   = 2,
	XDG_POSITIONER_ANCHOR_LEFT		   = 3,
	XDG_POSITIONER_ANCHOR_RIGHT		   = 4,
	XDG_POSITIONER_ANCHOR_TOP_LEFT	   = 5,
	XDG_POSITIONER_ANCHOR_BOTTOM_LEFT  = 6,
	XDG_POSITIONER_ANCHOR_TOP_RIGHT	   = 7,
	XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT = 8,
} xdg_positioner_anchor;

typedef enum xdg_positioner_gravity : u32 {
	XDG_POSITIONER_GRAVITY_NONE			= 0,
	XDG_POSITIONER_GRAVITY_TOP			= 1,
	XDG_POSITIONER_GRAVITY_BOTTOM		= 2,
	XDG_POSITIONER_GRAVITY_LEFT			= 3,
	XDG_POSITIONER_GRAVITY_RIGHT		= 4,
	XDG_POSITIONER_GRAVITY_TOP_LEFT		= 5,
	XDG_POSITIONER_GRAVITY_BOTTOM_LEFT	= 6,
	XDG_POSITIONER_GRAVITY_TOP_RIGHT	= 7,
	XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT = 8,
} xdg_positioner_gravity;

typedef enum xdg_positioner_constraint_adjustment : u32 {
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE	  = 0,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X  = 1,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y  = 2,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X	  = 4,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y	  = 8,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X = 16,
	XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y = 32,
} xdg_positioner_constraint_adjustment;

#define XDG_SURFACE                                              \
	((XdgSurface, xdg_surface, 7),                               \
		D((Destroy)),                                            \
		R((GetToplevel), N(XDG_TOPLEVEL)),                       \
		R((GetPopup),                                            \
			N(XDG_POPUP),                                        \
			O(Parent, , TRUE),                                   \
			O(Positioner, XDG_POSITIONER)),                      \
		R((SetWindowGeometry), I(X), I(Y), I(Width), I(Height)), \
		R((AckConfigure), U(Serial)),                            \
		E((Configure), U(Serial)))

typedef enum xdg_surface_error : u32 {
	XDG_SURFACE_ERROR_NOT_CONSTRUCTED	  = 1,
	XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED = 2,
	XDG_SURFACE_ERROR_UNCONFIGURED_BUFFER = 3,
	XDG_SURFACE_ERROR_INVALID_SERIAL	  = 4,
	XDG_SURFACE_ERROR_INVALID_SIZE		  = 5,
	XDG_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 6,
} xdg_surface_error;

#define XDG_TOPLEVEL                                                  \
	((XdgToplevel, xdg_toplevel, 7),                                  \
		D((Destroy)),                                                 \
		R((SetParent), O(Parent, , TRUE)),                            \
		R((SetTitle), S(Title)),                                      \
		R((SetAppId), S(AppId)),                                      \
		R((ShowWindowMenu), O(Seat, WL_SEAT), U(Serial), I(X), I(Y)), \
		R((Move), O(Seat, WL_SEAT), U(Serial)),                       \
		R((Resize),                                                   \
			O(Seat, WL_SEAT),                                         \
			U(Serial),                                                \
			U(Edges, xdg_toplevel_resize_edge)),                      \
		R((SetMaxSize), I(Width), I(Height)),                         \
		R((SetMinSize), I(Width), I(Height)),                         \
		R((SetMaximized)),                                            \
		R((UnsetMaximized)),                                          \
		R((SetFullscreen), O(Output, WL_OUTPUT, TRUE)),               \
		R((UnsetFullscreen)),                                         \
		R((SetMinimized)),                                            \
		E((Configure), I(Width), I(Height), A(States)),               \
		E((Close)),                                                   \
		E((ConfigureBounds, 4), I(Width), I(Height)),                 \
		E((WmCapabilities, 5), A(Capabilities)))

typedef enum xdg_toplevel_error : u32 {
	XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE = 0,
	XDG_TOPLEVEL_ERROR_INVALID_PARENT	   = 1,
	XDG_TOPLEVEL_ERROR_INVALID_SIZE		   = 2,
} xdg_toplevel_error;

typedef enum xdg_toplevel_resize_edge : u32 {
	XDG_TOPLEVEL_RESIZE_EDGE_NONE		  = 0,
	XDG_TOPLEVEL_RESIZE_EDGE_TOP		  = 1,
	XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM		  = 2,
	XDG_TOPLEVEL_RESIZE_EDGE_LEFT		  = 4,
	XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT	  = 5,
	XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT  = 6,
	XDG_TOPLEVEL_RESIZE_EDGE_RIGHT		  = 8,
	XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT	  = 9,
	XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT = 10,
} xdg_toplevel_resize_edge;

typedef enum xdg_toplevel_state : u32 {
	XDG_TOPLEVEL_STATE_MAXIMIZED			   = 1,
	XDG_TOPLEVEL_STATE_FULLSCREEN			   = 2,
	XDG_TOPLEVEL_STATE_RESIZING				   = 3,
	XDG_TOPLEVEL_STATE_ACTIVATED			   = 4,
	XDG_TOPLEVEL_STATE_TILED_LEFTSINCE		   = 5,	  // v2
	XDG_TOPLEVEL_STATE_TILED_RIGHTSINCE		   = 6,	  // v2
	XDG_TOPLEVEL_STATE_TILED_TOPSINCE		   = 7,	  // v2
	XDG_TOPLEVEL_STATE_TILED_BOTTOMSINCE	   = 8,	  // v2
	XDG_TOPLEVEL_STATE_SUSPENDEDSINCE		   = 9,	  // v6
	XDG_TOPLEVEL_STATE_CONSTRAINED_LEFTSINCE   = 10,  // v7
	XDG_TOPLEVEL_STATE_CONSTRAINED_RIGHTSINCE  = 11,  // v7
	XDG_TOPLEVEL_STATE_CONSTRAINED_TOPSINCE	   = 12,  // v7
	XDG_TOPLEVEL_STATE_CONSTRAINED_BOTTOMSINCE = 13,  // v7
} xdg_toplevel_state;

typedef enum xdg_toplevel_wm_capabilities : u32 {
	XDG_TOPLEVEL_WM_CAPABILITIES_WINDOW_MENU = 1,
	XDG_TOPLEVEL_WM_CAPABILITIES_MAXIMIZE	 = 2,
	XDG_TOPLEVEL_WM_CAPABILITIES_FULLSCREEN	 = 3,
	XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE	 = 4,
} xdg_toplevel_wm_capabilities;

#define XDG_POPUP                                                    \
	((XdgPopup, xdg_popup, 7),                                       \
		D((Destroy)),                                                \
		R((Grab), O(Seat, WL_SEAT), U(Serial)),                      \
		R((Reposition, 3), O(Positioner, XDG_POSITIONER), U(Token)), \
		E((Configure), I(X), I(Y), I(Width), I(Height)),             \
		E((PopupDone)),                                              \
		E((Repositioned, 3), U(Token)))

typedef enum xdg_popup_error : u32 {
	XDG_POPUP_ERROR_INVALID_GRAB = 0,
} xdg_popup_error;

WLI_DEFINE(XDG_WM_BASE)
WLI_DEFINE(XDG_POSITIONER)
WLI_DEFINE(XDG_SURFACE)
WLI_DEFINE(XDG_TOPLEVEL)
WLI_DEFINE(XDG_POPUP)
