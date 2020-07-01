/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "fibonacci.c"

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = {"FiraCode Nerd Font:style=Retina,Regular:pixelsize=12:antialias=true:autohint=true"};
static const char dmenufont[]       = "FiraCode Nerd Font:style=Retina,Regular:pixelsize=12:antialias=true:autohint=true";
static const char col_NormFG[]      = "#FEFEFE";//"#282a36";#222222
static const char col_NormBG[]      = "#181617";//"#444444";
static const char col_SelFG[]       = "#ad1b16";//"#E4312C";//"#e5e9f0";
static const char col_SelBG[]       = "#181617";//"#eeeeee"
static const char col_Border[]      = "#3D5A60";//"#005577"
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_NormFG, col_NormBG, col_Border },
	[SchemeSel]  = { col_SelFG, col_SelBG,  col_Border  },
	[SchemeTitle]  = { col_NormFG, col_NormBG,  col_Border  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* grid of tags */
#define DRAWCLASSICTAGS             1 << 0
#define DRAWTAGGRID                 1 << 1

#define SWITCHTAG_UP                1 << 0
#define SWITCHTAG_DOWN              1 << 1
#define SWITCHTAG_LEFT              1 << 2
#define SWITCHTAG_RIGHT             1 << 3
#define SWITCHTAG_TOGGLETAG         1 << 4
#define SWITCHTAG_TAG               1 << 5
#define SWITCHTAG_VIEW              1 << 6
#define SWITCHTAG_TOGGLEVIEW        1 << 7

static const unsigned int drawtagmask = DRAWTAGGRID; /* | DRAWCLASSICTAGS to show classic row of tags */
static const int tagrows = 2;

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class				instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "gimp",				NULL,     NULL,           0,			1,          0,           0,        -1 },
	{ "gpick",				NULL,     NULL,           0,			1,          0,           0,        -1 },
	{ "firefox",			NULL,     NULL,           0,			0,          0,          -1,        -1 },
	{ "gnome-calculator",	NULL,     NULL,           0,			1,          0,          -1,        -1 },
	{ "st",					NULL,     NULL,           0,			0,          1,          -1,        -1 },
	{ "ranger",				NULL,     NULL,           0,			0,          1,          -1,        -1 },
	{ NULL,					NULL,     "Event Tester", 0,			1,          0,           1,        -1 }, /* xev */
 
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ " ﬿ ",      tile },    /* first entry is default */
	{ "  ",      NULL },    /* no layout function means floating behavior */
	{ "  ",      monocle },
	{ " 𤋮",      centeredmaster },
	{ "~𤋮",      centeredfloatingmaster },
	{ "[@]",      spiral },
 	{ "[\\]",     dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-p", "Run : ", "-l", "10", "-m", dmenumon, "-fn", dmenufont, \
	"-nb", col_NormBG, "-nf", col_NormFG, "-sb", col_SelFG, "-sf", col_SelBG, NULL };
static const char *termcmd[]  = { "st", "-t", " Terminal", NULL };
static const char scratchpadname[] = " Scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", "-e", "vim", NULL };
static const char *volToggle[] = { "volctl", "toggle", NULL };
static const char *volUp[] = { "volctl", "up", NULL};
static const char *volDown[] = { "volctl", "down", NULL};
static const char *brightnessUp[] = { "britectl", "up", NULL };
static const char *brightnessDown[] = { "britectl", "down", NULL };
static const char *trackToggle[] = { "playerctl", "toggle", NULL };
static const char *trackNxt[] = { "playerctl", "next", NULL };
static const char *trackPrev[] = { "playerctl", "prev", NULL };
static const char *browser[] = { "runBrowser", NULL };
static const char *fullSS[] = { "Screenshot", "full", NULL };
static const char *selectiveSS[] = { "Screenshot", "", NULL };
static const char *emojiMenu[] = { "emojiMenu", NULL };
static const char *ytDownloader[] = { "YTDownloader", NULL };
static const char *exitOps[] = { "exitOps", NULL };
static const char *resMonitor[] = { "resourcemon", NULL };

