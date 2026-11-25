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

wayland_prototype Wayland_XdgWmBase_Prototype = {
	.Name		  = "xdg_wm_base",
	.Version	  = 7,
	.RequestCount = 4,
	.EventCount	  = 1,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "create_positioner",
				"on",
				P{ &Wayland_XdgPositioner_Prototype } },
		  { "get_xdg_surface", "ono", P{ &Wayland_XdgSurface_Prototype } },
		  { "pong", "ou", NULL },
		  },
	.Events =
		M{
		  { "ping", "ou", NULL },
		  },
};

enum wayland_xdg_wm_base_error {
	WAYLAND_XDG_WM_BASE_ERROR_ROLE					= 0,
	WAYLAND_XDG_WM_BASE_ERROR_DEFUNCT_SURFACES		= 1,
	WAYLAND_XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP = 2,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT	= 3,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_SURFACE_STATE = 4,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_POSITIONER	= 5,
	WAYLAND_XDG_WM_BASE_ERROR_UNRESPONSIVE			= 6,
};

wayland_prototype Wayland_XdgPositioner_Prototype = {
	.Name		  = "xdg_positioner",
	.Version	  = 7,
	.RequestCount = 10,
	.EventCount	  = 0,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "set_size", "oii", NULL },
		  { "set_anchor_rect", "oiiii", NULL },
		  { "set_anchor", "ou", NULL },
		  { "set_gravity", "ou", NULL },
		  { "set_constraint_adjustment", "ou", NULL },
		  { "set_offset", "oii", NULL },
		  { "set_reactive", "3o", NULL },
		  { "set_parent_size", "3oii", NULL },
		  { "set_parent_configure", "3ou", NULL },
		  },
	.Events = NULL,
};

enum wayland_xdg_positioner_anchor {
	WAYLAND_XDG_POSITIONER_ANCHOR_NONE		   = 0,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP		   = 1,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM	   = 2,
	WAYLAND_XDG_POSITIONER_ANCHOR_LEFT		   = 3,
	WAYLAND_XDG_POSITIONER_ANCHOR_RIGHT		   = 4,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP_LEFT	   = 5,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM_LEFT  = 6,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP_RIGHT	   = 7,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT = 8,
};

enum wayland_xdg_positioner_constraint_adjustment {
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE	  = 0,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X  = 1,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y  = 2,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X	  = 4,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y	  = 8,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X = 16,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y = 32,
};

enum wayland_xdg_positioner_error {
	WAYLAND_XDG_POSITIONER_ERROR_INVALID_INPUT = 0
};

enum wayland_xdg_positioner_gravity {
	WAYLAND_XDG_POSITIONER_GRAVITY_NONE			= 0,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP			= 1,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM		= 2,
	WAYLAND_XDG_POSITIONER_GRAVITY_LEFT			= 3,
	WAYLAND_XDG_POSITIONER_GRAVITY_RIGHT		= 4,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP_LEFT		= 5,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM_LEFT	= 6,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP_RIGHT	= 7,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT = 8,
};

wayland_prototype Wayland_XdgSurface_Prototype = {
	.Name		  = "xdg_surface",
	.Version	  = 7,
	.RequestCount = 5,
	.EventCount	  = 1,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "get_toplevel", "on", P{ &Wayland_XdgToplevel_Prototype } },
		  { "get_popup", "ono?o", P{ &Wayland_XdgPopup_Prototype } },
		  { "set_window_geometry", "oiiii", NULL },
		  { "ack_configure", "ou", NULL },
		  },
	.Events =
		M{
		  { "configure", "ou", NULL },
		  },
};

enum wayland_xdg_surface_error {
	WAYLAND_XDG_SURFACE_ERROR_NOT_CONSTRUCTED	  = 1,
	WAYLAND_XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED = 2,
	WAYLAND_XDG_SURFACE_ERROR_UNCONFIGURED_BUFFER = 3,
	WAYLAND_XDG_SURFACE_ERROR_INVALID_SERIAL	  = 4,
	WAYLAND_XDG_SURFACE_ERROR_INVALID_SIZE		  = 5,
	WAYLAND_XDG_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 6,
};

wayland_prototype Wayland_XdgToplevel_Prototype = {
	.Name		  = "xdg_toplevel",
	.Version	  = 7,
	.RequestCount = 14,
	.EventCount	  = 4,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "set_parent", "oo?", NULL },
		  { "set_title", "os", NULL },
		  { "set_app_id", "os", NULL },
		  { "show_window_menu", "oouii", NULL },
		  { "move", "oou", NULL },
		  { "resize", "oouu", NULL },
		  { "set_max_size", "oii", NULL },
		  { "set_min_size", "oii", NULL },
		  { "set_maximized", "o", NULL },
		  { "unset_maximized", "o", NULL },
		  { "set_fullscreen", "oo?", NULL },
		  { "unset_fullscreen", "o", NULL },
		  { "set_minimized", "o", NULL },
		  },
	.Events =
		M{
		  { "configure", "oiia", NULL },
		  { "close", "o", NULL },
		  { "configure_bounds", "4oii", NULL },
		  { "wm_capabilities", "5oa", NULL },
		  },
};

enum wayland_xdg_toplevel_error {
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE = 0,
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_PARENT	   = 1,
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_SIZE		   = 2,
};

enum wayland_xdg_toplevel_resize_edge {
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_NONE		  = 0,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP		  = 1,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM		  = 2,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_LEFT		  = 4,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT	  = 5,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT  = 6,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_RIGHT		  = 8,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT	  = 9,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT = 10,
};

enum wayland_xdg_toplevel_state {
	WAYLAND_XDG_TOPLEVEL_STATE_MAXIMIZED			   = 1,
	WAYLAND_XDG_TOPLEVEL_STATE_FULLSCREEN			   = 2,
	WAYLAND_XDG_TOPLEVEL_STATE_RESIZING				   = 3,
	WAYLAND_XDG_TOPLEVEL_STATE_ACTIVATED			   = 4,
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_LEFTSINCE		   = 5,	  // v2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_RIGHTSINCE		   = 6,	  // v2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_TOPSINCE		   = 7,	  // v2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_BOTTOMSINCE	   = 8,	  // v2
	WAYLAND_XDG_TOPLEVEL_STATE_SUSPENDEDSINCE		   = 9,	  // v6
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_LEFTSINCE   = 10,  // v7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_RIGHTSINCE  = 11,  // v7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_TOPSINCE	   = 12,  // v7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_BOTTOMSINCE = 13,  // v7
};

enum wayland_xdg_toplevel_wm_capabilities {
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_WINDOW_MENU = 1,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_MAXIMIZE	 = 2,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_FULLSCREEN	 = 3,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE	 = 4,
};

wayland_prototype Wayland_XdgPopup_Prototype = {
	.Name		  = "xdg_popup",
	.Version	  = 7,
	.RequestCount = 3,
	.EventCount	  = 3,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "grab", "oou", NULL },
		  { "reposition", "3oou", NULL },
		  },
	.Events =
		M{
		  { "configure", "oiiii", NULL },
		  { "popup_done", "o", NULL },
		  { "repositioned", "3ou", NULL },
		  },
};

enum wayland_xdg_popup_error {
	WAYLAND_XDG_POPUP_ERROR_INVALID_GRAB = 0,
};

WLI_DEFINE(XDG_WM_BASE)
WLI_DEFINE(XDG_POSITIONER)
WLI_DEFINE(XDG_SURFACE)
WLI_DEFINE(XDG_TOPLEVEL)
WLI_DEFINE(XDG_POPUP)
