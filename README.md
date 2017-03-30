# QDirStat

Qt-based directory statistics: KDirStat without any KDE -- from the author of
the original KDirStat.

(c) 2015-2017 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2017-03-31


## Overview

QDirStat is a graphical application to show where your disk space has gone and
to help you to clean it up.

This is a Qt-only port of the old Qt3/KDE3-based KDirStat, now based on the
latest Qt 5. It does not need any KDE libs or infrastructure. It runs on every
X11-based desktop on Linux, BSD and other Unix-like systems.

QDirStat has a number of new features compared to KDirStat. To name a few:

- Multi-selection in both the tree and the treemap.
- Unlimited number of user-defined cleanup actions.
- Properly show errors of cleanup actions (and their output, if desired).
- File categories (MIME types) and their treemap color are now configurable.
- Exclude rules for directories are easily configurable.
- Desktop-agnostic; no longer relies on KDE or any other specific desktop.

See section _New Features_ for more details.



## Screenshots

[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png" width="1241">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-main-win.png)

_Main window screenshot - notice the multi-selection in the tree and the treemap_


[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png" height="237">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-type-stats.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png" height="169">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-output.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png" height="170">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-column-config.png)

[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png" height="337">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-locating-file-types.png)


[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-config.png" height="245">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-config.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-mime-config.png" height="245">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-mime-config.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-exclude-rules-config.png" height="245">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-exclude-rules-config.png)


_Full-size images and descriptions on the [Screenshots Page](https://github.com/shundhammer/qdirstat/blob/master/screenshots/README.md)_


-----------------------


## Latest News

**Latest stable release: V1.3**

- 2017-03-10 Filling the gaps in the treemap

  [GitHub issue #58](https://github.com/shundhammer/qdirstat/issues/58) shows
  that users feel under-informed when there are grey areas in the treemap. The
  explanation is simple: Treemap tiles are only displayed when they have at
  least a certain minimum size (by default 3 pixels). Otherwise the treemap
  just gets cluttered with tiny things that don't show any information
  whatsoever.

  The remaining space is taken by its parent directory's tile. They were
  rendered just flat grey which makes their boundaries against each other
  invisible, thus giving the impression that there is nothing.

  So I experimented with visible borders, but that completely destroyed the
  visual impression of the treemap because those borders were everywhere. Fill
  patterns also didn't help: They were just ugly, and there was no way to tell
  where one directory tile ends and where the next one starts.

  Then I tried gradients. The first impression was good, but then I found that
  it was hard to tell which item was a (now over-emphasized) directory and
  which one a large file. Locating large files deep inside the directory
  hierarchy is the major strong point of the treemap visualization, so I
  wouldn't want to give that up. After playing a bit with the gradient
  parameters (toning it down and giving it just a little blueish tint) I ended
  up with this:

  ![Using Gradients](https://cloud.githubusercontent.com/assets/11538225/23812600/7e769e48-05db-11e7-820c-2a6f70916403.png)

  I think this is a good compromise.

  Of course this is configurable: Edit `~/.config/QDirStat/QDirStat.conf`:

      [Treemaps]
      ...
      DirGradientEnd=#707080
      DirGradientStart=#606070
      ...
      UseDirGradient=true

  Beware that QSettings sorts the entries alphabetically, so the start is after
  the end (how philosophical...).

  If there are graphics designers out there who want to tune this even more,
  please send me the results (preferably with a screenshot). But please
  remember that this effect should be kind of subtle, and the user should
  clearly see the difference between a large file and a directory full of tiny
  things that just don't get rendered because of the minimum tile size.

------------------------

- 2017-03-05 **New stable release: V1.3**

  It's about time to ship all those latest changes.


- 2017-02-27 Implemented [GitHub issue #30](https://github.com/shundhammer/qdirstat/issues/30):

  When hovering over a treemap tile, display the full path and the total size
  of that element in the status bar. When the hover ends (when the mouse cursor
  leaves the treemap tile), display the current selection again in the status
  bar.

- 2017-02-24 Improved logging: More secure and automatically log-rotating.

  QDirStat now uses its own log directory `/tmp/qdirstat-$USER` (where `$USER`
  is your user name; the numeric user ID is now only used if the user name
  cannot be obtained). It no longer keeps one single log file growing, but
  starts a new one each time it is started. 3 old logs are kept; any older ones
  are deleted.

  The permissions for that directory are set up in a pretty restrictive way
  (0700, i.e. `rwx------`) when it is created. If it already exists, QDirStat
  checks the owner and creates a new one with a random name if it is owned by
  anyone else than the user who started QDirStat.

      [sh @ balrog] ~ 68 % ls -ld /tmp/qdirstat-sh
      drwx------ 2 sh sh 4096 Feb 24 18:29 /tmp/qdirstat-sh
      [sh @ balrog] ~ 69 % ls -l /tmp/qdirstat-sh
      total 16
      -rw-rw-r-- 1 sh sh 2067 Feb 24 18:29 qdirstat-00.old
      -rw-rw-r-- 1 sh sh 2067 Feb 24 18:07 qdirstat-01.old
      -rw-rw-r-- 1 sh sh 2067 Feb 24 18:07 qdirstat-02.old
      -rw-rw-r-- 1 sh sh 2067 Feb 24 18:29 qdirstat.log


  For anybody regularly watching the log file this means they will now have to
  use `tail -F qdirstat.log` rather than `tail -f` since the latter does not
  realize when the file it watches is renamed and a new one is created under
  the same name.

- 2017-02-23 Fixed [GitHub issue #24](https://github.com/shundhammer/qdirstat/issues/24):

   During directory reading, subdirectories would get out of sync when opening
   a tree branch.

   It looks like QDirStat's tree display was a bit too dynamic for the concepts
   of the underlying Qt classes (QTreeView / QAbstractItemModel): During
   reading, QDirStat would sort the tree by the number of pending read
   jobs. That number is constantly changing, so the sort order would also
   constantly need to change. This is very hard to do properly with the
   limitations those underlying classes impose; basically it would require a
   reset of all the data the QTreeView keeps, thus making it forget things like
   its current scrollbar position or which tree branches were expanded or
   collapsed. That would make the user interface pretty much unusable.

   So the fix for this is to not sort by read jobs, but by directory names
   instead since they don't change all the time. The user can still sort by any
   other column, but that sort is a momentary thing that might become invalid
   moments later as data (accumulated sizes, number of child items) are
   updated. Everybody please notice that **this is a known limitation** and any
   complaints about that will flatly be rejected. The alternative would be to
   not allow the user to sort at all during directory reading, and that is
   certainly a lot less desirable.


- 2017-02-22

  - @flurbius contributed a patch to switch the main window layout from tree
    view above and treemap below to side-by-side (Menu _Treemap_ -> _Treemap as
    Side Panel_).

  - Added new document [GitHub-Workflow.md](https://github.com/shundhammer/qdirstat/blob/master/doc/GitHub-Workflow.md)
    explaining how to work with GitHub and Git to contribute to QDirStat.

- 2017-02-20 Locating files by type from the _File Type Statistics_ window

![Locating FilesWindow](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png)

  You can now locate files with a specific filename extension directly:

  - You select a file type (a filename extension) in the "File Type Statistics" window.

  - You click "Locate" or you double-click the item.

  - The "Locate Files" window opens.

  - You click a directory there.

  - In the main window, the branch for that directory opens, and all matching
    files are selected in the tree view and in the treemap.

  - You can now directly start cleanup actions for those files.

  See also [GitHub issue #48](https://github.com/shundhammer/qdirstat/issues/48).


- 2017-02-18 New document: [QDirStat for Servers](https://github.com/shundhammer/qdirstat/blob/master/doc/QDirStat-for-Servers.md)
  describing how to use QDirStat and the `qdirstat-cache-writer` script on
  headless (no X server, no X libs) servers.

- 2017-02-17 _File Type Statistics_ window merged to Git master

  Latest screenshot:

  ![File Type Statistics Window Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png)

  **Limitations:**

  Since filename extensions (suffixes) don't have as much semantics in
  Linux/Unix systems as they do in Windows, many files are categorized as
  "Other". This is a known limitation, but it's a limitation of the whole
  concept of using suffixes to categorize files by type. And no, checking file
  headers for magic byte sequences like the "file" command does is not an
  option here; QDirStat would have to do that for (at least) all the 30,000+
  files typically listed under the "Other" category. So we'll have to live with
  that limitation.

  Next thing to come: Locating files with a specific suffix from there.
  See [GitHub issue #48](https://github.com/shundhammer/qdirstat/issues/48).


- 2017-02-12 Working on a _File Type Statistics_ window

  People who know WinDirStat inevitably want that _File Type_ view in QDirStat,
  too. I was really reluctant to do that because I didn't quite see the point;
  in WinDirStat, it serves mostly as a legend to the treemap colors since they
  are constantly changing in WinDirStat: The file type that consumes most disk
  space always gets color #1, the next-most color #2 etc., so it depends which
  directory you scan what color each file type gets. In QDirStat, the colors
  are stable; they are predefined and configurable in the _MIME Type
  Categories_ configuration dialog.

  And as most of you probably know, filename extensions have a much stricter
  meaning in Windows than on Linux/Unix systems; Linux people get very creative
  when it comes to using dots in filenames. Sometimes those dots delimit a
  filename's extension (suffix) from its base name, sometimes they are used for
  entirely different purposes.

  Anyway, there was one user who was insistent enough to make me reconsider,
  and I did some experimenting this weekend, and now we have an (albeit still
  experimental) **File Type Statistics** view. So far, that code lives in a Git
  branch, but I think it will stabilize in the next one or two weeks, so I will
  merge it to Git master.


  See the whole discussion with more screenshots at
  [GitHub issue #45](https://github.com/shundhammer/qdirstat/issues/45)


- 2017-01-03 **New stable release: V1.2**

_See [DevHistory.md](https://github.com/shundhammer/qdirstat/blob/master/doc/DevHistory.md)
for older entries._


## History


This is just a rough summary. For more details, see [DevHistory.md](https://github.com/shundhammer/qdirstat/blob/master/doc/DevHistory.md)

- 2017-03-05 New stable release: V1.3

- 2017-01-03 New stable release: V1.2

- 2016-10-31 New stable release: V1.1-Pumpkin

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


## WinDirStat and QDirStat

I keep reading articles and user forum comments about QDirStat being a "nice
Linux port of WinDirStat". Well, nothing could be further from the truth:
**WinDirStat is a Windows port of KDirStat**, the predecessor of QDirStat.

So it's the other way round: **The Linux version was there first**, and
somebody liked it so much that he wrote a Windows version based on that
idea. That's a rare thing; usually people port Windows originals to Linux.

See also https://windirstat.net/background.html and the WinDirStat "About"
dialog.



## QDirStat and K4DirStat

K4DirStat is a port to KDE 4 / Qt 4 of my old KDE 3 / Qt 3 KDirStat. QDirStat is
independent of that; it is based on the old KDE 3 KDirStat directly.



## Motivation / Rant

After having used KDE since its early days (since about 1998), I didn't like
the direction any more that KDE has been taking. I loved KDE 1, KDE 2, KDE
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

- I don't like the fact that I can't simply put icons on my desktop any more --
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



## New Features

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

    ```
    [DirectoryTree]
    SlowUpdateMillisec = 3000
    ```



## Old Features

Features ported from the old KDirStat:

- Fast and efficient directory reading.

- Not crossing file system boundaries by default so you can see what eats up
  all the disk space on your root file system without getting distorted numbers
  due to all the other file systems that are mounted there. If you absolutely
  wish, you can use "Continue reading at mount point" from the context menu or
  from the "File" menu -- or configure QDirStat to always read across file
  systems.

- Efficent memory usage. A modern Linux root file system has well over 500,000
  objects (files, directories, symlinks, ...) and well over 40,000
  directories. This calls for minimalistic C++ objects to represent each one of
  them. QDirStat / KDirStat do their best to minimize that memory footprint.

- Hierarchical tree view that displays accumulated sums in each branch,
  together with a percent bar so you can see at a glimpse how the
  subdirectories compare with each other.

- All numbers displayed human readable -- e.g., 34.4 MB instead of 36116381
  Bytes.

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
    subdirectories from disk. If you or the users that machine use QDirStat
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


## Features that are Gone

(Compared with the old KDirStat)


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



## MacOS X Compatibility

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

_There be dragons._ ;-)


### Architecture maintainer wanted for QDirStat for MacOS X

If you are a developer with some prior C++ and Qt knowledge on the MacOS X
platform and you'd like to see QDirStat working there, please consider joining
the team.


## Windows Compatibility

None for the forseeable future.

Directory reading might be quite easy to replace for Windows; we don't have
that problem with devices and crossing filesystems on that platform.

But the cleanups might be a challenge, "move to trash" works completely
differently, and we'd need an installer for a Windows version.

So, for the time being, use [WinDirStat](https://windirstat.info/) instead.
WinDirStat is a close relative to the KDirStat family anyway; the author had
liked KDirStat on Linux so much that he decided to write a Windows clone and
called it WinDirStat.


## Ready-made Packages

### openSUSE / SUSE Linux Enterprise

QDirStat packages for:

- openSUSE Tumbleweed
- openSUSE Leap 42.2
- openSUSE Leap 42.1
- openSUSE 13.2
- SUSE Linux Enterprise (SLE) 12 SP2
- SUSE Linux Enterprise (SLE) 12 SP1
- SUSE Linux Enterprise (SLE) 12

Download page for the [**latest stable release**](https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-stable&package=qdirstat)

Download page for the [**current development version** (git master)](https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-git&package=qdirstat)

Since this version is in development, it may be not quite as stable and
reliable as the latest official stable release, although the QDirStat
developers try their best to keep it as stable as possible.


### Ubuntu

See Nathan Rennie-Waldock's
[**QDirStat PPA**](https://launchpad.net/~nathan-renniewaldock/+archive/ubuntu/qdirstat)


## Building

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


## Contributing

See file [Contributing.md](https://github.com/shundhammer/qdirstat/blob/master/doc/Contributing.md)
and [GitHub-Workflow.md](https://github.com/shundhammer/qdirstat/blob/master/doc/GitHub-Workflow.md)


## To Do

See file [TODO.md](https://github.com/shundhammer/qdirstat/blob/master/doc/TODO.md)


## Troubleshooting

### Can't Move a Directory to Trash

See file [Troubleshooting.md](https://github.com/shundhammer/qdirstat/blob/master/doc/Troubleshooting.md)


## Reference

### Original KDirStat

Home page: http://kdirstat.sourceforge.net/

Sources: https://github.com/shundhammer/kdirstat


### K4Dirstat

Home page: https://bitbucket.org/jeromerobert/k4dirstat/wiki/Home

Sources: https://bitbucket.org/jeromerobert/k4dirstat/src


### WinDirStat (for Windows)

Home page: https://windirstat.info/


### XDG Trash Spec

http://standards.freedesktop.org/trash-spec/trashspec-1.0.html


## Reviews

### YouTube

[Spatry: _Quick Look: QDirStat_](https://www.youtube.com/watch?v=ysm4-x_5ftI)


## Misc

http://moo.nac.uci.edu/~hjm/HOWTO_move_data.html#qdirstat

http://moo.nac.uci.edu/~hjm/kdirstat/kdirstat-for-clusters.html