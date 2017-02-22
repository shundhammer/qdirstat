# QDirStat

Qt-based directory statistics: KDirStat without any KDE -- from the author of
the original KDirStat.

(c) 2015-2017 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2017-02-22


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

[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png" width="1241">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-main-win.png)

_Main window screenshot - notice the multi-selection in the tree and the treemap_


[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png" height="237">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-type-stats.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png" height="169">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-output.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png" height="170">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-column-config.png)

[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png" height="337">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-locating-file-types.png)


[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-config.png" height="245">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-config.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-mime-config.png" height="245">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-mime-config.png)
[<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-exclude-rules-config.png" height="245">]
(https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-exclude-rules-config.png)


_Full-size images and descriptions on the [Screenshots Page]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/README.md)_


-----------------------



## Current Development Status

**Latest stable release: V1.2**

- 2017-02-22 @flurbius contributed a patch to switch the main window layout
  from tree view above and treemap below to side-by-side (Menu _Treemap_ ->
  _Treemap as Side Panel_).

- 2017-02-20 Locating files by type from the _File Type Statistics_ window

![Locating FilesWindow]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png)

  You can now locate files with a specific filename extension directly:

  - You select a file type (a filename extension) in the "File Type Statistics" window.

  - You click "Locate" or you double-click the item.

  - The "Locate Files" window opens.

  - You click a directory there.

  - In the main window, the branch for that directory opens, and all matching
    files are selected in the tree view and in the treemap.

  - You can now directly start cleanup actions for those files.

  See also [GitHub issue #48](https://github.com/shundhammer/qdirstat/issues/48).


- 2017-02-18 New document: [QDirStat for Servers]
(https://github.com/shundhammer/qdirstat/blob/master/doc/QDirStat-for-Servers.md)
  describing how to use QDirStat and the `qdirstat-cache-writer` script on
  headless (no X server, no X libs) servers.

- 2017-02-17 _File Type Statistics_ window merged to Git master

  Latest screenshot:

  ![File Type Statistics Window Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png)

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

  _Upgrading to this release is highly recommended for Btrfs users:_

  If you used QDirStat to scan a Btrfs partition, any subvolumes of that
  partition were not scanned (see
  [GitHub issue #39](https://github.com/shundhammer/qdirstat/issues/39)).

  Btrfs subvolumes were treated just like ordinary mount points (which, to all
  intents and purposes, they are). So you might have wondered why the _df_
  command shows your 40 GB root filesystem as 97% full, yet QDirStat shows only
  about 7 GB. The rest might be hidden in subvolumes.

  QDirStat stops reading at mount points - which only makes sense because
  normally you want to know what eats up the disk space on that one partition
  that is filling up, not on any others like /home that are mounted
  there. Unfortunately, a Btrfs subvolume is also just another mount point, and
  QDirStat would stop reading there, too - at /var/log, at /var/spool, at
  /var/lib/libvirt etc.; a typical Btrfs root filesystem has about a dozen
  subvolumes, and all files in them were disregarded by QDirStat.

  This is now fixed: Despite Btrfs doing its best to make this difficult (using
  one single privileged system call for all its functionality, including simple
  info calls), QDirStat now detects if a mount point is a Btrfs subvolume and
  continues reading if it is. QDirStat uses /proc/mounts (or, if this is not
  available, /etc/mtab) to find this out.

  This is fixed in the _qdirstat-cache-writer_ script, too.


- 2016-12-11 Bernhard Walle contributed some patches for MacOS X support.
  Thanks, Bernhard!

- 2016-12-09 Fixed Perl (_qdirstat-cache-writer_) part of
  [GitHub issue #39](https://github.com/shundhammer/qdirstat/issues/39):
  QDirStat doesn't scan Btrfs subvolumes

  The _qdirstat-cache-writer_ script now also checks the device names of a
  mount point and its parent directory, not only their major/minor device
  numbers; so now it will not stop at Btrfs subvolumes while scanning.

  That script uses a more simplistic approach than QDirStat itself: It invokes
  the _df_ command with that path and parses its output. If the path contains
  very weird special characters, this may fail, in which case that directory
  (which at that point is already known to have a different device major/minor
  number than its parent) is considered a filesystem boundary, and that branch
  is not scanned.


- 2016-12-08 Fixed C++ (QDirStat binary) part of
  [GitHub issue #39](https://github.com/shundhammer/qdirstat/issues/39):
  QDirStat doesn't scan Btrfs subvolumes

  This was a bit of a challenge since the relevant Btrfs commands to obtain any
  useful information about subvolumes all require root privileges, and I really
  wouldn't want to scare users off by prompting for a _sudo_ password. QDirStat
  now fetches the information from /proc/mounts (or /etc/mtab if /proc/mounts
  is unavailable) and does some heuristics (which are not completely fool
  proof) to check if a potential mount point is still on the same device. That
  means that it will no longer treat a Btrfs subvolume as an ordinary mount
  point where it stops reading by default, but it just continues. On the other
  hand, another Btrfs mounted into the current file system is of course treated
  as a normal mount point. See also the corresponding
  [GitHub issue](https://github.com/shundhammer/qdirstat/issues/39)
  for details.

  The Perl _qdirstat-cache-writer_ still has the old behaviour, i.e. it still
  stops at a subvolume mount point. This will be addressed next.


- 2016-12-07 Fixed [GitHub issue #40](https://github.com/shundhammer/qdirstat/issues/40):
  Crash without useful error message when no display available

  When ssh'ing without -X to a remote machine and starting QDirStat there, it
  would just dump core and not issue any meaningful message. The fatal error
  message was only in the log file:

  `<ERROR>   :0 ():  QXcbConnection: Could not connect to display`

  Now this message is also repeated on stderr, and in this particular case
  ("Could not connect to display"), it does not dump core any more, but just
  exits with error code 1.


- 2016-10-31 (Halloween) New stable release: V1.1-Pumpkin


_See file DevHistory.md for older entries:_

https://github.com/shundhammer/qdirstat/blob/master/doc/DevHistory.md



## Motivation / Rant

After having used KDE since its early days (since about 1998), I don't like the
direction any more that KDE is taking.  I loved KDE 1, KDE 2, KDE 3. When KDE 4
came along, it took me a long time to try to adopt it, and when I did, I moved
back to KDE 3 after a short while, then tried again with the next release,
moved back again -- several times.

I really tried to like it, but whenever I thought I tamed it to meet my
requirements, a new version came along that introduced yet another annoyance.

To name a few:

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

Now the next generation KDE is arriving, _Plasma 5_. When I was force-migrated
to it at work with the _SUSE Tumbleweed_ rolling release, the experience was so
bad that I moved to the _Xfce_ Desktop.

Now every time I start my own KDirStat, it starts about a dozen KDE processes
along with it -- processes that it needs only for minor things like loading
icons or translations. I really don't need or want that.

Time to make KDirStat self-sufficient; it never used that much of all the KDE
infrastructure anyway. Time to make a pure Qt-based and self-sufficient
QDirStat.

And while I was at it, time to add some features that I had wanted for a long
time, yet I could never get myself to start working on:

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
  world understood (I was not among them), later _CMake_ which is little
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

- QDirStat now has its own log file. It now logs to /tmp/qdirstat-$UID.log,
  which for most Linux home users (with only one user account on the machine)
  is typically /tmp/qdirstat-1000.log . No more messages on stdout that either
  clobber the shell you started the program from or that simply go missing.

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
- SUSE Linux Enterprise (SLE) 12 SP1
- SUSE Linux Enterprise (SLE) 12

Download page for the [**latest stable release**]
(https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-stable&package=qdirstat)

Download page for the [**current development version** (git master)]
(https://software.opensuse.org/download/package?project=home:shundhammer:qdirstat-git&package=qdirstat)

Since this version is in development, it may be not quite as stable and
reliable as the latest official stable release, although the QDirStat
developers try their best to keep it as stable as possible.


### Ubuntu

See Nathan Rennie-Waldock's
[**QDirStat PPA**]
(https://launchpad.net/~nathan-renniewaldock/+archive/ubuntu/qdirstat)


## Building

### Build Environment

Make sure you have a working Qt 5 build environment installed. This includes:

- C++ compiler (gcc recommended)
- Qt 5 runtime environment
- Qt 5 header files
- libz (compression lib) runtime and header file

If anything doesn't work, first of all **make sure you can build any of the
simple examples supplied with Qt**, e.g. the
[calculator example]
(http://doc.qt.io/qt-5/qtwidgets-widgets-calculator-example.html).


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

See file [Contributing.md]
(https://github.com/shundhammer/qdirstat/blob/master/doc/Contributing.md)
and [GitHub-Workflow.md]
(https://github.com/shundhammer/qdirstat/blob/master/doc/GitHub-Workflow.md)


## To Do

See file [TODO.md]
(https://github.com/shundhammer/qdirstat/blob/master/doc/TODO.md)


## Troubleshooting

### Can't Move a Directory to Trash

See file [Troubleshooting.md]
(https://github.com/shundhammer/qdirstat/blob/master/doc/Troubleshooting.md)


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

[Spatry: _Quick Look: QDirStat_]
(https://www.youtube.com/watch?v=ysm4-x_5ftI)


## Misc

http://moo.nac.uci.edu/~hjm/HOWTO_move_data.html#qdirstat

http://moo.nac.uci.edu/~hjm/kdirstat/kdirstat-for-clusters.html