# samp-disabled-commands

[![sampctl](https://img.shields.io/badge/sampctl-samp--disabled--commands-2f2f2f.svg?style=for-the-badge)](https://github.com/rsetiawan7/samp-disabled-commands)

## Introduction

Preview: 
<blockquote class="imgur-embed-pub" lang="en" data-id="a/cxUzkmh" data-context="false" ><a href="https://imgur.com/a/cxUzkmh">SA-MP Disabled Commands (click if not showed)</a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>

This include helps you to disable some commands because of maybe you're still fixing some bugs
and you want disable it without re-compile your code.

Only support ZCMD for now.

## Dependencies

- zcmd
- y_ini (from YSI 5.x)
- (optional) y_debug
- (optional) crashdetect

<!--
Short description of your library, why it's useful, some examples, pictures or
videos. Link to your forum release thread too.

Remember: You can use "forumfmt" to convert this readme to forum BBCode!

What the sections below should be used for:

`## Installation`: Leave this section un-edited unless you have some specific
additional installation procedure.

`## Testing`: Whether your library is tested with a simple `main()` and `print`,
unit-tested, or demonstrated via prompting the player to connect, you should
include some basic information for users to try out your code in some way.

And finally, maintaining your version number`:

* Follow [Semantic Versioning](https://semver.org/)
* When you release a new version, update `VERSION` and `git tag` it
* Versioning is important for sampctl to use the version control features

Happy Pawning!
-->

## Installation

Simply install to your project:

```bash
sampctl package install rsetiawan7/samp-disabled-commands
```

Include in your code and begin using the library:

```pawn
#include <disabled-commands>
```

Make sure you include this library in all of your gamemodes and filterscripts those have ZCMD.

## Usage

<!--
Write your code documentation or examples here. If your library is documented in
the source code, direct users there. If not, list your API and describe it well
in this section. If your library is passive and has no API, simply omit this
section.
-->
Callbacks:

```pawn
// Called when disabled commands reloaded.
public OnDisabledCommandsReloaded()
{
  SendClientMessageToAll(-1, "Disabled commands reloaded in GameMode.");
  return 1;
}

// Called when commands cancelled because of you disable it.
public OnPlayerCommandCancelled(playerid, cmdtext[])
{
  SendClientMessage(playerid, -1, "This command is disabled.");
  return 1;
}
```

Functions:

```pawn
// Add a command to be disabled. Reloaded automatically if success.
AddDisabledCommands(const command[]);

// Remove a command and enable it. Reloaded automatically if success.
RemoveDisabledCommands(const command[]);

// Reload disabled commands manually. (perhaps there are many changes in external?)
ReloadDisabledCommands(const command[]);
```