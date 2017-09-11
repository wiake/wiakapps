quote="25April2009"

"wiak" is actually a team of six programmers at wiak organisation (EDIT: website currently no longer active online); so that identity is generally used in threads concerned with programming work in wiak to try and keep the general and the technical separated.
...

One of the 'products' of wiak team was called 'wiak', written in C in 2008, which:

"provides IPC (InterProcessCommunication) facilities (such as SysV message-passing queues) . . . to languages that don't have easy, flexible or any access to them (such as bash script). It also works well with the likes of gtkdialog to create event driven GUI or commandline user interfaces. . . . It is intended that wiak will eventually provide UNIX and INET socket capability
. . . [wiak] facilitates the creation of different clients for the same backend server(s), or indeed, different backend server(s) for the same GUI-client."

The wiak IPC program binary was also used in foksyfeyer dotpet addon for visually-impaired (or totally blind) users of Puppy linux in 2009.

# wiakapps

 Welcome to the Home of the WIAK Suite of Applications

    WIAK(TM) is:

    A family of programmers

    The magicians behind the multiple personalities of the modular wiakapps

    A philosophy and methodology for programming which simplifies User Interface code

    A shell for Inter-Process Communications which WIAK(S) --UP the sleeping components of a wiakapp(TM)

    A versatile and super-lightweight building block which works well, with gtkdialog or C or . . . , even from the commandline


WIAK Record, shown below, is one of wiak's progeny.

Click any of its buttons to see and find downloads for the rest of the family

First published: Dec 2007

What Is WIAK?
Wiak is a simple, limited, but very efficient, special-purpose, usually non-interactive, extremely lightweight IPC shell written in C.

WIAK stands for, the "WIAK Interface für Application Kontrolle". As an Inter-Process Communications utility, its name reflects its ability to create and control diverse wiakapps whose component parts may be written in a variety of programming languages and with an assortment of GUI toolkits.

Like most any program, wiak could be started up as the first main system application, after "init", on boot. However, wiak is more particularly of the family "shell" in that it can be used to start up (and in some cases control) other processes (or indeed other shells), whilst providing one or more IPC facilities for communicating between the running processes.

Compatible pipe-using commandline utilities and applications can be chained together, and called up and controlled, via wiak and a wiak compatible backend server, to produce a WIAK Application or "wiakapp".

In effect, wiak is thus a simple IPC subsystem which can be used to dynamically build a complex application by selection from a pool of individually functional parts. The resulting wiak-created-application, or wiakapp for short, is itself dynamic and temporary in shape and form, since wiak does not actually glue wiakapp component parts together. Instead, it creates through its message passing mechanism an illusion of virtual glue for what are otherwise unconnected processes.

Wiak encourages a client/server approach to programming, where, for example, the GUI component of the wiakapp can act as a client, sending simple commands and other data, via wiak, to a server application which itself might be a wrapper to a specific function or group of functions (e.g. GNU or UNIX commandline utilities). 
