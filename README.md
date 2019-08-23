# QDirStat
<img src="https://github.com/shundhammer/qdirstat/blob/master/src/icons/qdirstat.svg" height="64">

Qt-based directory statistics: KDirStat without any KDE -- from the author of
the original KDirStat.

(c) 2015-2019 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2019-08-23


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

See section [_New Features_](#new-features) for more details.



## Screenshot

[<img width="900" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-main-win.png)

_Main window screenshot - notice the multi-selection in the tree and the treemap_


## Table of Contents

1. [Screenshot](#screenshot)
1. [More Screenshots](#more-screenshots)
1. [Latest Stable Release](#latest-stable-release)
1. [Latest News](#latest-news)
1. [History](#history)
1. [Related Software](#related-software)
   1. [WinDirStat and QDirStat](#windirstat-and-qdirstat)
   1. [QDirStat and K4DirStat](#qdirstat-and-k4dirstat)
   1. [Other](#other)
1. [Motivation / Rant](#motivation--rant)
1. [Features](#features)
   1. [New Features](#new-features)
   1. [Old Features](#old-features)
   1. [Features that are Gone (Compared to the Old KDirStat)](#features-that-are-gone)
1. [MacOS X Compatibility](#macos-x-compatibility)
1. [Windows Compatibility](#windows-compatibility)
1. [Ready-made Packages](#ready-made-packages)
1. [Building](#building)
   1. [Build Environment](#build-environment)
   1. [Compiling](#compiling)
   1. [Installing](#installing)
   1. [Install to a Custom Directory](#install-to-a-custom-directory)
1. [Contributing](#contributing)
1. [To Do](#to-do)
1. [Troubleshooting](#troubleshooting)
   1. [Can't Move a Directory to Trash](#cant-move-a-directory-to-trash)
1. [Reference](#reference)
1. [Packaging Status](#packaging-status)
1. [Donate](#donate)


## More Screenshots


[<img align="top" height="237" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-type-stats.png)
[<img align="top" height="169" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-cleanup-output.png)
[<img align="top" height="170" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png"  >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-column-config.png)

[<img width="900" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-locating-file-types.png)


[<img align="top" width="220" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-cleanups.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-cleanups.png)
[<img align="top" width="220" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-mime.png"    >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-mime.png)
[<img align="top" width="220" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-exclude.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-exclude.png)
[<img align="top" width="220" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-general.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-general.png)


[<img align=top height="300" src="https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram.png"        >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram.png)
[<img align=top height="300" src="https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram-options.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-histogram-options.png)

[<img height="262" src="https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-stats-help.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-stats-help.png)

_Full-size images and descriptions on the [Screenshots Page](https://github.com/shundhammer/qdirstat/blob/master/screenshots/Screenshots.md)_


-----------------------

## Donate

QDirStat is Free Open Source Software. You are not required to pay anything.
Donations are most welcome, of course.

Donate via PayPal (freely select the amount to donate):

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W)


## Latest Stable Release

**QDirStat V1.6**

See the [release announcement](https://github.com/shundhammer/qdirstat/releases).

Download installable binary packages for various Linux distributions here:
[Ready-made packages](#ready-made-packages)


## Latest News

- 2019-08-22

  - Improved handling for directories that could not be read.

    When you run QDirStat as a normal (non-root) user on system directories, it
    is very common that you get a "permission denied" error for directories
    that contain sensitive information. Such a directory gets a folder icon
    with a little lock to indicate that it's locked for you; there is no way to
    find out how much disk space it consumes.

    Now, such directories are also displayed in a special color (for now bright
    red) in the tree. All their parents are now displayed in another (slightly
    less obtrusive) color to indicate that you probably don't see the complete
    disk space information for that subdirectory.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-light.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-err-dirs-light.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-dark.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-err-dirs-dark.png)

    In those examples, The `/etc/ssl/private` directory does not have read
    permissions for non-root users; note the icon with the lock and the red
    text color.

    When you don't open that branch in the tree, you might not realize this;
    it's also in the log file, but most users don't read that. So its parent
    directories `/etc/ssl` and `/etc` are also displayed in a different color:
    Dark red in the light widget theme and yellow in the dark widget theme.

    In addition to that, the parent directories now show a `>` prefix for the
    size, number of items, number of files, number of subdirs fields to
    indicate that there may be more. This is a hint (not only, but also) for
    color blind users.


- 2019-08-12

  - Toned down over-information to reduce clutter:

    - No longer showing column "Oldest File" in layout L2 by default.

      Of course, you can always re-enable this if you like:
      Just switch to layout L2, right-click on the column header to open the
      column context menu, then "Hidden Columns" -> "Show Column 'Oldest File'".

    - Removed "Oldest File" from the details panel for directories:

      I found that this does not contribute any useful information here, yet it
      adds to the screen clutter. Information about the oldest file in a
      subtree is useful to drill down deeper into the subtree to find it, so
      the tree view is much better suited for that; having that information in
      the details panel as well does not add any value for that procedure.


- 2019-08-03

  - Fixed text color in histogram in dark widget themes
    [(GitHub Issue #117)](https://github.com/shundhammer/qdirstat/issues/117).

- 2019-08-01

  - Added a column "Oldest File" that shows the timestamp (the mtime) of the
    oldest file in a subtree.  This is strictly about files; directories,
    symlinks and special files (block or character devices, FIFOs etc.) are
    ignored for this.

    This is the first simple approach for
    [GitHub Issue #118](https://github.com/shundhammer/qdirstat/issues/118).
    (Click for screenshot)

    This already helped me to spot some old cruft in my home directory; some
    dot directories from ancient versions of flashplayer and whatnot. This
    is more helpful than I initially thought.

  - Added an entry "Oldest File" that does the same in the details panel for
    directories and packages.


- 2019-07-22 **New stable release: V1.6**

  - Performance improvement while reading directories: On average 25% faster on
    normal (non-SSD) disks

    See also this article: [Linux Disk Usage Tools Compared: QDirStat
    vs. K4DirStat vs. Baobab vs. Filelight vs. ncdu including
    benchmarks](https://github.com/shundhammer/qdirstat/issues/97)


  - Vast performance improvement for huge directories (with 100.000 entries or
    more in a single directory) in the tree view: There is now instant response
    for jumping from the first to the last item, dragging the scroll bar or
    using the mouse wheel.

  - New **packages view**:
    QDirStat can now visualize the file lists of installed packages:

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-details.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-pkg-details.png)

    I.e. files are now grouped by the package they belong to, and in each
    subtree only the files that belong to the package are displayed: In this
    example, in `/usr/bin` only the `chromium-browser` binary is displayed, not
    all the other files in `/usr/bin`. This is intentional.

    You can display all installed packages with their file lists (but that
    takes a while), or you can select just a subset. Use Menu _File_ -> _Show
    Installed Packages_ or start QDirStat with a `pkg:/` command line argument.

    As with the other package manager related features, this is supported for
    all Linux distributions that use one of _dpkg_, _rpm_ or _pacman_ (or any
    higher-level package manager based on any of those like _apt_, _zypper_
    etc.).

    More details at [Pkg-View.md](doc/Pkg-View.md).


  - New **unpackaged files view**:

    QDirStat can now visualize the files in system directories that are not
    packaged, i.e. that are not part of any file list of any installed software
    package.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-usr-share-qt5.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-unpkg-usr-share-qt5.png)

    This can be useful to track down problems after package upgrades or after
    manually installing software with `sudo make install`.

    This is supported for all Linux distributions that use _dpkg_ or _rpm_ (or
    any higher-level package manager based on any of those like _apt_, _zypper_
    etc.).

    More details at [Unpkg-View.md](doc/Unpkg-View.md).


  - New standard cleanup: _Check File Type_. This uses the `file` command to
    find out more detailed information what exactly a file is and displays it
    in the cleanup output window.

    More details, screenshots and how to get it if you already have an existing
    QDirStat cleanup configuration file (i.e. if you used QDirStat before):
    [GitHub Issue #102](https://github.com/shundhammer/qdirstat/issues/102).

  - Implemented [GitHub Issue #90](https://github.com/shundhammer/qdirstat/issues/90):
    Support excluding directories containing a file with a specific name or pattern.

    Similar to some backup tools, you can now specify an exclude rule that lets
    you exclude a directory that contains a file like `.nobackup` or
    `.qdirstatexclude`.

  - Greatly improved the man page; see `man 1 qdirstat`.

  - Some bug fixes.

  See also the [release announcement](https://github.com/shundhammer/qdirstat/releases/tag/1.6).

--------------------------------------------------


- 2019-07-18

  - Greatly improved the man page; see `man 1 qdirstat`.

    It started with the Debian maintainer of QDirStat asking for a man page
    because that's a standard requirement for Debian packages, and he even
    wrote the initial one; thanks again, Patrick!.

    That initial man page was very concise, and as QDirStat keeps evolving, not
    only was there an increasing number of command line options that was not
    documented in the man page (but of course when invoking the program with
    the `--help` command line option), but it also didn't explain much beyond
    the command line arguments.

    Now it contains not only the latest set of command line arguments including
    syntax and meaning of `pkg:/` and `unpkg:/` URLs, but also some general
    information what the program is (including what the treemap is) and how to
    use it.


- 2019-07-08

  New **unpackaged files view**:

  QDirStat can now visualize the files in system directories that are not
  packaged, i.e. that are not part of any file list of any installed software
  package.

  This reads the complete file lists first (i.e. all file lists from all
  installed packages), then reads the requested directory as usual and puts the
  files that are packaged in a special branch `<Ignored>` in the tree view.

  Those files are _not_ displayed in the treemap, i.e. the treemap now only
  contains unpackaged files.

  [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-usr-share-qt5.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-unpkg-usr-share-qt5.png)

  **What is this Good For?**

  - Recovering a wrecked system after botched upgrades

  - Recovering from too much `sudo make install`

  - Fixing packaging problems, e.g. find leftover shared libs that tend to get in
    the way

  - QA for distro release managers, QA engineers, PMs

  - Satisfying user curiosity

  - _Because we can!_  ;-)

  More details at [Unpkg-View.md](doc/Unpkg-View.md).

  Comments and questions are welcome at [GitHub Issue #110](https://github.com/shundhammer/qdirstat/issues/110).

  If you have a genuine problem with the new feature, please open a separate
  issue so it can be tracked properly.



- 2019-06-16

  - Added [documentation for the _pkg-tools_ scripts](scripts/pkg-tools/README.md).

    In short, they can be used to see unpackaged files on a system that uses
    one of the supported package managers (_dpkg_, _rpm_, _pacman_), but it's a
    bit of a clunky solution, so this is meant for advanced users only.

    ~~With a little bit of luck, there _may_ be a better solution forthcoming
    with real integration into QDirStat's GUI.~~

    _Update 2019-07-08: This real integration into the GUI is now available._


- 2019-05-12

  - New standard cleanup: _Check File Type_. This uses the `file` command to
    find out more detailed information what exactly a file is and displays it
    in the normal cleanup output window (the orange text is the interesting
    part).

    More details and screenshots at [GitHub Issue #102](https://github.com/shundhammer/qdirstat/issues/102).
    Notice that you can also add this to any older version of QDirStat.


- 2019-05-11

  - Implemented the single-command call to get all file lists for all installed
    packages for _rpm_ as well.

    Now it's only _pacman_ that still needs a separate external command for
    each package. If anybody knows how to do this for _pacman_ as well, please
    let me know.

  - Another drastic performance improvement in the packages view:

    Now down to **6.5 seconds** on my system from initially 180 seconds for
    getting all file lists for all 2400 installed packages.

    | sec   |  Version   | Description                                                         |
    |------:|------------|---------------------------------------------------------------------|
    | 180.0 | ce3e793298 | First pkg view; sequential separate `dpkg -L` calls                 |
    |  53.4 | 68038f0525 | Separate `dpkg -L` calls in multiple background processes           |
    |  38.5 | ce54879a48 | Single `dpkg -S "*"` call to get all file lists for all pkg at once |
    |  37.7 | 45b0a7a941 | Use cache for `lstat()` syscalls                                    |
    |  24.6 | c50e9a7686 | Use slower update timer for tree view while reading                 |
    |   6.5 | a1043a20fb | Keep tree collapsed during reading                                  |

    (Much) more details at [GitHub Issue #101](https://github.com/shundhammer/qdirstat/issues/101).


- 2019-05-10

  Drastic performance improvements in the packages view:

  - When reading many (configurable; right now 200) package lists, QDirStat no
    longer fires off a separate background process (6 in parallel at any given
    time) for each external command (`dpkg-query --listfiles` or `rpm -ql` or
    `pacman -Qlp`) and collects their output.

    Rather, it now tries to use a single external command that can return all
    file lists for all packages at once. It builds a cache from that and uses
    it to build the internal tree as it processes the read jobs for each
    package one by one.

    Right now this works for _dpkg_ (`dpkg -S "*"`). Experiments show that it
    will also work for _rpm_ (that's on the _to do_ list).

    But there does not seem to be an equivalent command for _pacman_; it looks
    as if _pacman_ can only return a file list for a single package or a list
    of all installed packages, but without any reference what package each file
    in that list belongs to. _If anybody knows, please contact me._

  - Now caching the result of `lstat()` syscalls for directories in the package
    view since most packages share common system directories like `/usr`,
    `/usr/bin`, `/usr/share` etc.; `lstat()` is an expensive affair, and even
    just avoiding to switch from user space to kernel space and back that often
    is a speed improvement.

  - Reduced the display update inverval in the packages view. While reading
    package information, there is not all that much to see anyway. Yet
    constantly recalculating the column widths to make sure they fit their
    content is expensive. This is now done just every 5 seconds, not 3 times a
    second.

    There is even a noticeable difference when using the L1 layout and a
    smaller window size so there is less content to take care of.

  The net effect of all this performance tuning is that on my machine (Xubuntu
  18.04 LTS (i.e. _dpkg_) with ~2400 packages) reading all packages with all
  their file lists is now down to under 30 seconds from formerly 90 to 120 (it
  varied wildly).


- 2019-05-09

  - Now no longer showing a directory's own size (the size of the directory
    node, not of any file contained in the directory) in the packages view:
    This distorted both the treemap and the total sums. Directories are
    typically shared between a lot of packages, so it does not make very much
    sense to add the size of a very common directory like /usr/bin or /usr/lib
    to the total size of each package that owns a file in any of them, thus
    accounting for those directories many times. On my system, /usr/bin is
    shared between 454 packages, so its 68 kB would be added up 454 times, thus
    adding up to 30 MB.

    In the treemap that meant that packages that install only one or two very
    small files (symlinks!) to a very common directory would show a lot of
    empty space; the 56 byte (or so) symlink would be completely dwarfed by the
    directory it is in, even if the directory only has 4 kB. A lot of such lone
    small files each in a separate directory meant a whole lot of empty space
    in the treemap, thus making packages very hard to compare against each
    other.

    Of course this means that the size of the directory nodes is now completely
    missing in the toplevel total sum (the Pkg:/ node in the tree), but that is
    much less distorting than multiplying each of those directory node sizes by
    the number of packages that have files in each of them.

  - No longer regarding the directory's own mtime (modification time) in the
    packages view: The latest overall mtime in a subtree cascades upward, so
    any change in any subtree would affect the latest mtime in the higher tree
    levels. But if that change was caused by something outside of the current
    view, this is irrelevant. So if you want to know the latest mtime anywhere
    in package _foo_ which includes the /usr/bin/foo command, it is irrelevant
    if one hour ago you installed or updated some other package which also
    installed some other commands to /usr/bin; you don't want that latest mtime
    of the /usr/bin directory to affect the display of every package that has a
    file in /usr/bin.


- 2019-05-04

  New **packages view**:
  QDirStat can now visualize the file lists of installed packages:

  [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-details.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-pkg-details.png)

  I.e. files are now grouped by the package they belong to, and in each subtree
  only the files that belong to the package are displayed: In this example, in
  `/usr/bin` only the `chromium-browser` binary is displayed, not all the other
  files in `/usr/bin`. This is intentional.

  You can display all installed packages with their file lists (but that takes
  a while), or you can select just a subset. Use Menu _File_ -> _Show Installed
  Packages_ or start QDirStat with a `pkg:/` command line argument.

  As with the other package manager related features, this is supported for all
  Linux distributions that use one of _dpkg_, _rpm_ or _pacman_ (or any
  higher-level package manager based on any of those like _apt_, _zypper_ etc.).

  More details at [Pkg-View.md](doc/Pkg-View.md).

  Comments and questions are welcome at [GitHub Issue #100](https://github.com/shundhammer/qdirstat/issues/100).

  If you have a genuine problem with the new feature, please open a separate
  issue so it can be tracked properly.


- 2019-04-12

  - **Performance improvement** while reading directories: **25% faster** on my
    /work directory with an ext4 filesystem with 230 GB / 216k items on a
    rotational (non-SSD) disk; with cleared caches now 24.5 sec average
    compared to previously 32.5 sec average (with filled caches down to 1.5
    from previously 2.0 sec).

    It now uses `fstatat()` (instead of `lstat()`) which accepts the file
    descriptor of an open directory, so glibc and the kernel save the time
    previously needed for parsing the path and locating the directory; that
    part will also bring some speed improvement for SSDs.

    In addition to that, the entries for each directory are now sorted by i-no
    before calling `fstatat()` so the kernel can now read the i-nodes on disk
    in sequential order, thus saving disk seek times. SSD users will not
    benefit from that since there are no disk seek times on an SSD.

  - Now using the name as the secondary sort field in the tree view if the
    primary sort field (usually the size) is equal for two items.

  - Vast performance improvement for huge directories (with 100.000 entries or
    more in a single directory) in the tree view: There is now instant response
    for jumping from the first to the last item, dragging the scroll bar or
    using the mouse wheel.

    It had turned out that by default the underlying QTreeView widget queries
    each item in turn how tall it wants to become (using the `sizeHint()`)
    which in turn had to query the font for each one for its metrics.

    QDirStat now sets the QTreeView's `uniformRowHeights` flag to indicate that
    all rows have the same height, so this only needs to be done for the first
    one, and the result is simply multiplied by the number of items.

    Amazingly enough it was not sorting the items (which is what comes to mind
    when there is such a performance bottleneck), no, it was someting as
    mundane as the widget having to figure out the proportions of its scroll
    bar slider vs. the scroll bar overall length. And for that, it needs to
    know the number of items (which is simple) and the height of each one
    (which was not).

    The reason why the widget does that is because each item might have a
    different font or a different icon, and then each item might have a
    different height. That `uniformRowHeights` flag tells it that this is not
    the case.


- 2019-04-06

  - New article _Linux Disk Usage Tools Compared: QDirStat vs. K4DirStat
    vs. Baobab vs. Filelight vs. ncdu_ including benchmarks:
    [GitHub Issue #97](https://github.com/shundhammer/qdirstat/issues/97)

    (written in the GitHub issue tracker so users can join the discussion)


- 2019-04-05

  - Performance boost for huge directories (with 100.000 entries or more in a single
    directory):

    A routine that counts the direct children of a directory now uses a cached
    value for each directory so it does not have to be recalculated over and
    over again even if nothing changed. Amazingly enough, this little thing had
    turned out to be the performance bottleneck that had made QDirStat
    prohibitively slow for such directories. It was not the sorting of the
    entries (the sort order was always cached), no, the problem was something
    as trivial as counting the children on the current level of the tree view.

    Of course, a directory that contains 100.000 entries in a single level
    still has quite some performance impact, but at least now it's
    tolerable. This was tested with up to 500.000 entries in a single directory
    (there is now a script that can create such a directory in the test/util
    directory of the source tree).

    Hint: Avoid dragging the vertical scroll bar of the tree view in such a
    directory; better use keyboard commands such as the _Home_ or the _End_
    key. The scroll bar will make the underlying Qt widget go through every
    single entry in turn, and that will take a while (it will eventually become
    responsive again, though).

    _Update 2019-04-12: This is now no longer an issue; using
    `uniformRowHeights` fixed that._


  - Implemented [GitHub Issue #90](https://github.com/shundhammer/qdirstat/issues/90):
    Support excluding directories containing a file with a specific name or pattern.

    Similar to some backup tools, you can now specify an exclude rule that lets
    you exclude a directory that contains a file like `.nobackup` or
    `.qdirstatexclude`. The exclude rule configuration now has a new option for
    that:

    [<img width="300" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-exclude.png" >](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-exclude.png)

    This makes it possible to reuse such files that are there anyway for some
    other tool and get a good idea how large the resulting backup will become.

    Since this change required some refactoring in a quite sensitive part
    (reading the directories), please watch out for possible bugs that this might
    have introduced and report it if you find something.


- 2018-11-07 **New stable release: V1.5**


_See [DevHistory.md](doc/DevHistory.md)
for older entries._


## History


This is just a rough summary. For more details, see [DevHistory.md](doc/DevHistory.md)

- 2018-11-07 New stable release: V1.5

- 2017-06-04 New stable release: V1.4

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


## Related Software

### WinDirStat and QDirStat

I keep reading articles and user forum comments about QDirStat being a "nice
Linux port of WinDirStat". Well, nothing could be further from the truth:
**WinDirStat is a Windows port of KDirStat**, the predecessor of QDirStat.

So it's the other way round: **The Linux version was there first**, and
somebody liked it so much that he wrote a Windows version based on that
idea. That's a rare thing; usually people port Windows originals to Linux.

See also https://windirstat.net/background.html and the WinDirStat "About"
dialog.



### QDirStat and K4DirStat

K4DirStat is a port to KDE 4 / Qt 4 of my old KDE 3 / Qt 3 KDirStat. QDirStat is
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


## Motivation / Rant

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
  explained it properly, please have a lot at it.

- Packages view: Show installed packages and their files in the tree. Supported
  for all Linux distributions using any of _dpkg_, _rpm_, _pacman_ as their
  low-level package manager or any higher-level package manager like _apt_,
  _zypper_ etc.; more details at [Pkg-View.md](doc/Pkg-View.md).

- Unpackaged files view: Show a directory tree, but ignore all files that
  belong to an installed software package. Those ignored files are displayed in
  a special branch _<Ignored>_ in the tree view, and they are not displayed at
  all in the treemap. This may be useful to find files that were manually
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

    ```
    [DirectoryTree]
    SlowUpdateMillisec = 3000
    ```



### Old Features

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

None for the forseeable future. Use [WinDirStat](https://windirstat.info/).

<details>
Directory reading might be quite easy to replace for Windows; we don't have
that problem with devices and crossing filesystems on that platform.

But the cleanups might be a challenge, "move to trash" works completely
differently, and we'd need an installer for a Windows version.

So, for the time being, use [WinDirStat](https://windirstat.info/) instead.
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

- Recent releases (Artful Aardvark (17.10) or later):

  https://packages.ubuntu.com/search?keywords=qdirstat&searchon=names

- Older releases (Pre-17.10):

  Use Nathan Rennie-Waldock's
  [**QDirStat PPA**](https://launchpad.net/~nathan-renniewaldock/+archive/ubuntu/qdirstat)



### Debian

https://packages.debian.org/search?keywords=qdirstat


### Fedora

https://apps.fedoraproject.org/packages/qdirstat/builds/


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


## To Do

See file [TODO.md](doc/TODO.md)


## Troubleshooting

### Can't Move a Directory to Trash

See file [Troubleshooting.md](doc/Troubleshooting.md)


## Reference

- Original [KDirStat](http://kdirstat.sourceforge.net/)  ([source code](https://github.com/shundhammer/kdirstat))
- [K4Dirstat](https://bitbucket.org/jeromerobert/k4dirstat/wiki/Home) ([source code](https://bitbucket.org/jeromerobert/k4dirstat/src))
- [WinDirStat](https://windirstat.info/) (for Windows)
- [Disk Usage Tools Compared: QDirStat vs. K4DirStat vs. Baobab vs. Filelight vs. ncdu](https://github.com/shundhammer/qdirstat/wiki/disk-usage-tools-compared) (including benchmarks)
- [XDG Trash Spec](http://standards.freedesktop.org/trash-spec/trashspec-1.0.html)
- [Spatry's QDirStat Review on YouTube](https://www.youtube.com/watch?v=ysm4-x_5ftI)
- Articles by Harry Mangalam (University of California, Irvine)
  - [QDirStat](http://moo.nac.uci.edu/~hjm/HOWTO_move_data.html#qdirstat)
  - [KDirStat for Clusters](http://moo.nac.uci.edu/~hjm/kdirstat/kdirstat-for-clusters.html)

Of course, don't forget to check out the [doc directory](doc/).


## Packaging Status

Repology: QDirStat versions in Linux / BSD distributions:

[![Repology](https://repology.org/badge/tiny-repos/qdirstat.svg)](https://repology.org/metapackage/qdirstat/versions)

(click for details)


## Donate

QDirStat is Free Open Source Software. You are not required to pay anything.
Donations are most welcome, of course.

Donate via PayPal (freely select the amount to donate):

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W)
