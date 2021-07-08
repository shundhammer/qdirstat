# QDirStat
<img src="https://github.com/shundhammer/qdirstat/blob/master/src/icons/qdirstat.svg" height="64">

Qt-based directory statistics: KDirStat without any KDE -- from the author of
the original KDirStat.

(c) 2015-2021 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2021-07-08


## Screenshot

[<img width="900" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-main-win.png)

_Main window screenshot - notice the multi-selection in the tree and the treemap_


## Overview

QDirStat is a graphical application to show where your disk space has gone and
to help you to clean it up.

This is a Qt-only port of the old Qt3/KDE3-based KDirStat, now based on the
latest Qt 5. It does not need any KDE libs or infrastructure. It runs on every
X11-based desktop on Linux, BSD and other Unix-like systems and of course in a
Docker container.

QDirStat has a number of new features compared to KDirStat. To name a few:

- Multi-selection in both the tree and the treemap.

- Unlimited number of user-defined cleanup actions.

- Properly show errors of cleanup actions (and their output, if desired).

- Configurable file categories (MIME types), treemap colors, exclude rules,
  tree columns.

- Package manager support:
  - Show what software package a system file belongs to.
  - [Packages view](doc/Pkg-View.md) showing disk usage of installed software
    packages and their individual files.
  - [Unpackaged files view](doc/Unpkg-View.md) showing what files in system
    directories do not belong to any installed software package.

- New views:
  - Disk usage per file type (by filename extension).
  - File size histogram view.
  - Free, used and reserved disk size for each mounted filesystem (like _df_)

