static unsigned int borderpx        = 4;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static unsigned int gappih          = 8;        /* horiz inner gap between windows */
static unsigned int gappiv          = 8;        /* vert inner gap between windows */
static unsigned int gappoh          = 8;        /* horiz outer gap between windows and screen edge */
static unsigned int gappov          = 8;        /* vert outer gap between windows and screen edge */
static unsigned int systraypinning  = 1;        /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static unsigned int systrayspacing  = 2;        /* systray spacing */
static int systraypinningfailfirst  = 1;        /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static unsigned int systrayiconsize = 32;       /* systray icon size in px */
static int showsystray              = 1;        /* 0 means no systray */
static int smartgaps                = 1;        /* 1 means no outer gap when there is only one window */
static int showbar                  = 1;        /* 0 means no bar */
static int topbar                   = 1;        /* 0 means bottom bar */
static int swallowfloating          = 1;        /* 1 means swallow floating windows by default */
static const int refreshrate        = 240;
static int iconsize                 = 32;
static int iconspacing              = 12;
static char font[]                  = { "JetBrainsMonoNerdFont:size=16:antialias=true"};
static char font2[]                 = { "NotoColorEmoji:size=14:antialias=true"};
static const char *fonts[]          = { font,font2 };
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};	/* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

#define TERMINAL "st"
#define TERMCLASS "St"
#define BROWSER "librewolf"

static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
    /* class                         instance       title               tags mask  isfloating  isterminal  noswallow  monitor  scratchkey*/
    { "mpv",                         NULL,          NULL,               0,         1,          0,          0,          1,       0   },
    { "KeePassXC",                   NULL,          NULL,               1 << 8,    0,          0,          0,          0,       0   },
    { "org.mozilla.Thunderbird",     NULL,          NULL,               1 << 2,    0,          0,          0,          0,       0   },
    { "qBittorrent",                 NULL,          NULL,               1 << 6,    0,          0,          0,          0,       0   },
    { "calibre",                     "calibre-gui", NULL,               1 << 3,    0,          0,          0,          0,       0   },
    { "discord",                     NULL,          NULL,               1 << 3,    0,          0,          0,          1,       0   },
    { TERMCLASS,                     NULL,          NULL,               0,         0,          1,          0,         -1,       0   },
    { "floatingTerm",                NULL,          NULL,               0,         1,          1,          0,         -1,       0   },
    { "Ghostscript",                 NULL,          NULL,               0,         0,          0,          1,         -1,       0   }, /* ghostscript */
    { NULL,                          NULL,          "Event Tester",     0,         0,          0,          1,         -1,       0   }, /* xev */
	{ NULL,                          NULL,          "spterm",           0,         1,          1,          1,         -1,      't'   },
	{ NULL,                          NULL,          "pulsemixer",       0,         1,          1,          1,         -1,      's'   },
	{ NULL,                          NULL,          "spnotes",          0,         1,          1,          1,         -1,      'n'   },
	{ NULL,                          NULL,          "spfiles",          0,         1,          1,          1,         -1,      'f'   },
	{ NULL,                          NULL,          "spsysmon",         0,         1,          1,          1,         -1,      'M'   },
	{ NULL,                          NULL,          "spmusic",          0,         1,          1,          1,         -1,      'm'   },
	{ NULL,                          NULL,          "sprss",            0,         1,          1,          1,         -1,      'r'   },
};


/* layout(s) */
static float mfact          = 0.5;  /* factor of master area size [0.05..0.95] */
static int nmaster          = 1;    /* number of clients in master area */
static int resizehints      = 1;    /* 1 means respect size hints in tiled resizals */
static int lockfullscreen   = 1;    /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    
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
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]        = { "dmenu_run", NULL };
static const char *termcmd[]         = { TERMINAL, NULL };
static const char *browser[]         = { BROWSER, NULL };
static const char *email[]           = { "thunderbird", NULL };
static const char *notes[]           = { TERMINAL,"-t","notes","-e","sh","-c","cd ~/dox/notes && $EDITOR", NULL};
static const char *fileManager[]     = { TERMINAL, "-e", "yazi", NULL };
static const char *guiFileManager[]  = { "pcmanfm-qt", NULL };
static const char *passwords[]       = { "keepassxc", NULL };
static const char *books[]           = { "calibre", NULL };
static const char *lockscreen[]      = { "slock", NULL };
static const char *communicator[]    = { "discord", NULL };

/* First arg only serves to match against key in rules*/
static const char *spterm[]     = {"t", TERMINAL, "-t", "spterm", NULL};
static const char *spsound[]    = {"s", TERMINAL, "-t", "pulsemixer","-e","pulsemixer", NULL};
static const char *spnotes[]    = {"n", TERMINAL, "-t", "spnotes","-e","sh","-c","cd ~/dox/notes && $EDITOR", NULL};
static const char *spfiles[]    = {"f", TERMINAL, "-t", "spfiles","-e","yazi", NULL};
static const char *spsysmon[]   = {"M", TERMINAL, "-t", "spsysmon","-e","btop", NULL};
static const char *spmusic[]    = {"m", TERMINAL, "-t", "spmusic","-e","rmpc", NULL};
static const char *sprss[]      = {"r", TERMINAL, "-t", "sprss","-e","newsboat", NULL};


