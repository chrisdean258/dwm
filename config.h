/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
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
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_black, col_gray2 },
	[SchemeSel]  = { col_gray4, col_black,  col_cyan  },
};

/* Custom function dcls */
void restart(const Arg * arg);
void focustagmon(const Arg * arg);
void movetotag(const Arg * arg);
void newtag(const Arg * arg);
void newtagspawn(const Arg * arg);
void zoom_or_open(const Arg * arg);
void focusmon_int(Monitor * m);
void mylayout(Monitor * m);
void handle_st(const Arg * arg);
void handle_browser(const Arg * arg);
void spawn_and_open(const char * name, const Arg * command);
Client * find_client_by_name(const char * name);


/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class               instance    title       tags mask     isfloating   monitor */
	{ "Chromium-browser",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "st-256color",       NULL,       NULL,       1 << 7,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[0]",      mylayout }, /* first entry is default */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/bash", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Return, handle_st,      {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY|ControlMask,           XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_Escape, killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_s,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  focustagmon,    {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, focustagmon,    {.i = +1 } },
	{ MODKEY|ControlMask,           XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_g,      handle_browser, SHCMD("browser") },
	{ MODKEY,                       XK_o,      spawn,          SHCMD("monitor detect") },
	{ MODKEY,                       XK_r,      restart,        {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY,                       XK_v,      spawn,          SHCMD("vol -5%") },
	{ MODKEY|ShiftMask,             XK_v,      spawn,          SHCMD("vol +5%") },
	{ MODKEY,                       XK_a,      spawn,          SHCMD("dmenu_audio") },
	{ MODKEY|ShiftMask,             XK_b,      spawn,          SHCMD("dmenu_backlight") },
	{ MODKEY|ShiftMask,             XK_u,      spawn,          SHCMD("backlight +10") },
	{ MODKEY,                       XK_u,      spawn,          SHCMD("backlight -10") },
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

unsigned int freetag(int force)
{
	Client * i;
	unsigned int mask;

	mask = 0;

	for(i = selmon->clients; i; i = i->next)
	{
		if(force || i != selmon->sel) mask |= i->tags;
	}

	/* Dat bithack tho */
	return (mask + 1) & ~mask;
}

Client * find_client_by_name(const char * name)
{
	Client * c;
	for(c = selmon->clients; c && !strstr(c->name, name); c = c->next);
	return c;
}

void handle_browser(const Arg* arg)
{
	spawn_and_open("hrom", arg);
}

void handle_st(const Arg* arg)
{
	spawn_and_open("st", arg);
}

void spawn_and_open(const char * name, const Arg * command)
{
	Client * c;
	const Rule * r;
	Arg a;
	int i;
	a.ui = 0;
	c = find_client_by_name(name);
	if(!c)
	{
		spawn(command);
		for (i = 0; i < LENGTH(rules); i++) {
			r = &rules[i];
			if (r->class && strstr(r->class, name)) a.ui = r->tags;
		}
	}
	else a.ui = c->tags;
	toggleview(&a);
}

void restart(const Arg * arg)
{
	execvp("dwm", (char * const[]) { "dwm", NULL } );
}

void focustagmon(const Arg * arg)
{
	tagmon(arg);
	focusmon(arg);
}

void movetotag(const Arg * arg)
{
	tag(arg);
	view(arg);
}


void newtag(const Arg * arg)
{
	Arg a;
	a.ui = freetag(0);
	movetotag(&a);
}

void newtagspawn(const Arg * arg)
{
	Arg a;
	a.ui = freetag(1);
	view(&a);
	spawn(arg);
}

void mylayout(Monitor * m)
{
	if(m == selmon) monocle(m);
	else tile(m);
}
