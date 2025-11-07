static const char *colorname[NUMCOLS] = {
	[INIT] =   "black",     /* after initialization */
	[BACKGROUND] =   "black",     /* after initialization */
	[INPUT] =  "#005577",   /* during input */
	[FAILED] = "#CC3333",   /* wrong password */
	[PAM] =    "#9400D3",   /* waiting for PAM */
	[CAPS] = "red",         /* CapsLock on */
};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "selbgcolor",   STRING,  &colorname[INIT] },
		{ "color0",       STRING,  &colorname[BACKGROUND] },
		{ "color4",       STRING,  &colorname[INPUT] },
		{ "color1",       STRING,  &colorname[FAILED] },
		{ "color9",       STRING,  &colorname[CAPS] },
		{ "color5",       STRING,  &colorname[PAM] },
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;

/* time in seconds before the monitor shuts down */
static const int monitortime = 60;

/* PAM service that's used for authentication */
static const char* pam_service = "login";

/* insert grid pattern with scale 1:1, the size can be changed with logosize */
