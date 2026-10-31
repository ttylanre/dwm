/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;   /* border pixel of windows */
static const unsigned int snap      = 32;  /* snap pixel */
static const int showbar            = 1;   /* 0 means no bar */
static const int topbar             = 1;   /* 0 means bottom bar */
static const int vertpad            = 5;   /* vertical padding of bar */
static const int sidepad            = 6;   /* horizontal padding of bar */
static const int horizpadbar        = 6;   /* horizontal padding for statusbar */
static const int vertpadbar         = 6;   /* vertical padding for statusbar */
static const int swallowfloating    = 0;   /* 1 means swallow floating windows by default */
static const unsigned int gappih    = 9;   /* horiz inner gap between windows */
static const unsigned int gappiv    = 9;   /* vert inner gap between windows */
static const unsigned int gappoh    = 10;  /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;  /* vert outer gap between windows and screen edge */
static int smartgaps                = 0;   /* 1 means no outer gap when there is only one window */

static const char *fonts[]          = { "JetBrainsMono Nerd Font:size=12" };
static const char dmenufont[]       = "JetBrainsMono Nerd Font:size=12";

/* Catppuccin Mocha */
static const char col_base[]     = "#1e1e2e"; /* bar / inactive background */
static const char col_surface1[] = "#45475a"; /* inactive border */
static const char col_text[]     = "#cdd6f4"; /* normal text */
static const char col_blue[]     = "#89b4fa"; /* selected background */
static const char col_lavender[] = "#b4befe"; /* selected border */

#define baralpha    0x33
#define borderalpha OPAQUE

static const char *colors[][3] = {
	/*               fg           bg           border       */
	[SchemeNorm] = { col_text,    col_base,    col_surface1 },
	[SchemeSel]  = { col_base,    col_blue,    col_lavender },
};

static const unsigned int alphas[][3] = {
	/*               fg      bg        border      */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };

static const unsigned int ulinepad      = 5; /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke   = 2; /* thickness / height of the underline */
static const unsigned int ulinevoffset  = 0; /* how far above the bottom of the bar the line should appear */
static const int ulineall               = 0; /* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance  title          tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",     NULL,     NULL,          0,         1,          0,          0,         -1 },
	{ "Firefox",  NULL,     NULL,          1 << 8,    0,          0,         -1,         -1 },
	{ "kitty",    NULL,     NULL,          0,         0,          1,          0,         -1 },
	{ NULL,       NULL,     "Event Tester", 0,        0,          0,          1,         -1 }, /* xev */
};

/* layout(s) */
static const float mfact        = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster        = 1;    /* number of clients in master area */
static const int resizehints    = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1;    /* 1 will force focus on the fullscreen window */
static const int refreshrate    = 120;  /* refresh rate for client move/resize */

#define FORCE_VSPLIT 1 /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },                  /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },                  /* no layout function means floating behavior */
	{ NULL,       NULL },
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
static const char *dmenucmd[] = {
	"dmenu_run", "-i", "-m", dmenumon, "-fn", dmenufont, NULL
};
/* static const char *dmenucmd[] = {
	"dmenu_run", "-m", dmenumon, "-fn", dmenufont,
	"-nb", col_base, "-nf", col_text,
	"-sb", col_blue, "-sf", col_base,
	NULL
}; */
static const char *appmenucmd[] = { "j4-dmenu-desktop", "--dmenu=dmenu -i", NULL };
static const char *termcmd[]        = { "kitty", NULL };
static const char *browsercmd[]     = { "firefox", NULL };
static const char *fmcmd[]          = { "pcmanfm", NULL };
static const char *screenshotcmd[]  = { "/home/lanre/.local/bin/take-screenshot", NULL };
static const char *selectshotcmd[]  = { "/home/lanre/.local/bin/take-screenshot", "select", NULL };
static const char *clipmenucmd[]    = { "clipmenu", NULL };
static const char *powermenucmd[]   = { "powermenu", NULL };
static const char *lockcmd[]        = { "betterlockscreen", "-l", NULL };
static const char *colorpickcmd[]   = { "sh", "-c", "xcolor | xclip -selection clipboard", NULL };

#include "movestack.c"

static const Key keys[] = {
	/* modifier                     key            function        argument */

	/* launchers */
    { MODKEY,                       XK_d,          spawn,          {.v = appmenucmd } },
    { MODKEY|ShiftMask,             XK_d,          spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,     spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,          spawn,          {.v = browsercmd } },
	{ MODKEY,                       XK_e,          spawn,          {.v = fmcmd } },
	{ MODKEY,                       XK_v,          spawn,          {.v = clipmenucmd } },
	{ MODKEY,                       XK_x,          spawn,          {.v = colorpickcmd } },
	{ MODKEY,                       XK_w,          spawn,          SHCMD("wallpick") },
	{ MODKEY,                       XK_F1,         spawn,          {.v = lockcmd } },
	{ MODKEY,                       XK_Print,      spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,             XK_Print,      spawn,          {.v = selectshotcmd } },

	/* bar */
	{ MODKEY|ShiftMask,             XK_b,          togglebar,      {0} },

	/* focus / stack */
	{ MODKEY,                       XK_j,          focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,          focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,          movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,          movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Return,     zoom,           {0} },

	/* master area / client factor */
    { MODKEY,                       XK_i,          incnmaster,   {.i = +1 } },
    { MODKEY|ShiftMask,             XK_i,          incnmaster,   {.i = -1 } },
	{ MODKEY,                       XK_h,          setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,          setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,          setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,          setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,          setcfact,       {.f =  0.00} },

	/* gaps */
	{ MODKEY|Mod1Mask,              XK_u,          incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_u,          incrgaps,       {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_i,          incrigaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,          incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,          incrogaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,          incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,          incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,          incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,          incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,          incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,          incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,          incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,          incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,          incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_0,          togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,          defaultgaps,    {0} },

	/* windows / layouts */
	{ MODKEY,                       XK_Tab,        view,           {0} },
	{ MODKEY,                       XK_q,          killclient,     {0} },
	{ MODKEY,                       XK_t,          setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,          setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,          setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ControlMask,           XK_comma,      cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period,     cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,      setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,      togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,          togglefullscr,  {0} },

	{ 0, XF86XK_AudioRaiseVolume,                  spawn,          SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 5%+") },
    { 0, XF86XK_AudioLowerVolume,                  spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-") },
    { 0, XF86XK_AudioMute,                         spawn,          SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle") },
    { 0, XF86XK_MonBrightnessUp,                   spawn,          SHCMD("brightnessctl set 5%+") },
    { 0, XF86XK_MonBrightnessDown,                 spawn,          SHCMD("brightnessctl set 5%-") },

	/* tags / monitors */
	{ MODKEY,                       XK_0,          view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,          tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,      focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,     focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,      tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,     tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                          0)
	TAGKEYS(                        XK_2,                          1)
	TAGKEYS(                        XK_3,                          2)
	TAGKEYS(                        XK_4,                          3)
	TAGKEYS(                        XK_5,                          4)
	TAGKEYS(                        XK_6,                          5)
	TAGKEYS(                        XK_7,                          6)
	TAGKEYS(                        XK_8,                          7)
	TAGKEYS(                        XK_9,                          8)

	/* session */
	{ MODKEY|ShiftMask,             XK_q,          spawn,          {.v = powermenucmd } },
	{ MODKEY|ControlMask|ShiftMask, XK_q,          quit,           {1} }, /* restart dwm */
	/* { MODKEY|ShiftMask,          XK_q,          quit,           {0} }, */
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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
