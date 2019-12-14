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
	XGrabKey(dpy, XKeysymToKeycode(dpy, keysym), AnyModifier, root, True,
		GrabModeAsync, GrabModeAsync);
}

char * GetInputString()
{
	char * cmd = "cat ~/.bash_history | grep '....' | sort | uniq -c | sort -nr | cut -d " " -f 8- | dmenu -b -p : -nb '#000' -nf '#FFF' -sf '#FFF' -sb '#000'";
	cmd = "echo -n | dmenu -b -p : -nb '#000' -nf '#FFF' -sf '#FFF' -sb '#000'";
	static char output[1024];
	FILE * proc;

	ungrabkeyboard();
	proc = popen(cmd, "r");
	if(!proc || !fgets(output, 1024, proc))
	{
		perror("dmenu");
	}
	output[MAX(strlen(output) - 1, 0)] = '\0';
	pclose(proc);
	grabkeyboard();

	return output;
}


void NormalMode()
{
	if(mode == Normal) return;

	mode = Normal;
	XAutoRepeatOff(dpy);
	grabkeyboard();
}

void InsertMode()
{
	if(mode == Insert) return;

	mode = Insert;
	XAutoRepeatOn(dpy);
	ungrabkeyboard();
	grabkey(XK_F11);
}

void RunCommand(char * command)
{
	Arg a;
	char buff[1024];

	if(strcmp(command, "q") == 0) killclient(&a);
	else
	{
		snprintf(buff, 1024, "source $HOME/.bash_aliases; %s", command);
		a.v = (const char*[]){ "/bin/bash", "-c", buff, NULL };
		spawn(&a);
	}

}

void CommandMode()
{
	char * c;

	if(mode == Command) return;
	mode = Command;
	c = GetInputString();
	RunCommand(c);
	NormalMode();
}