/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
        { "normbgcolor",        STRING,  &normbgcolor },
        { "normbordercolor",    STRING,  &normbordercolor },
        { "normfgcolor",        STRING,  &normfgcolor },
        { "selbgcolor",         STRING,  &selbgcolor },
        { "selbordercolor",     STRING,  &selbordercolor },
        { "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};


static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ControlMask,           XK_Return, togglescratch,  {.v = spterm } },
	{ MODKEY|ControlMask,           XK_s,      togglescratch,  {.v = spsound } },
	{ MODKEY|ControlMask,           XK_n,      togglescratch,  {.v = spnotes } },
	{ MODKEY|ControlMask,           XK_f,      togglescratch,  {.v = spfiles } },
	{ MODKEY|ControlMask,           XK_m,      togglescratch,  {.v = spsysmon } },
	{ MODKEY,                       XK_m,      togglescratch,  {.v = spmusic } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_z,      incrgaps,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_z,      incrgaps,       {.i = -1 } },
	{ MODKEY,                       XK_g,      togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_g,      defaultgaps,    {0} },
    { MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_v,      setlayout,      {.v = &layouts[13]} },
	{ MODKEY|ControlMask,		    XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_c,      movecenter,     {0} },
	{ MODKEY,                       XK_s,      togglesticky,   {0} },
    { 0,                            XF86XK_AudioMute,          spawn,   SHCMD("wpctl  set-mute   @DEFAULT_AUDIO_SINK@ toggle && pkill -RTMIN+9 dwmblocks" ) },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,   SHCMD("wpctl  set-volume @DEFAULT_AUDIO_SINK@ 5%- && pkill -RTMIN+9 dwmblocks && pkill -RTMIN+9 dwmblocks" ) },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,   SHCMD("wpctl  set-volume @DEFAULT_AUDIO_SINK@ 5%+ && pkill -RTMIN+9 dwmblocks && pkill -RTMIN+9 dwmblocks" ) },
    { 0,                            XF86XK_AudioPlay,          spawn,   SHCMD("playerctl play-pause") },
    { 0,                            XF86XK_AudioNext,          spawn,   SHCMD("playerctl next") },
    { 0,                            XF86XK_AudioPrev,          spawn,   SHCMD("playerctl previous") },
    { MODKEY,                       XK_Escape, spawn,          {.v = lockscreen } },
    { MODKEY|ShiftMask,             XK_Escape, spawn,          SHCMD("powermenu")},
    { MODKEY,                       XK_w,      spawn,          {.v = browser } },
    { MODKEY,                       XK_e,      spawn,          {.v = email } },
    { MODKEY,                       XK_n,      spawn,          {.v = notes } },
    { MODKEY|ShiftMask,             XK_n,      togglescratch,  {.v = sprss }},
    { MODKEY,                       XK_f,      spawn,          {.v = fileManager } },
    { MODKEY|Mod1Mask,              XK_f,      spawn,          {.v = guiFileManager } },
    { MODKEY,                       XK_p,      spawn,          {.v = passwords } },
    { MODKEY|ShiftMask,             XK_grave,  spawn,          SHCMD("dmenubookmarks select_browser") },
    { MODKEY,                       XK_grave,  spawn,          SHCMD("dmenubookmarks select") },
    { MODKEY|Mod1Mask,              XK_b,      spawn,          SHCMD("dmenubookmarks add") },
    { MODKEY|ShiftMask,             XK_b,      spawn,          {.v = books } },
    { MODKEY|ShiftMask,             XK_c,      spawn,          {.v = communicator } },
    { MODKEY|ShiftMask,             XK_p,      spawn,          SHCMD("colorpicker") },
    { MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("wallpaper open")},
    { MODKEY|ControlMask,           XK_w,      spawn,          SHCMD("wallpaper random") },
    { MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("screenshot") },
    { MODKEY|ShiftMask,             XK_m,      spawn,          SHCMD("mpvq play") },
    { MODKEY|ShiftMask,             XK_a,      spawn,          SHCMD("mpvq addclip") },
    { MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("dmenuunicode") },
    { MODKEY|ShiftMask|ControlMask, XK_m,      spawn,          SHCMD("boomer") },
    { 0,                            XK_Print,  spawn,          SHCMD("screenshot") },
    { MODKEY|ShiftMask|Mod1Mask,    XK_space,  spawn,          SHCMD("playerctl -p mpv play-pause") },
    { MODKEY|Mod1Mask,              XK_space,  spawn,          SHCMD("playerctl -p mpd play-pause") },
    { MODKEY|Mod1Mask,              XK_period, spawn,          SHCMD("playerctl -p mpd next") },
    { MODKEY|Mod1Mask,              XK_comma,  spawn,          SHCMD("playerctl -p mpd previous") },
    { MODKEY|Mod1Mask,              XK_equal,  spawn,          SHCMD("playerctl -p mpd volume 0.05+") },
    { MODKEY|Mod1Mask,              XK_minus,  spawn,          SHCMD("playerctl -p mpd volume 0.05-") },
    { MODKEY|Mod1Mask,              XK_s,      spawn,          SHCMD("playerctl -p mpd pause && playerctl -p mpd position 0") },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0}},
	{ MODKEY|ControlMask,           XK_r,      quit,           {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      { .v = &layouts[0] } },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      { .v = &layouts[13] } },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigstatusbar,   {.i = 6} },
	{ ClkStatusText,        ShiftMask,      Button2,        sigstatusbar,   {.i = 7} },
	{ ClkStatusText,        ShiftMask,      Button3,        sigstatusbar,   {.i = 8} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

