/* This files contains the extensions needed to turn dwm into vwm */
#include <time.h>


typedef enum Mode { Normal, Insert, Command } Mode;
Mode mode = Command;

void grabkeyboard()
{
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };
	int i;

	for(i = 0; i < 1000; i++)
	{
		if(XGrabKeyboard(dpy, root, True, GrabModeAsync,
				GrabModeAsync, CurrentTime) == GrabSuccess)
			return;
		nanosleep(&ts, NULL);
	}
	die("cannot grab keyboard");
}

void ungrabkeyboard()
{
	XUngrabKeyboard(dpy, CurrentTime);
	XFlush(dpy);
}

void grabkey(KeySym keysym)
{
	XGrabKey(dpy, XKeysymToKeycode(dpy, keysym), 0, root, True,
		GrabModeAsync, GrabModeAsync);
}

char * GetInputString()
{
	char * cmd = "echo -n | dmenu -b -p : -nb '#000' -nf '#FFF' -sf '#FFF' -sb '#000'";
	static char output[1024];
	FILE * proc;

	FILE * f = fopen("GetInputString", "w");
	fclose(f);

	ungrabkeyboard();
	proc = popen(cmd, "r");
	fgets(output, 1024, proc);
	output[MAX(strlen(output) - 1, 0)] = '\0';
	pclose(proc);
	grabkeyboard();

	return output;
}


void NormalMode()
{
	if(mode == Normal) return;

	mode = Normal;
	grabkeyboard();
}

void InsertMode()
{
	if(mode == Insert) return;

	mode = Insert;
	ungrabkeyboard();
	grabkey(XK_Escape);
	grabkeys();
}

void CommandMode()
{
	char * c;
	Arg a;

	if(mode == Command) return;
	mode = Command;
	c = GetInputString();
	if(strcmp(c, "q") == 0) running = 0;
	if(c[0] == '!')
	{
		a.v = (const char*[]){ "/bin/sh", "-c", c+1, NULL };
		spawn(&a);
	}
	NormalMode();
}