static Key keys[] = {
	/* modifier             key							function        argument */
	{ MODKEY,               XK_r,						spawn,          {.v = dmenucmd } },
	{ MODKEY,				XK_Return,					spawn,          {.v = termcmd } },
	{ 0,                    XF86XK_AudioMute,           spawn,          {.v = volToggle} },
	{ 0,                    XF86XK_AudioLowerVolume,    spawn,          {.v = volDown } },
	{ 0,                    XF86XK_AudioRaiseVolume,    spawn,          {.v = volUp } },
	{ 0,                    XF86XK_MonBrightnessUp,     spawn,          {.v = brightnessUp } },
	{ 0,                    XF86XK_MonBrightnessDown,   spawn,          {.v = brightnessDown } },
	{ 0,                    XF86XK_AudioPlay,           spawn,          {.v = trackToggle } },
	{ 0,                    XF86XK_AudioNext,           spawn,          {.v = trackNxt } },
	{ 0,                    XF86XK_AudioPrev,           spawn,          {.v = trackPrev } },
	{ Mod1Mask,             XK_b,                       spawn,          {.v = browser } },
	{ Mod1Mask,             XK_s,                       spawn,          {.v = fullSS } },
	{ Mod1Mask,             XK_x,                       spawn,          {.v = selectiveSS } },
	{ Mod1Mask,             XK_e,                       spawn,          {.v = emojiMenu } },
	{ Mod1Mask,             XK_d,                       spawn,          {.v = ytDownloader } },
	{ Mod1Mask,             XK_r,                       spawn,          {.v = resMonitor } },
	{ MODKEY,               XK_l,                       spawn,          {.v = exitOps } },
	{ MODKEY,               XK_grave,					togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,               XK_b,						togglebar,      {0} },
	{ MODKEY,               XK_s,						togglesticky,   {0} },
	{ MODKEY|ShiftMask,     XK_f,						togglefullscr,  {0} },
	{ MODKEY|ShiftMask,     XK_space,					togglefloating, {0} },
	{ MODKEY,				XK_j,						focusstack,     {.i = +1 } },
	{ MODKEY,				XK_k,						focusstack,     {.i = -1 } },
	{ MODKEY,               XK_u,						incnmaster,     {.i = -1 } },
	{ MODKEY,               XK_i,						incnmaster,     {.i = +1 } },
	{ MODKEY,               XK_y,						setmfact,       {.f = -0.05} },
	{ MODKEY,               XK_o,						setmfact,       {.f = +0.05} },
	{ Mod1Mask,				XK_Return,					zoom,           {0} },
	{ MODKEY,               XK_Tab,						view,           {0} },
	{ MODKEY,				XK_q,						killclient,     {0} },
	{ MODKEY,               XK_t,						setlayout,      {.v = &layouts[0]} },
	{ MODKEY,               XK_f,						setlayout,      {.v = &layouts[1]} },
	{ MODKEY,               XK_m,						setlayout,      {.v = &layouts[2]} },
	{ MODKEY,               XK_c,						setlayout,      {.v = &layouts[3]} },
	{ MODKEY,               XK_v,						setlayout,      {.v = &layouts[4]} },
	{ MODKEY,               XK_g,						setlayout,      {.v = &layouts[5]} },
	{ MODKEY,				XK_w,						setlayout,      {.v = &layouts[6]} }, 
	{ MODKEY,               XK_space,					setlayout,      {0} },
	{ MODKEY,               XK_0,						view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,     XK_0,						tag,            {.ui = ~0 } },
	{ MODKEY,               XK_comma,					focusmon,       {.i = -1 } },
	{ MODKEY,               XK_period,					focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_comma,					tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,     XK_period,					tagmon,         {.i = +1 } },
	{ MODKEY,               XK_minus,					setgaps,        {.i = -1 } },
	{ MODKEY,               XK_equal,					setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_equal,					setgaps,        {.i = 0  } },
	TAGKEYS(                XK_1,										0)
	TAGKEYS(                XK_2,										1)
	TAGKEYS(                XK_3,										2)
	TAGKEYS(                XK_4,										3)
	TAGKEYS(                XK_5,										4)
	TAGKEYS(                XK_6,										5)
	TAGKEYS(                XK_7,										6)
	TAGKEYS(                XK_8,										7)
	TAGKEYS(                XK_9,										8)
	{ MODKEY|ShiftMask,     XK_q,						quit,           {0} },

    { MODKEY,				XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_VIEW } },
    { MODKEY,				XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_VIEW } },
    { MODKEY,				XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_VIEW } },
    { MODKEY,				XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_VIEW } },

    { MODKEY|ControlMask,   XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,   XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,   XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,   XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

