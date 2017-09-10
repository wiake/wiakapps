# wiakapps

First published: Dec 2007

What Is WIAK?
Wiak is a simple, limited, but very efficient, special-purpose, usually non-interactive, extremely lightweight IPC shell written in C.

WIAK stands for, the "WIAK Interface für Application Kontrolle". As an Inter-Process Communications utility, its name reflects its ability to create and control diverse wiakapps whose component parts may be written in a variety of programming languages and with an assortment of GUI toolkits.

Like most any program, wiak could be started up as the first main system application, after "init", on boot. However, wiak is more particularly of the family "shell" in that it can be used to start up (and in some cases control) other processes (or indeed other shells), whilst providing one or more IPC facilities for communicating between the running processes.

Compatible pipe-using commandline utilities and applications can be chained together, and called up and controlled, via wiak and a wiak compatible backend server, to produce a WIAK Application or "wiakapp".

In effect, wiak is thus a simple IPC subsystem which can be used to dynamically build a complex application by selection from a pool of individually functional parts. The resulting wiak-created-application, or wiakapp for short, is itself dynamic and temporary in shape and form, since wiak does not actually glue wiakapp component parts together. Instead, it creates through its message passing mechanism an illusion of virtual glue for what are otherwise unconnected processes.

Wiak encourages a client/server approach to programming, where, for example, the GUI component of the wiakapp can act as a client, sending simple commands and other data, via wiak, to a server application which itself might be a wrapper to a specific function or group of functions (e.g. GNU or UNIX commandline utilities). 