See section [_New Features_](#new-features) for more details.


## Table of Contents

1. [Screenshot](#screenshot)
1. [Latest Stable Release](#latest-stable-release)
1. [Latest News](#latest-news)
1. [History](#history)
1. [Related Software](#related-software): KDirStat, WinDirStat, K4DirStat and more
1. [Motivation / Rant: Why?](#motivation--rant-why)
1. [Features](#features)
1. [MacOS X Compatibility](#macos-x-compatibility)
1. [Windows Compatibility](#windows-compatibility)
1. [Ready-made Packages](#ready-made-packages)
1. [QDirStat Docker Container](#qdirstat-docker-container)
1. [Building](#building)
1. [Contributing](#contributing)
1. [Troubleshooting](#troubleshooting)
1. [Further Reading](#further-reading)
1. [Packaging Status](#packaging-status)
1. [Donate](#donate)


## More Screenshots


[<img align="top" height="237" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-type-stats.png)
[<img align="top" height="169" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-output.png)
[<img align="top" height="150" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png"  >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-column-config.png)

[<img width="700" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-locating-file-types.png)


[<img align="top" width="180" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-cleanups.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-cleanups.png)
[<img align="top" width="180" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-mime.png"    >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-mime.png)
[<img align="top" width="180" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-exclude.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-exclude.png)
[<img align="top" width="180" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-general.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-general.png)


[<img align=top width="360" src="https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram.png"        >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram.png)

_Full-size images and descriptions on the [Screenshots Page](https://github.com/shundhammer/qdirstat/blob/master/screenshots/Screenshots.md)_


-----------------------

## Donate

QDirStat is Free Open Source Software; you are not required to pay anything.
If you want to support continued development of QDirStat, donations are most
welcome.

Donate any amount you like via PayPal:

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W)


## Latest Stable Release

**QDirStat V1.7.1**

See the [release announcement](https://github.com/shundhammer/qdirstat/releases).

Download installable binary packages for various Linux distributions here:
[Ready-made packages](#ready-made-packages)


## Latest News

- 2021-07-02

  - Don't show inactive (unmounted) mounts managed by the automounter anymore
    in the "Mounted Filesystems" window. They were listed there as filesystem
    type "autofs" and 0 bytes total size which was not very useful.

    Once they become auto-mounted by a user accessing them in the filesystem,
    they are shown there normally, of course.


- 2021-06-26

  - Improved the fix for [GitHub issue #169](https://github.com/shundhammer/qdirstat/issues/169):
    Breadcrumbs navigator becomes too wide for very long paths.

    Now no longer aggressively shortening all path components in the
    breadcrumbs navigator when the complete path becomes even just a little bit
    too long, but concentrating on the longest parts and shortening them one by
    one until the complete path is not too long anymore.


- 2021-06-24

  - First (crude) fix for [GitHub issue #169](https://github.com/shundhammer/qdirstat/issues/169):
    Breadcrumbs navigator becomes too wide for very long paths.
    Now shortening path components if the overall path would become too wide.
    Right now it shortens them a bit too aggressively.

- 2021-06-21 [QDirStat AppImage and why I don't like it](https://github.com/shundhammer/qdirstat/issues/168)

    TL;DR:
    - It's big and fat (113 MB)
    - It's an outdated version (QDirStat 1.6.1 from 16 months ago)
    - It doesn't even tell you what version it contains
    - It still needs a fairly recent version of GLibc, so you can't run it on Ubuntu 18.04 LTS
    - It's unclear if at least the libraries inside (e.g. the Qt libs) are up to date


- 2021-04-05 **New stable release: 1.7.1**

  **Summary:**

  - Added a "Discover" toplevel menu with actions to easily find
    - the largest files
    - the newest files
    - the oldest files
    - files with multiple hard links
    - broken symbolic links
    - sparse files

  - Now showing the target of symbolic links in the details panel.
    If the target does not exist, a **Broken Link** warning is also shown.

  - Menu reorganization. The new toplevel menus are now:

    File, Edit, View, Go To, Discover, Clean up, Help

  - Enabled _Refresh Selected_ now for files as well. In that case, the parent
    directory is refreshed.

  - Added hotkey `[F6]` for _Refresh Selected_.

  - Now ignoring the loopback mounts of installed snap packages in the "Open
    Directory" dialog and in the "Mounted Filesystems" window.

  - Added links to external documents to a new "Problems and Solutions" submenu
    of the "Help" menu so they are more easily discoverable.

  - Added a document about
    [finding files that are shadowed by a mount](doc/Shadowed-by-Mount.md)
    and a script for the most common case.

  - Bug fix: Fixed [GitHub Issue #149](https://github.com/shundhammer/qdirstat/issues/149):

    Segfault when using a cleanup action with refresh policy _AssumeDeleted_
    after a cleanup action with _RefreshParent_ in the same directory.


  **Details:**

  - Added a "Discover" toplevel menu with actions to easily find
    - the largest files
    - the newest files
    - the oldest files
    - files with multiple hard links
    - broken symbolic links
    - sparse files

    in the whole displayed directory tree or, if a directory is selected, in
    that subtree.

    In each case, a non-modal dialog is opened with a list of the results.
    Clicking on one of them selects it in the main window where you can see
    more details and use cleanup actions.

    [<img width="750" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-discover.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-discover.png)

    Most of that was already there in some way or the other, but now it's
    easier to discover (pun intended) and to use.

    Finding large files is of course what the treemap is primarily for; just
    spot large blobs and click on them to find out what they are and where they
    are hiding.

    Finding the newest files can also be done by sorting the tree by the "last
    modified" column and then opening the topmost branches. This is often
    useful to find out where some browser dumped that last download.

    Similarly, to find the oldest files, enable the "oldest files" tree column,
    sort by that and open branches until you can see a file.

    Files with multiple hard links or sparse files were mentioned in the log;
    otherwise they were not so easy to find (short of using the command line,
    of course).


  - Now showing the target of symbolic links in the details panel.

    If it's a short path, the whole path is shown; otherwise without the path
    (".../somewhere"), and the full path is shown as a pop-up upon mouse click.

    If the target does not exist, a **Broken Link** warning is also shown.


  - Menu reorganization: They had become a little too crowded, especially on
    the top level.

    - The new toplevel menus are now:

      File, Edit, View, Go To, Discover, Clean up, Help

      I.e. it's down to 7 items which is generally regarded as the gold
      standard by usability experts.

    - The former "Settings" menu is gone; "Configure QDirStat" is now in the
      "Edit" menu. There was only that one action in the "Settings" menu, and
      that is quite wasteful in terms of screen space and toplevel menu
      complexity.

    - Moved out some options entirely from the menus; they are still available
      when editing the config file manually:

      - "Show current path"
      - "Treemap as side panel"

    - "Expand tree level" is now limited to level 5 (formerly 9). Opening that
      many tree branches means a huge performance drop anyway.

    - The former "Treemap" menu is now a submenu of "View". Most of those
      actions are available as tool bar buttons and mouse wheel operations
      anyway.


  - Enabled _Refresh Selected_ now for files as well. In that case, the parent
    directory is refreshed.

  - Added hotkey `[F6]` for _Refresh Selected_.

    `[F5]` is still _Refresh All_ like in all web browsers. Since window
    managers / desktop environments tend to consume `[F5]` with modifier keys
    (`[Shift] [F5]`, `[Alt] [F5]`, `[Ctrl] [F5]`), this is the closest we can
    get, and it's more consistent than using something like `[Ctrl] [R]`.

    This was inspired by the discussion in [PR#145](https://github.com/shundhammer/qdirstat/pull/145).

  - Now ignoring the loopback mounts of installed snap packages in the "Open
    Directory" dialog and in the "Mounted Filesystems" window.

    Yes, each of them has a separate loop mount, even if it's only installed,
    not in active use. Those mounts clutter the output of commands like `df` or
    `mount` with nonsensical cruft:

    ```
    df -hT | grep snap

    /dev/loop0  squashfs  159M  159M  0 100% /snap/chromium/1244
    /dev/loop1  squashfs   55M   55M  0 100% /snap/core18/1880
    /dev/loop2  squashfs   63M   63M  0 100% /snap/gtk-common-themes/1506
    /dev/loop3  squashfs   30M   30M  0 100% /snap/snapd/8542

    ```

    (From a freshly installed Xubuntu 20.04 LTS)



  - Added links to external documents to a new "Problems and Solutions" submenu
    of the "Help" menu so they are more easily discoverable.

  - Added a document about
    [finding files that are shadowed by a mount](doc/Shadowed-by-Mount.md)
    and a script for the most common case.

  - Bug fix: Fixed [GitHub Issue #149](https://github.com/shundhammer/qdirstat/issues/149):

    Segfault when using a cleanup action with refresh policy _AssumeDeleted_
    after a cleanup action with _RefreshParent_ in the same directory.


-------------



_See [DevHistory.md](doc/DevHistory.md)
for older entries._


## History


This is just a rough summary. For more details, see [DevHistory.md](doc/DevHistory.md).

- 2021-04-05 New stable release: 1.7.1

  - Added a "Discover" toplevel menu with actions to easily find
    - the largest files
    - the newest files
    - the oldest files
    - files with multiple hard links
    - broken symbolic links
    - sparse files

  - Now showing the target of symbolic links in the details panel.
    If the target does not exist, a **Broken Link** warning is also shown.

  - Menu reorganization. The new toplevel menus are now:

    File, Edit, View, Go To, Discover, Clean up, Help


- 2020-07-26 New stable release: 1.7

  - Closing the gap between sizes reported by QDirstat and sizes reported by
    the `du` command: Now also taking the allocated size into account.

  - Now also displaying the allocated size where it makes sense.

  - New "Mounted Filesystems" window showing output similar to the `df` command
    (but without the cruft).

  - New directory selection dialog showing all (real) filesystems.


- 2020-02-13 New stable release: 1.6.1

  - Much better handling for "permission denied" errors while reading directories

  - Now showing the exact byte size (134 495 994 Bytes instead of 128.3 MB)
    upon mouse click

  - New optional tree column "Oldest File" (not enabled by default)


- 2019-07-22 New stable release: V1.6

  - New _packages_ view

  - New _unpackaged files_ view

  - Performance improvements while reading directories

  - Vast Performance improvement for huge directories
    (100.000+ entries in a single directory)


- 2018-11-07 New stable release: V1.5

  - New _details_ panel

  - Package manager support to show what software package a file belongs to

  - New _breadcrumbs_ navigation

  - Switchable tree layouts L1 / L2 / L3


- 2017-06-04 New stable release: V1.4

  - New _file size statistics_ with histogram

  - Shading for empty space in the treemap for lots of very small files


- 2017-03-05 New stable release: V1.3

  - New _file type_ view

  - Locate files of a certain type (filename extension) in the tree


- 2017-01-03 New stable release: V1.2

  - Improved Btrfs subvolumes support


- 2016-10-31 New stable release: V1.1-Pumpkin

  - Bug fixes

  - Split up the config file into several ones


- 2016-05-16 First stable release: V1.0

- 2016-04-08 Beta 3 release

- 2016-03-20 Beta 2 release

- 2016-02-06 Beta 1 release

- 2015-11-28 QDirStat project start: Ported from the old KDE 3 KDirStat

- Predecessor: KDE 3 **KDirStat**

  - 2006-06-01 KDirStat 2.5.3: The last KDE3 based version.

  - 2003: Bernhard Seifert wrote **WinDirStat** based on the KDirStat idea of
    coupling a tree view and a treemap and providing cleanup actions.

  - 2003-01-05 KDirStat 2.3.3: Treemaps

  - 2002-02-25 KDirStat 2.0.0: Complete rewrite for KDE 2 / Qt 2

  - 2000-01-21 KDirStat 0.86 for KDE 1 announced: First public version.


## Related Software

### KDirStat and QDirStat

KDirStat was the first program of this kind (combining a traditional tree view
with a treemap), also written by the same author as QDirStat. It was made for
KDE 1 back in early 2000; later ported to KDE 2, then KDE 3.

QDirStat is based on that code, but made independent of any KDE libraries or
infrastructure, so it has much fewer library and package dependencies;
basically only the Qt 5 libs and libz, both of which most Linux / BSD machines
have installed anyway if there is any graphical desktop installed.


### WinDirStat and QDirStat

There are lots of articles and user forum comments about QDirStat being a "nice
Linux port of WinDirStat". Well, nothing could be further from the truth:
**WinDirStat is a Windows port of KDirStat**, the predecessor of QDirStat.

So it's the other way round: **The Linux version was there first**, and
somebody liked it so much that he wrote a Windows version based on that
idea. That's a rare thing; usually people port Windows originals to Linux.

See also https://windirstat.net/background.html and the WinDirStat "About"
dialog.



### QDirStat and K4DirStat

K4DirStat is a port to KDE 4 / Qt 4 of the old KDE 3 / Qt 3 KDirStat. QDirStat is
independent of that; it is based on the old KDE 3 KDirStat directly.


### Other

- Baobab
- Filelight
- ncdu
- du

See
[Disk Usage Tools Compared](https://github.com/shundhammer/qdirstat/wiki/disk-usage-tools-compared):
QDirStat vs. K4DirStat vs. Baobab vs. Filelight vs. ncdu (including benchmarks)
in the Wiki.


## Motivation / Rant: Why?

After having used KDE since its early days (since about 1998), I didn't like
the direction anymore that KDE has been taking. I loved KDE 1, KDE 2, KDE
3. When KDE 4 came along, it took me a long time to try to adopt it, and when I
did, I moved back to KDE 3 after a short while, then tried again with the next
release, moved back again -- several times.

I really tried to like it, but whenever I thought I tamed it to meet my
requirements, a new version came along that introduced yet another annoyance.

To name a few:

- A lot of things that used to be user configurable in KDE 3 are not
  configurable anymore, and when you approach the KDE 4/5 developers about
  that, they will tell you that this is intentional, and they do not intend to
  bring those config options back. Well, thanks a lot; this is the Apple
  approach where they think they know what is good for you, and you are just
  too stupid.

- Konqueror as the old central tool is as good as dead. It's still there as an
  alternate file manager (for those who find it), but the primary one is the
  dumbed-down Dolphin that I consider unusable: It's only useful for complete
  newbies, not for power users. The web browser part of Konqueror is so
  outdated that you can't do much with it with most modern web sites, so the
  great integration of web and local file manager that was the major strong
  point of Konqueror (and thus KDE) no longer exists.

- I don't like the fact that I can't simply put icons on my desktop anymore --
  no, I have to create a plasmoid first as a container, and those things keep
  doing weird stuff that drives every user crazy. With one false move of your
  mouse, it might be gone, change shape, move to another place or whatever.

- I also don't like the desktop search that eats resources like there is no
  tomorrow (disk space, disk I/O, CPU usage) and that for all practical
  purposes you can't get rid of.

- I don't like the fact that the mail client relies on that MySQL based
  framework called _Akonadi_ that is not only resource-hungry, but also so
  fragile that I had to use the _akonadiconsole_ lots of times just to bring it
  back to life. Seriously, if I as a Linux system developer have a hard time
  doing that, what is a normal user expected to do?

- Activities vs. multiple desktops. I tried to use both, but they don't
  integrate well. The desktops previewer is far inferior to the old one from
  KDE3: Only monochrome rectangles, no real preview. The activities plasmoid
  keeps rearranging my carefully placed and named activities at random. WTF?!

- Everything is so fragmented that not even the naming is clear anymore. What
  used to be KDE is now a jumble of the KF Framework, the KF libs, the KF apps
  and the Plasma desktop. Yeah, great job, folks; people used to know what KDE
  stood for. Nobody knows what the hell all those components are, and neither
  does anybody care anymore. You paved your way to oblivion with buzzwords.
  Great marketing strategy for gaining more visibility!

Then the next generation KDE arrived, _Plasma 5_. When I was force-migrated to
it at work with the _SUSE Tumbleweed_ rolling release, the experience was so
bad that I moved to the _Xfce_ Desktop.

Now every time I started my own KDirStat, it started about a dozen KDE
processes along with it -- processes that it needs only for minor things like
loading icons or translations. I really don't need or want that.

So it was time to make KDirStat self-sufficient; it never used that much of all
the KDE infrastructure anyway. Time to make a pure Qt-based and self-sufficient
QDirStat.

And while I was at it, I took the chance to add some features that I had wanted
for a long time, yet I had never gotten myself to start working on:

- Multi-selection in the directory tree so you can delete several files at
  once.

- Remove limitations like having only a fixed number of user-defined cleanup
  actions.

- Properly show the output of cleanup actions, in particular when they reported
  errors.

- Make treemap colors configurable: Use custom colors and match them to
  user-defined filename extensions.

- Move away from the arcane KDE build system: Back with KDE 1/2/3 it was the
  _Autotools_ with custom KDE extensions that only a handful people in the
  world really understood (I was not among them), later _CMake_ which is little
  better, just differently confusing.

Yes, there is a Qt4 / Qt5 port of KDirStat called K4DirStat. K4DirStat is an
independent project that started when I had not worked on the old KDirStat for
a long time (my last KDirStat release had been in mid-2006).

QDirStat is based on that same code from the 2006 KDirStat. It's an 80% rewrite
using a lot of newer Qt technologies. And there was a lot of cleaning up that
old code base that had been long overdue.



## Features

### New Features

- Multi-selection:

  - Both views (the tree and the treemap) now support _extended_ selection,
    i.e. you can select more than one item. This was the most requested feature
    for the last KDirStat. Now you can select more than one item at the same
    time to move it to the trash can, to directly delete it or whatever.

  - Tree view:
    - Shift-click: Select a range of items.
    - Ctrl-Click:  Select an additional item or deselect a selected one.

  - Treemap:
    - Ctrl-Click:  Select an additional item or deselect a selected one.

    - The current item is highlighted with a red rectangle, all other selected
      ones with a yellow rectangle. If the current item is not also selected,
      it has a dotted red outline.

- Proper output of cleanup actions with different colors for the commands that
  are executed, for their output and for error messages (see screenshot
  above). That output window can be configured to always open, to open after a
  certain (configurable) timeout, or only if there are error mesages -- or not
  at all, of course. If things go wrong, you can kill the external command
  started by the cleanup action from there. You can zoom in and out (increase
  or decrease the font size) as you like.

- File type statistics window. WinDirStat has it, and users wanted it in
  QDirStat, too. Since filename extensions (suffixes) don't have as much
  semantics in Linux/Unix systems as they do in Windows, many files are
  categorized as "Other". This is a known limitation, but it's a limitation of
  the whole concept of using suffixes to categorize files by type. And no,
  checking file headers for magic byte sequences like the "file" command does
  is not an option here; QDirStat would have to do that for (at least) all the
  30,000+ files typically listed under the "Other" category. So we'll have to
  live with that limitation.

- Locate files by file type window. If you double-click on any of the filename
  extensions (suffixes) in the file type statistics window, you will get
  another window that lists all the directories that contain files of that type
  including the number and total size of those files. You can double-click each
  of those lines, and that directory will open in the main window with the
  files of that type preselected so you can start cleanup actions like moving
  them to trash or converting them to a better format (.bmp -> .png)
  immediately.

- File size statistics window with histogram, percentiles, buckets and a lot of
  documentation that everybody should be able to understand. Even if (or,
  better yet, in particular if) your math teacher or statistics professor never
  explained it properly, please have a look at it.

- Packages view: Show installed packages and their files in the tree. Supported
  for all Linux distributions using any of _dpkg_, _rpm_, _pacman_ as their
  low-level package manager or any higher-level package manager like _apt_,
  _zypper_ etc.; more details at [Pkg-View.md](doc/Pkg-View.md).

- Unpackaged files view: Show a directory tree, but ignore all files that
  belong to an installed software package. Those ignored files are displayed in
  a special branch _<Ignored>_ in the tree view, and they are not displayed at
  all in the treemap. This is useful to find files that were manually
  installed by a `sudo make install` command. More details at
  [Unpkg-View.md](doc/Unpkg-View.md).

- New macros to use in cleanup actions:

  - %d : Directory name with full path. For directories, this is the same as
    %p. For files, this is their parent directory's %p.

  - %terminal : Terminal window application of the current desktop; one of
    "konsole", "gnome-terminal", "xfce4-terminal", "lxterminal", "eterm".
    The fallback is "xterm".

  - %filemanager : File manager application of the current desktop; one of
    "konqueror", "nautilus", "thunar", "pcmanfm". The fallback is "xdg-open".

- Which desktop is used is determined by the _$XDG_CURRENT_DESKTOP_ environment
  variable. Users can override this with the _$QDIRSTAT_DESKTOP_ environment
  variable, so you can get, say, the Xfce terminal or file manager despite
  currently running KDE if you set

      export QDIRSTAT_DESKTOP="Xfce"

- Of course, you can still simply use your favourite file manager if you simply
  change %filemanager in the default "Open File Manager Here" cleanup action to
  the command to start it.

- You can now select the shell to use for the cleanup commands:

  - $SHELL (the user's login shell) - using the same environment, syntax and
    wildcard etc. behaviour of the shell the user is used to.
  - /bin/bash for well-defined behaviour for wildcards etc.
  - /bin/sh as a last resort (which might be a simplistic _dash_ on Ubuntu).

- Mouse actions in the treemap window:

    - Left click:           Select item and make it the current item.
    - Right click:          Open the context menu with cleanup actions and more.
    - Ctrl+Left click:      Add item to selection or toggle selection.
    - Middle click:         Select the current item's parent. Cycle back at toplevel.
    - Double click left:    Zoom treemap in.
    - Double click middle:  Zoom treemap out.
    - Mouse wheel:          Zoom treemap in or out.

- You can configure what columns to display in the tree view and in which
  order. The only thing that is fixed is the "Name" column which is always
  there and always the first (leftmost). Use the context menu in the tree
  header to unlock column widths. Drag columns to the left or right to change
  their order.

- Exclude rules are now greatly simplified. They no longer always get the
  entire path to match which requires quite complex regexps; by default, they
  only get the last path component -- i.e., no longer
  "/work/home/sh/src/qdirstat/src/.git", but only ".git". You can now even tell
  the exclude rule to use a simplified syntax: "FixedString" or "Wildcard" in
  addition to the normal "RegExp". The old behaviour (matching against the full
  path) is still available, though.

- Configuration dialog for exclude rules -- see screenshots.

- Subvolume detection for Btrfs. Btrfs subvolumes are just ordinary mount
  points, so normally QDirStat would stop scanning there, leaving a large part
  of a Btrfs partition unaccounted for. But for each mount point found while
  scanning a directory tree, QDirStat checks /proc/mounts or /etc/mtab if it
  has the same device name as its parent directory, and if yes, considers it a
  subvolume and continues scanning.

- Actions to go one directory level higher or to the toplevel: Context menu and
  menu "Go To" -> "Up One Level" or "Toplevel". This is useful if you clicked
  on a file in the treemap that is deep down in some subdirectory, and you want
  to know what subdirectory that is: Simply click "Go Up" twice (the first
  click will get you to the <Files> pseudo subdirectory, the second one to the
  real one).

- Open all tree branches up to a certain level and close all other ones: Menu
  "View" -> "Expand Tree To Level" -> "Level 0" ... "Level 9".

- The total sum of the selected items (subtrees) is displayed in the status
  line if more than one item is selected.

- Icons are now compiled into the source thanks to Qt's resource system; now
  it's just one binary file, and nothing will go missing. No more dozens of
  little files to handle.

- The build system is now Qt's _QMake_. I got rid of that _AutoTools_
  (Automake, Autoconf, Libtool) stuff that most developers find intimidating
  with its crude M4 macro processor syntax. QMake .pro files are so much
  simpler, and they do the job just as well. And no, it will definitely never
  be _CMake_: I don't like that thing at all. It's just as much as a PITA as
  the AutoTools, just not as portable, no usable documentation, it's changing
  all the time, and those out-of-source builds are a royal PITA all on their
  own with constantly having to change back and forth between source and build
  directories.

- QDirStat now has its own log file. It now logs to
  `/tmp/qdirstat-$USER/qdirstat.log` (where $USER is your Linux user name).
  No more messages on stdout that either clobber the shell you started the
  program from or that simply go missing.

- No longer depending on dozens of KDE libs and a lot of KDE infrastructure; it
  now only requires Qt which is typically installed anyway on a Linux / BSD /
  Unix machine with any X11 (graphical) desktop.

- It should still compile and work with Qt4. We now have a contributor who is
  very interested in that (Michael Matz), so it should be possible to maintain
  this compatibility.

- Slow down display update from 333 millisec (default) to 3 sec (default) with
  `qdirstat --slow-update` or `qdirstat -s`. The slow update interval can be
  customized in `~/.config/QDirStat/QDirStat.conf`:

    ```ini
    [DirectoryTree]
    SlowUpdateMillisec = 3000
    ```



### Old Features

Features ported from the old KDirStat:

- Fast and efficient directory reading.

- Not crossing filesystem boundaries by default so you can see what eats up
  all the disk space on your root filesystem without getting distorted numbers
  due to all the other filesystems that are mounted there. If you absolutely
  wish, you can use "Continue reading at mount point" from the context menu or
  from the "File" menu -- or configure QDirStat to always read across file
  systems.

- Efficent memory usage. A modern Linux root filesystem has well over 800,000
  objects (files, directories, symlinks, ...) and about 100,000 directories.
  This calls for minimalistic C++ objects to represent each one of
  them. QDirStat / KDirStat do their best to minimize that memory footprint.

- Hierarchical tree view that displays accumulated sums in each branch,
  together with a percent bar so you can see at a glimpse how the
  subdirectories compare with each other.

- All numbers displayed human readable -- e.g., 34.4 MB instead of 36116381
  Bytes.

- All size units are 1024-based, i.e. 1 kB = 1024 Bytes; 1 MB = 1024 kB; 1 GB =
  1024 MB.

- In the tree, also displaying the exact byte size as the context menu (right
  click).

- Each tree level uses another color for that percent bar so you can easily
  compare subdirectories even if some of them are opened in the tree.

- If a directory has files and subdirectories, all files in that subdirectory
  are grouped into a <Files> pseudo directory (called _dot entry_ in the
  QDirStat sources) so you can compare the disk usage of files on that
  directory level with the subdirectories.

- Displaying the latest modification time of any object in each branch. You can
  instantly see in what subdirectory where any changes lately. You can sort by
  this column, of course.

- Treemap display. Treemaps are a way to visualize hierarchical data
  structures, invented by Ben Shneiderman. Basically, the hierarchy is
  flattened and each level grouped in a rectangle, inside which it is again
  subdivided in rectangles. The area of each rectangle corresponds to the size
  of each item or subdirectory. For the purposes of QDirStat, it is enough to
  know that a large blob corresponds to a large file; you can instantly see
  where large ISOs or movies are.

- You can zoom the treemap in and out (Ctrl + / Ctrl - / mouse wheel / menu /
  tool bar) to see more details of directories that are otherwise dominated by
  larger ones.

- You can move the boundary between treemap and tree view up and down as you
  like. You can also get rid of the treemap completely (menu "Treemap" -> "Show
  Treemap" or F9 key)

- Treemap and tree list view communicate. Select an item in one view, and it is
  also selected in the other. If you click on that large blob in the treemap,
  it is located in the tree view, all branches up to its directory are opened,
  and the tree view scrolls to that item.

- Cleanup actions. Once you know what is consuming the disk space, you can
  start cleanup actions from within QDirStat to reclaim disk space - or to
  investigate further if you can safely delete a file. You can create your own
  cleanup actions (as many as you like), and there are some predefined ones:

  - Open file manager here. This will start a file manager in the directory of
    the current item. QDirStat tries its best to guess the name of the relevant
    file manager application for the current desktop, based on the
    $XDG_CURRENT_DESKTOP environment variable. You can override this with the
    $QDIRSTAT_DESKTOP environment variable.

  - Open terminal window here. In most cases, this is much easier than to
    navigate to that directory with 'cd' in an already open terminal window and
    using tab-completion numerous times. As with the file manager application,
    QDirStat tries its best to guess the name of the relevant terminal window
    application for the current desktop.

  - Move to trash bin. QDirStat has its own implementation of the XDG trash
    specification.

  - Delete immediately.

  - Compress: Create a compressed tar archive from a directory and then delete
    the directory.

  - Delete junk files: Backup files left behind by editors, core dumps.

  - All predefined cleanup actions are fully configurable, of course. You can
    change any of them, disable them, or delete them.

- You can copy the complete path of the selected file or directory to the
  system clipboard and paste it to another application.

- Reading and writing cache files:

  - This is mostly meant for remote servers in some server room somewhere:
    Rather than installing the Qt and X11 runtime environment and running
    QDirStat over remote X (ssh with X forwarding), you can run the supplied
    _qdirstat-cache-writer_ Perl script on the server, copy the resulting cache
    file to your desktop machine and view the content there with QDirStat.

  - For large directories (archives etc.) that don't change that much, you can
    also generate a QDirStat cache file (either with the Perl script or with
    QDirStat itself) and save it to that corresponding directory. If QDirStat
    finds a file .qdirstat.cache.gz in a directory, it checks if the toplevel
    directory in that cache file is the same as the current directory, and if
    it is, it uses the cache file for that directory rather than reading all
    subdirectories from disk. If you or the users of that machine use QDirStat
    often, this might take a lot of I/O load from the server.

  - If you use the '-l' option of the qdirstat-cache-writer script, it uses the
    long file format with a complete path for each entry, so you can use the
    _zgrep_ command with it as a replacement for the _locate_ command.

  - The KDirStat / QDirStat file format is well documented and very simple. It
    seems to be used by a number of admins and some backup software.
    See also the specification in the doc/ directory:
    https://github.com/shundhammer/qdirstat/blob/master/doc/cache-file-format.txt

  - You can specify a cache file to read directly at the command line:

    ```
    qdirstat --cache cache-file
    ```

- Other command line options: See
    ```
    qdirstat --help
    ```


### Features that are Gone

(Compared to the old KDirStat)

- Pacman animation
- KIO slave support
- Feedback form

<details>

- KPacman: That was that PacMan animation wile reading directory reading. This
  is gone now. KPacMan looked out of place pretty soon after it got to KDirStat
  due to Qt styles doing fancy rendering of widget backgrounds with gradients
  etc.  I know that it does have its fans, but it's unrealistic to get this
  back without breaking the menu bar rendering.

- KioDirReadJob: Network-transparent directory reading for network protocols
  like FTP, HTTP, Fish (ssh-based). This depended on KDE's KIO slaves, so this
  functionality is gone now without KDE. That's a pity, but this is a little
  price to be paid to avoid the rest of the hassle with using the KDE libs.

- KFeedback: That was that form where users could tell their opinion about
  KDirstat. But that was not used that often anyway - not nearly enough to
  justify the effort that has gone into that part. And the KDE usability
  people, like usability people generally tend to do, first discussed that to
  death and then decided they didn't want anything like that in general in KDE
  applications. So be it.

- KActivityTracker: That was a supporting class for KFeedback that kept track
  of how much a user was using the program and after a while (when it was
  determined that it made sense) asked if the user wouldn't like to give his
  feedback about the program.
  Don't you all just hate those dumbass web designers who tell you to do a
  survey how much you like their grand web page before you even had a chance to
  look at it? Shove a pop-up up your face covering the stuff you are
  interesting in with their self-loving marketing bullshit? -- KActivityTracker
  was made to avoid exactly this: Ask the user only once you know that he
  actually used the program for a while.

</details>



## MacOS X Compatibility

<summary>
There is some experimental support for MacOS X, but it's really only that: Experimental.
</summary>
<details>

I was amazed to find that it doesn't take more than the normal "qmake" and then
"make" to build QDirStat for MacOS X. We (Sonja Krause-Harder and I) did some
basic testing, and it seems to work.

The cleanups may need some adaptation, but this is something that might even be
configured by the user.

If anybody wants to give it a try, download Qt for MacOS X, install it, open a
shell window, search the _qmake_ command:

    find . -name qmake

Add this to your $PATH, then do the normal

    qmake
    make

Not sure how well "make install" works, though.

**_Be advised that QDirStat on MacOS X is purely experimental at this stage._**

There is no support. If you try this, you are on your own. Even more so than
with the other platforms, you will have to make sure that your Qt build
environment is set up correctly.

</details>

_There be dragons._ ;-)


### Architecture maintainer wanted for QDirStat for MacOS X

If you are a developer with some prior C++ and Qt knowledge on the MacOS X
platform and you'd like to see QDirStat working there, please consider joining
the team.


## Windows Compatibility

There is no native Windows version, but you can use the
[docker](#qdirstat-docker-container) container.
Other than that, there is [WinDirStat](https://windirstat.info/).

<details>
There are currently no plans for doing a native Windows port.

Directory reading might be quite easy to replace for Windows; we don't have
that problem with devices and crossing filesystems on that platform.

But the cleanups might be a challenge, "move to trash" works completely
differently, and we'd need an installer for a Windows version.

So, for the time being, use the [docker](#qdirstat-docker-container) or
[WinDirStat](https://windirstat.info/) instead.

WinDirStat is a close relative to the KDirStat family anyway; the author had
liked KDirStat on Linux so much that he decided to write a Windows clone and
called it WinDirStat.
</details>


## Ready-made Packages

[![Packaging status](https://repology.org/badge/tiny-repos/qdirstat.svg)](https://repology.org/metapackage/qdirstat/versions)


### openSUSE / SUSE Linux Enterprise

QDirStat packages for openSUSE Tumbleweed / Leap (15.x, 42.x) and SLE (15, 12):

- Download page for the [**latest stable release**](https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-stable&package=qdirstat)

- Download page for the [**current development version** (git master)](https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-git&package=qdirstat)

  Since this version is in development, it may be not quite as stable and
  reliable as the latest official stable release, although the QDirStat
  developers try their best to keep it as stable as possible.


### Ubuntu

https://packages.ubuntu.com/search?keywords=qdirstat&searchon=names


### Debian

https://packages.debian.org/search?keywords=qdirstat


### Fedora

https://apps.fedoraproject.org/packages/qdirstat/builds/


## QDirStat Docker Container

@jlesage kindly provides a
[docker container for QDirStat](https://hub.docker.com/r/jlesage/qdirstat).

That makes QDirStat usable even on non-Linux / non-Unix systems such as Windows
or MacOS X.

Docker is basically a virtualized environment to run software that was designed
for a different operating system.

It is very much like using VmWare or VirtualBox, but with much less overhead:
You don't have to install that other system first and then on top of that the
application that you really want to run. A docker container contains everything
that is needed, and it is preconfigured for that application.


### How to use Docker on Windows

- Download and install [Docker Desktop for Windows](https://hub.docker.com/editions/community/docker-ce-desktop-windows)
- Use the Docker Desktop for Windows to launch the
[QDirStat Docker](https://hub.docker.com/r/jlesage/qdirstat) (as a Linux container)
- Adapt the command line so the disk that you want to explore is used

More information:

- https://docs.docker.com/docker-for-windows/install/
- https://docs.docker.com/get-started/



## Building

_Notice that for most mainstream Linux or BSD distributions you don't have to
build your own; you can simply install a ready-made package from your normal
package manager._


### Build Environment

Make sure you have a working Qt 5 build environment installed. This includes:

- C++ compiler (gcc recommended)
- Qt 5 runtime environment
- Qt 5 header files
- libz (compression lib) runtime and header file

If anything doesn't work, first of all **make sure you can build any of the
simple examples supplied with Qt**, e.g. the
[calculator example](http://doc.qt.io/qt-5/qtwidgets-widgets-calculator-example.html).


#### Ubuntu

Install the required packages for building:

    sudo apt-get install build-essential qtbase5-dev zlib1g-dev

Dependent packages will be added automatically.

Recommended packages for developers:

    sudo apt-get install qttools5-dev-tools qtbase5-doc qtbase5-doc-html qtbase5-examples

See also

http://askubuntu.com/questions/508503/whats-the-development-package-for-qt5-in-14-04

If you also have a Qt4 development environment installed, select the desired
one via _qtchooser_:

    sudo apt-get install qtchooser
    export QT_SELECT="qt5"


#### SUSE

Install the required packages for building:

    sudo zypper install -t pattern devel_C_C++
    sudo zypper install libQt5Widgets-devel libqt5-qttools zlib-devel

If you also have a Qt4 development environment installed, make sure that the
Qt5 version of 'qmake' is the first in your $PATH:

    export PATH=/usr/lib64/qt5/bin:$PATH


### Compiling

Open a shell window, go to the QDirStat source directory, then enter these
commands:

    qmake
    make


### Installing

    sudo make install

or

    su -c make install


### Install to a Custom Directory

The default setup installs everything to `/usr`. To install to another
directory, set `INSTALL_PREFIX` during `qmake`.

<details>

    qmake INSTALL_PREFIX=/usr/local

Beware that some things might not work as expected; for example, you will not
get a `.desktop` file in the proper place to make QDirStat appear in any menus
in your graphical desktop environment or in the file manager. You will need to
copy the `.desktop` file manually to whatever directory your graphical desktop
environment uses somewhere in your home directory. Similar with the application
icon used in that `.desktop` file.
</details>


## Contributing

See file [Contributing.md](doc/Contributing.md)
and [GitHub-Workflow.md](doc/GitHub-Workflow.md)


## Troubleshooting

### Can't Move a Directory to Trash

See file [Troubleshooting.md](doc/Troubleshooting.md)


## Further Reading

- Original [KDirStat](http://kdirstat.sourceforge.net/)  ([source code](https://github.com/shundhammer/kdirstat))
- [K4Dirstat](https://bitbucket.org/jeromerobert/k4dirstat/wiki/Home) ([source code](https://bitbucket.org/jeromerobert/k4dirstat/src))
- [WinDirStat](https://windirstat.info/) (for Windows)
- [Disk Usage Tools Compared: QDirStat vs. K4DirStat vs. Baobab vs. Filelight vs. ncdu](https://github.com/shundhammer/qdirstat/wiki/disk-usage-tools-compared) (including benchmarks)
- [XDG Trash Spec](http://standards.freedesktop.org/trash-spec/trashspec-1.0.html)
- [Spatry's QDirStat Review on YouTube](https://www.youtube.com/watch?v=ysm4-x_5ftI)

Of course, don't forget to check out the [doc directory](doc/).


## Packaging Status

Repology: QDirStat versions in Linux / BSD distributions:

[![Repology](https://repology.org/badge/tiny-repos/qdirstat.svg)](https://repology.org/metapackage/qdirstat/versions)

(click for details)


## Donate

QDirStat is Free Open Source Software; you are not required to pay anything.
If you want to support continued development of QDirStat, donations are most
welcome.

Donate any amount you like via PayPal:

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W)
