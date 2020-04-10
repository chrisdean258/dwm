/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const int focusonwheel       = 0;        /* 0 no focus on wheel */
#ifndef HD
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
#else
static const char *fonts[]          = { "monospace:size=16" };
static const char dmenufont[]       = "monospace:size=16";
#endif
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_black[]       = "#000000";
static const char col_red[]         = "#ff0000";
static const char col_yellow[]      = "#ffff00";
static const char col_white[]       = "#ffffff";
static const char col_green[]       = "#4E9A06";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]   = { col_gray3, col_black, col_gray2 },
	[SchemeSel]    = { col_gray4, col_black,  col_cyan  },
	[SchemeWarn]   = { col_black, col_yellow, col_red },
	[SchemeUrgent] = { col_white, col_red,    col_red },
	[4]            = { col_red,   col_black, col_gray2  },
	[5]            = { col_green, col_black, col_gray2  },
};

typedef int search_func(const char *, const char *);

/* Custom function dcls */
void restart(const Arg * arg);
void setmfact_rel(const Arg * a);
void focustagmon(const Arg * arg);
void handle_st(const Arg * arg);
void handle_browser(const Arg * arg);
void spawn_and_open(const char * name, search_func func, const Arg * command, int tag);
Client * find_client_by_name(const char * name, search_func func, int tag);
int strin(const char * a, const char *b);
int streq(const char * a, const char *b);


/* tagging */
static const char *tags[] = { "1", "2" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class               instance    title       tags mask     isfloating   monitor */
	{ "hromsdfsd",         NULL,       NULL,       1 << 0,       0,           -1 }, /* Chrome */
	/* { "hrom",           NULL,       NULL,       1 << 8,       0,           -1 },
	{ "st-256color",       NULL,       NULL,       1 << 7,       0,           -1 },
	{ "Zathura",           NULL,       NULL,       1 << 6,       0,           -1 }, */
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
/* alt only to view  */
/* alt + ctrl to add or remove tag from view */
/* alt + shift to view a tag */
/* alt + ctrl + shift to toggle a tage from view */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key           function        argument */
	{ MODKEY,                       XK_p,         spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_semicolon, spawn,          SHCMD("dmenu_run") },
	{ MODKEY|ShiftMask,             XK_t,         spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Return,    handle_st,      {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return,    spawn,          {.v = termcmd } },
	{ MODKEY|ControlMask,           XK_Return,    zoom,           {0} },
	{ MODKEY,                       XK_b,         togglebar,      {0} },
	{ MODKEY,                       XK_j,         focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,         focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,         incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,         incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_y,         setmfact_rel,   {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_y,         setmfact_rel,   {.f = +0.05} },
	{ MODKEY,                       XK_Tab,       toggleview,     {.ui = ~0} },
	{ MODKEY|ShiftMask,             XK_Tab,       toggletag,      {.ui = ~0} },
	{ MODKEY,                       XK_Escape,    killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_c,         killclient,     {0} },
	{ MODKEY,                       XK_t,         setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,         setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,         setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,     setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,     togglefloating, {0} },
	{ MODKEY,                       XK_0,         view,           {.ui = 0 } },
	{ MODKEY|ControlMask,           XK_0,         toggleview,     {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,         tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,     focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,     focustagmon,    {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,    focustagmon,    {.i = +1 } },
	{ MODKEY|ControlMask,           XK_comma,     tagmon,         {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period,    tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_g,         handle_browser, SHCMD("browser") },
	{ MODKEY,                       XK_o,         spawn,          SHCMD("monitor") },
	{ MODKEY,                       XK_r,         restart,        {0} },
	TAGKEYS(                        XK_1,                         0)
	TAGKEYS(                        XK_2,                         1)
	TAGKEYS(                        XK_3,                         2)
	TAGKEYS(                        XK_4,                         3)
	TAGKEYS(                        XK_5,                         4)
	TAGKEYS(                        XK_6,                         5)
	TAGKEYS(                        XK_7,                         6)
	TAGKEYS(                        XK_8,                         7)
	TAGKEYS(                        XK_9,                         8)
	{ MODKEY|ShiftMask,             XK_q,         quit,           {0} },
	{ MODKEY,                       XK_v,         spawn,          SHCMD("vol -5%") },
	{ MODKEY|ShiftMask,             XK_v,         spawn,          SHCMD("vol +5%") },
	{ MODKEY,                       XK_a,         spawn,          SHCMD("dmenu_audio") },
	{ MODKEY|ShiftMask,             XK_a,         spawn,          SHCMD("audio") },
	{ MODKEY|ShiftMask,             XK_b,         spawn,          SHCMD("dmenu_backlight") },
	{ MODKEY|ShiftMask,             XK_u,         spawn,          SHCMD("backlight +10") },
	{ MODKEY,                       XK_u,         spawn,          SHCMD("backlight -10") },
	{ MODKEY,                       XK_F5,        spawn,          SHCMD("audio toggle") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button3,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
};

int streq(const char * a, const char * b) { return !strcmp(a, b); }
int strin(const char * a, const char *b) { return strstr(a, b) != NULL; }
void handle_browser(const Arg* arg) { spawn_and_open("hrom", strin, arg, 1<<8); }
void handle_st(const Arg* arg) { spawn_and_open("st", streq, arg, 1<<7); }

void setmfact_rel(const Arg * a)
{
	Arg b;
	Client * c;
	int i;

	if(!selmon || !selmon->sel) return;
	for (i = 0, c = nexttiled(selmon->clients); c && i < selmon->nmaster; c = nexttiled(c->next), i++)
	{
		if(selmon->sel == c)
		{
			setmfact(a);
			return;
		}
	}
	b.f = -a->f;
	setmfact(&b);
}

Client * find_client_by_name(const char * name, search_func func, int tag)
{
	Client * c;
	for(c = selmon->clients; c; c = c->next)
	{
		if(func(c->name, name) && (c->tags & tag)) break;
	}
	return c;
}

void spawn_and_open(const char * name, search_func func, const Arg * command, int tag)
{
	Client * c;
	Arg a;

	c = find_client_by_name(name, func, tag);
	if(!c) spawn(command);
	else
	{
		a.ui = c->tags;
		toggleview(&a);
		focus(c);
		if(selmon->lt[selmon->sellt] == &layouts[1]) zoom(&a);
	}
}

void restart(const Arg * arg)
{
	execvp("dwm", (char * const[]){"dwm", NULL});
}

void handle_restart(int dummy) {
	restart(NULL);
}

void focustagmon(const Arg * arg)
{
	tagmon(arg);
	focusmon(arg);
	applyrules(selmon->sel);
}

