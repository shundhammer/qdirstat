# QDirStat Development History

This is more verbose than a traditional change log, thus the unusual name of
this file.

See the README.md file for the latest news:

https://github.com/shundhammer/qdirstat/blob/master/README.md


## QDirStat History


- 2022-06-28

  - A middle click in the treemap now toggles the highlight of that tile's
    parent and ancestors on or off. Previously, it always kept the highlight,
    and you had to click outside the highlighted area to get rid of the
    highlight again.

  - Added packed Git archives ("pack-*.pack") to the "Compressed Archives" MIME
    category, i.e., they appear now in green in the treemap, no longer in the
    "I don't know what that thing is" grey.

  - No longer drawing additional lines in the treemap by default if there is
    low contrast between individual treemap tiles; that only makes the treemap
    display uglier for little benefit. You can still switch it on with the
    `EnforceContrast` setting in the config file
    (`~/.config/QDirStat/QDirStat.conf`).


- 2022-05-31

  - Bug fix for the "Packages" view for .deb / APT based systems: Now also list
    packages that are on hold. They were previously missing.


- 2022-05-30

  - Improved BSD support ([GitHub issue #195](https://github.com/shundhammer/qdirstat/issues/195)):
    If neither `/proc/mounts` nor `/etc/mtab` is available, fall back to using
    `QStorageInfo` (if available; Qt 5.4 or later).

    This returns a little less complete information; for example, only
    rudimentary mount options which are used for some special cases.


- 2022-05-19

  - Now using `xdg-open %d` in KDE Plasma for the "Open File Manager Here"
    standard cleanup action. This may help for
    [GitHub issue #192](https://github.com/shundhammer/qdirstat/issues/192),
    yet still maintain the ability to use a powerful file manager like
    _Konqueror_ (if that is configured), falling back to the standard _Dolphin_.


- 2022-03-08

  - Fixed [GitHub issue #190](https://github.com/shundhammer/qdirstat/issues/190):
    MIME categories for overlapping suffixes

    Files with multiple suffixes like `.tar.gz` were wrongly sorted into the
    same MIME category as `.gz`. It did find the category for the longer one
    (`.tar.gz`), but then it continued looking, resulting in the shortest
    matching one (`.gz`). Now stopping at the longest hit.


- 2022-02-25

  - Prevent a hanging Samba (CIFS) or NFS mount from blocking the program, even
    when no information from that mount is needed at all:

    If you started QDirStat with a path on the command line, it collected the
    information about used / free / reserved disk space for all mounted
    filesytems already. If you had a network mount that didn't respond, you
    still had to wait for a timeout before the program could continue.

    Now it collects that information only when it's really needed:

    - In the "Open Directory" dialog where it displays those sizes in the
      "Places and Mounted Filesystems" bar on the left

    - In the "Mounted Filesystems" (`du`-like) window (Menu "View" -> "Show
      Mounted Filesystems").


- 2022-01-04

  - Now cutting off insanely long generated device names of LUKS devices in the
    "Mounted Filesystems" window: E.g. `/dev/mapper/luks-3fae43...` instead of
    `/dev/mapper/luks-3fae4328-4294-4c77-8f98-d437c41da26c`. The long name is
    displayed in a tooltip.


- 2021-12-30

  - Don't show inactive (unmounted) mounts managed by the automounter anymore
    in the "Places and Mounted Filesystems" bar of the "Open Directory"
    dialog.

    Notice that those directories are still accessible from the tree at the
    right side, but they no longer take a prominent place in the left bar.


- 2021-12-12

  - Fixed [GitHub issue #184](https://github.com/shundhammer/qdirstat/issues/184):
    When reading a cache file, sparse files were displayed as "allocated: 0 Bytes".

    About QDirStat cache files:
    - [qdirstat-cache-writer documentation](scripts/README.md)
    - [QDirStat for Servers](doc/QDirStat-for-Servers.md)


- 2021-09-28

  - Merged the changes for
    [GitHub issue #176](https://github.com/shundhammer/qdirstat/issues/176)
    to _master_:

    In the treemap, a middle click now also behaves like a left click
    (including `Ctrl`+click); but it also displays white frames indicating the
    parent directories of that item, and everything that does not belong to
    that directory branch is covered by a transparent overlay.

    See [GitHub issue #181](https://github.com/shundhammer/qdirstat/issues/181)
    for a screenshot and more details.


- 2021-08-28 **New stable release: 1.8**

  **Summary:**

  - New view: _File Age Statistics_

  - Navigation history like in a web browser (_Back_, _Forward_)

  - Bug fixes

  - Some small improvements


  **Details:**

  - New view: _File Age Statistics_

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-years.png"    width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-age-years.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-months.png"   width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-age-months.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-long-ago.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-file-age-long-ago.png)

    This shows the number of files per year and/or per month that were last
    modified in that year or month, together with the total size of those files
    and a percent bar for both (number and total size).

  - New documentation for that view: [File Age Statistics](doc/File-Age-Stats.md)

  - Navigation history for directories like in a web browser:

    - New buttons _Back_ / _Forward_ in the tool bar

    - Standard keyboard shortcuts `[Alt] [Cursor Left]` /
      `[Alt] [Cursor Right]` like in all common web browsers

    - Support for _Back_ / _Forward_ mouse buttons if the mouse has them

    - History menu on those buttons (long press on the buttons) for the last 16
      directories


  - Internal restructuring and refactoring to keep the code maintainable

  - Bug fixes:

    - Fixed [GitHub issue #169](https://github.com/shundhammer/qdirstat/issues/169):
      Shorten path components in the breadcrumb widget for insanely long paths.

    - Now closing a left-over _Permissions error_ panel when refreshing from disk.
      After the re-read, the permissions error may no longer be there.


  - Small improvements:

    - In the _Mounted Filesystems_ window, don't show inactive mounts managed
      by the automounter anymore.

    - In the _Open Directory_ window, automatically open the first directory
      level if a path was clicked in the _Places_ list on the left, and scroll
      the tree on the right so that path is at the top.

    - Added a little margin to the left in the main window's tree view for the
      _Size_ column so it looks now less cramped.


- 2021-08-06

  - Lots of internal restructuring and refactoring to keep the code
    maintainable: In particular, the MainWindow code was getting out of hand
    with more and more features and menu actions being added all the time, so
    some of that was moved out to separate classes.

    That involved quite some testing and rethinking how certain parts are
    working, which in turn involved some head-scratching, rearranging code had
    the byproduct of more internal documentation and also some small bug fixes.

  - The "Permissions error" panel message didn't always go away when the user
    had simply left it open and just opened a new directory; and probably also
    in some other situations. Now it's always cleanly removed whenever a new
    directory tree is read or when it's re-read ("Refresh all" or "Refresh
    Selected"). That was one of those small bug fixes.


- 2021-07-31

  - Now the "File Age Statistics" window also shows statistics for the months
    of this and the last year as collapsible (and by default collapsed) items
    in the same list, so you can break down the age of files further for the
    last 13-24 months. If a month entry is selected in that list, locating the
    files of course only locates the files that were last modified within that
    month.

    <img width="500" src="https://user-images.githubusercontent.com/11538225/127743490-439ce7f8-40eb-405c-bbfb-89bc4a1873e0.png">

    More details and more screenshots at [GitHub issue #172](https://github.com/shundhammer/qdirstat/issues/172).



- 2021-07-29

  - Added a "Back" and a "Forward" button to move back and forth in the history
    of visited directories; i.e. QDirStat behaves now very much like a web
    browser when navigating the filesystem.

    It also has a history menu when you long-press the "Back" or "Forward"
    buttons, and of course it uses the same keyboard shortcuts `[Alt] [Cursor
    Left]` and `[Alt] [Cursor Right]` that all common web browsers use.


- 2021-07-28

  - Added a "Locate" button to the new _File Age Statistics_ view, using the
    same window as the actions from the "Discover" menu.

    This is enabled if there are no more than 1000 files in the currently
    selected year to avoid the results list to become too long and too
    unwieldy. If there are more than 1000 files in the currently selected year,
    break it down to another subdirectory first; otherwise you'd be ending up
    with the better part of the whole filesystem in the results list in the
    extreme case.

- 2021-07-27

  - Added a whole new type of view: _File Age Statistics_, displaying file
    modification times by years, so you can see during what time frame there
    was any activity in a directory tree:

    [<img width="700" src="https://user-images.githubusercontent.com/11538225/127198386-215ecc9a-325c-4954-afef-e1d7f271c013.png">](https://user-images.githubusercontent.com/11538225/127198386-215ecc9a-325c-4954-afef-e1d7f271c013.png)

    More details and a lot more screenshots at [GitHub issue #172](https://github.com/shundhammer/qdirstat/issues/172).


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

------------


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

----------


- 2021-03-24

  - New document about [finding files that are shadowed by a mount](doc/Shadowed-by-Mount.md)

  - New script for finding files on the root filesystem that are shadowed by a
    mount (see doc link above).


- 2021-03-19

  - Added a context menu to the "Locate Files" window (the "Discover" results)
    with the cleanup actions that are applicable to files.

  - Keyboard shortcuts for cleanups now also work in the "Locate Files" window.


- 2020-12-21

  - Fixed [GitHub Issue #149](https://github.com/shundhammer/qdirstat/issues/149):
    Segfault when using a cleanup action with refresh policy _AssumeDeleted_ after
    a cleanup action with _RefreshParent_ in the same directory.


- 2020-11-23

  - Menu reorganization: They had become a little too crowded, especially on
    the top level.

    - Moved out some options from the menus; they are still available when
      editing the config file manually:

      - "Show current path"
      - "Treemap as side panel"

    - "Expand tree level" is now limited to level 5 (formerly 9). Opening that
      many tree branches means a huge performance drop anyway.

    - The former "Treemap" menu is now a submenu of "View". Most of those
      actions are available as tool bar buttons and mouse wheel operations
      anyway.

    - The former "Settings" menu is gone; "Configure QDirStat" is now in the
      "Edit" menu. There was only that one action in the "Settings" menu, and
      that is quite wasteful in terms of screen space and toplevel menu
      complexity.

      I experimented shortly with moving some more settings there, e.g. "Show
      details panel" and "Show tremap", but I reverted that: The details panel
      is different for each layout (L1, L2, L3), and the layout switching
      actions are in the "View" menu where they belong; so "Show details panel"
      should really remain near them. And not having "Show treemap" in the
      "Treemap" submenu would be very confusing.

    The new toplevel menus are now:

    - File
    - Edit
    - View
    - Go To
    - Discover
    - Clean up
    - Help

    I.e. it's down to 7 items which is generally regarded as the gold standard
    by usability experts.

    I am not completely happy yet with how additional views are scattered among
    several menus:

    - "File" -> "Show installed packages"
    - "File" -> "Show unpackaged files"
    - "View" -> "File size statistics"
    - "View" -> "File type statistics"
    - "View" -> "Show mounted filesystems"

    The rationale is that those views in the "File" menu load completely new
    content, replacing the scanned directory in both the tree view and the
    treemap; thus they are on a similar level as "Open directory", so they are
    right next to that action.

    The other views open in separate windows, so they are add-on views to the
    currently loaded directory tree.


- 2020-11-22

  - Added a "Discover" toplevel menu to easily find
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


- 2020-10-23

  - Now showing the target of symbolic links in the details panel.

    If it's a short path, the whole path is shown; otherwise without the path
    (".../somewhere"), and the full path is shown as a pop-up upon mouse click.

    If the target does not exist, a **Broken Link** warning is also shown.

- 2020-10-15

  - Enabled _Refresh Selected_ now for files as well. In that case, the parent
    directory is refreshed.

  - Added hotkey `[F6]` for _Refresh Selected_.

    `[F5]` is still _Refresh All_ like in all web browsers. Since window
    managers / desktop environments tend to consume `[F5]` with modifier keys
    (`[Shift] [F5]`, `[Alt] [F5]`, `[Ctrl] [F5]`), this is the closest we can
    get, and it's more consistent than using something like `[Ctrl] [R]`.

    This was inspired by the discussion in [PR#145](https://github.com/shundhammer/qdirstat/pull/145).

- 2020-08-03

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

    Thanks a lot. Time to extend my `df` shell alias to exclude filesystem type
    "squashfs", too:

    ```
    alias df='/bin/df -x tmpfs -x devtmpfs -x squashfs -h $*'
    ```

------------

- 2020-07-26 **New stable release: 1.7**

  **Summary:**

  - Closing the gap between sizes reported by QDirstat and sizes reported by
    the `du` command: Now also taking the allocated size into account.

  - Now also displaying the allocated size where it makes sense.

  - New "Mounted Filesystems" window showing output similar to the `df` command
    (but without the cruft).

  - New directory selection dialog showing all (real) filesystems.

  - New checkbox to cross filesystems (temporarily) in the directory selection
    dialog.

  - Workaround for NTFS oddities: Ignoring hard links on NTFS.

  - Added config option to ignore hard links in general.

  - Added a document about Btrfs free size and how different tools tend to show
    different values: [Btrfs-Free-Size.md](doc/Btrfs-Free-Size.md)

  - Bug fixes


  **Details:**

  - Closing the gap between sizes reported by QDirstat and sizes reported by
    the `du` command: Now also taking the allocated size into account.

    There were repeated threads on various social media where users wondered
    why QDirStat displayed different sizes than the `du` or the `df` commands,
    sometimes slightly different, sometimes off by quite a lot.

    In short, this is mostly due to the difference between a file's _byte size_
    and its _allocated blocks_: On most filesystem types, a file of 49 bytes
    still consumes a minimum of 4 KB (4096 bytes); disk space is allocated in
    _clusters_ (typically 4 KB), not byte by byte. On a typical Linux root
    filesystem with many thousands of tiny files, this difference can add up to
    a lot.

    Formerly, QDirStat only added up the _byte sizes_, disregarding the
    allocated, but really unused part at the end of tiny files. The rationale
    was that some filesystem types handle that in more intelligent ways, yet
    there is no documented way to get information from a filesystem if it
    actually does that. It turned out that in reality, most of them don't
    bother; they simply let most of that last cluster go to waste.

    Now QDirStat displays _both_ sizes where useful:

    [<img width="845" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-tiny-files.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-tiny-files.png)

    - For tiny files, the tree view now shows both sizes: "49 B (4k)"

    - For directories, the tree view now shows the total of the _allocated_
      sizes. This may sound a bit inconsistent, but it feels very natural and
      intuitive: You are typically interested in how much disk space the
      subtree consumes, no matter if some part of that is really wasted.

    - The _details_ view shows both sizes when there is a difference (in the
      displayed numeric precision): "Size: 42.1 MB" vs. "Allocated: 42.2 MB",
      but not "Size: 42.0 MB" vs. "Allocated: 42.0 MB".

    - Treemap tiles now correspond to a file's _allocated_ size. This makes a
      real difference for tiny files.

    More details at [GitHub Issue #134](https://github.com/shundhammer/qdirstat/issues/134):
    _Size Difference between QDirStat Reports and the "du" and "df" Commands_.


  - New "Mounted Filesystems" window ("View" -> "Show Mounted Filesystems" or
    Ctrl-M):

    [<img width="611" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-filesystems-window.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-filesystems-window.png)

    This shows information about "normal" mounted filesystems (excluding system
    mounts like `/dev`, `/proc`, `/sys`, bind mounts, Btrfs subvolumes) with
    usage data as reported by the filesystem itself. The "Reserved" column
    shows the disk space reserved for the root user on that filesystem, "Free"
    is the available space for non-privileged users.

    Sometimes the "Used" size reported here may be different from what QDirStat
    reports after reading that complete filesystem; this can be due to
    metadata, journals or snapshots. Notice that in those cases, the `du`
    command will also display a different value than the `df` command.


  - QDirStat now has its own custom directory selection dialog; it no longer
    uses one of the simplistic Qt standard file dialogs:

    [<img width="498" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-open-dir-dialog.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-open-dir-dialog.png)

    The "Places" bar on the left shows your home directory and all "real"
    filesystems (the same as in the new "Mounted Filesystems" window). For each
    filesystem, it shows

    - The mount point
    - The total size of the filesystem
    - The filesystem type ("ext4", "btrfs", "ntfs", ...)
    - The device name ("/dev/sda2") (in a tooltip)

    In the "Path" combo box you can simply edit the path or copy and paste it
    from another window. Use the "Up" button to move one directory level up.

  - The "Cross Filesystems" checkbox lets you temporarily override the global
    configuration option of the same name: QDirStat will then no longer stop
    when a mount point is found during reading a subtree, it will descend into
    that mounted filesystem and read it, too; but again, only for "real"
    filesystems, not for system mounts such as `/dev`, `/sys`, `/proc`, not for
    bind mounts, not for filesystems mounted multiple times, and not for
    network mounts (NFS, Samba / CIFS).

    More details at [GitHub Issue #129](https://github.com/shundhammer/qdirstat/issues/129).


  - Added a workaround for wrong size sums on NTFS: Now disregarding hard links
    on NTFS.

    The (current?) _ntfs-3g_ implementation using _fuseblk_ seems to disagree
    with Windows tools which files really have hard links.

    It appears that _ntfs-3g_ regards even the MS-DOS compatible short filename
    (`PROGRA~2` vs. `Program Files`) as a hard link which is of course utter
    nonsense; that means that almost all files on an NTFS partition are
    reported as having multiple hard links, so QDirStat displayed them as
    having only half their real size.

    Notice that this is a bug in _ntfs-3g_, not in QDirStat.

    See also [GitHub Issue #88](https://github.com/shundhammer/qdirstat/issues/88).


  - <summary>
    Added a config option to ignore hard links.
    </summary>

     <details>

     This is useful for a very small number of use cases. Hard links are not
     very common anymore in today's Linux / BSD / Unix-like systems, so most
     users won't have to bother with this at all.

     By default, QDirStat sums up the disk space for a file with multiple hard
     links for each hard link's share of the overall size: If a file with 1 MB
     has 4 hard links, for each of those 4 links QDirStat adds 1/4 of the size
     (i.e., 256 kB) to the parent directory. If all those 4 links are in the
     same directory, that's very simple: They add up to 4 * 256 kB = 1 MB, so
     the sum is correct.

     If those hard links are all in different directories, each directory only
     gets part of that disk space allocated, because in fact they share the disk
     space among each other; the total disk space sum taking all those
     directories into account is still correct, of course.

     The trouble starts when you want to make a backup of only one of those
     directories: Even though the disk space is still shared with other
     directories, on the backup medium, you still need the disk space for that
     complete file, i.e. the full 1 MB, not only that directory's share (256
     kB). With a lot of hard-linked files, that can add up to a lot of
     difference between what QDirStat displays and what disk space you actually
     need for the backup.

     There was a user who makes heavy use of that, and for that kind of use case
     there is now the option to ignore hard links: In that case, QDirStat sums
     up the complete size (the full 1 MB) for each hard link of the file.

     While that is useful for this special case, and you can now see the total
     size that you will need for your backup medium for that one directory, the
     total size higher up in the directory tree where more than one of those
     directories that share hard linked files with each other is off: That file
     now appears 4 times with 1 MB each, so it will add up to 4 MB.

     There is now a new config option in `~/.config/QDirStat/QDirStat.conf`:

     ```ini
     [DirectoryTree]
     ...
     IgnoreHardLinks=false
     ```

     This is intentionally not available in the GUI config dialog to avoid
     confusion; use a text editor (while QDirStat is not running) to change
     this setting.

     Please use that config option only when you are aware of the consequences;
     this is a specialized option for rare, specialized use cases. It basically
     makes sense only if the other hard links are all outside the subtree that
     QDirStat displays.

     More details at [GitHub Issue #124](https://github.com/shundhammer/qdirstat/issues/124).
     </details>


  - There is now a new document about Btrfs free size and how different tools
    tend to show different values: [Btrfs-Free-Size.md](doc/Btrfs-Free-Size.md)

    Whenever users run out of disk space on Btrfs (which happens a lot, mostly
    because of snapshots) and they try to figure out where all their disk space
    went, they are confused about different tools reporting totally different
    and inconsistent sizes; traditional Linux / Unix command line tools like
    `du` and `df` just like GUI tools like QDirStat. Hopefully, this document
    will shed some light on that.


  **Bug Fixes:**

  - Fixed the internal cache writer (the one called from the _File_ menu, not
    the _qdirstat-cache-writer_ Perl script): For files with multiple hard
    links, it wrote the wrong size to the cache file: The result of _size /
    links_, not _size_.

    This was part of [GitHub Issue #124](https://github.com/shundhammer/qdirstat/issues/124).

  - Fixed crash when terminating with Ctrl-Q while reading directories
    [(GitHub Issue #122)](https://github.com/shundhammer/qdirstat/issues/122).

  - Now automatically reopening the old current branch in the tree view when
    using actions like "refresh selected" and "continue reading at mount point
    ([GitHub Issue #135](https://github.com/shundhammer/qdirstat/issues/135)).

  - Not a bug, but a minor new feature: Now automatically expanding the first
    directory level of a mount point after "continue reading at mount point".

  - Now correctly labelling a mount point as mount point in the "Details"
    panel.

------------------

- 2020-06-14

  - Now displaying the allocated size for very small files (8 kB (2 clusters)
    or less) in a much more subdued way so it's still there, but less
    distracting.


  - If a file is wasting very much space (i.e. if it uses 33% or less of its
    allocated disk space), now displaying the allocated size in bold in the
    "Details" panel to draw more attention to the fact. This affects pretty
    much all tiny files with 1360 bytes or less since the cluster size of most
    (all?) Linux filesystem is so large (4 kB for ext4, XFS, Btrfs).

  Screenshot for both changes at [GitHub Issue #134](https://github.com/shundhammer/qdirstat/issues/134).


- 2020-06-13

  - Now automatically reopening the old current branch in the tree view when
    using actions like "refresh selected" and "continue reading at mount point
    ([GitHub Issue #135](https://github.com/shundhammer/qdirstat/issues/135)).

  - Now automatically expanding the first directory level of a mount point
    after "continue reading at mount point".

  - Now correctly labelling a mount point as mount point in the "Details"
    panel.


- 2020-06-12

  - NTFS is now detected as "ntfs", no longer as "fuseblk" in the "Open
    Directory" and the "Filesystems" dialogs.

    This was fallout of
    [GitHub Issue #88](https://github.com/shundhammer/qdirstat/issues/88)
    (NTFS hard links).

  - Now ignoring hard links on NTFS; the (current?) ntfs-3g implementation
    using fuseblk seems to disagree with Windows tools which files really have
    hard links. And in this case I tend to have more faith in the Windows
    tools. Now the total sizes of my Windows partitions as reported by QDirStat
    are much closer to the sizes reported by the `df` command, the `statfs()`
    system call and QDirStat's new "Filesystems" window.

    See also [GitHub Issue #88](https://github.com/shundhammer/qdirstat/issues/88).


- 2020-06-11

  - Fixed [GitHub Issue #134](https://github.com/shundhammer/qdirstat/issues/134):
    _Size Difference between QDirStat Reports and the "du" and "df" Commands_:

    - Now also showing the _allocated_ size in the details panel for files and
      directory trees. This can make a a big difference if there are lots of tiny files.

    - Now showing the _allocated_ size for directories in the tree view, not
      only the sum of all the _byte_ sizes of all the files in the subtree.

    - Now using the _allocated_ size in the treemap so tiny files appear with
      their true size, no longer disappearing in the grey background: A 6 bytes
      file is now displayed with the 4 KB that it actually consumes, not just
      the 6 bytes of its content.

    More information and lots of screenshots at
    [GitHub Issue #134](https://github.com/shundhammer/qdirstat/issues/134).


- 2020-06-03

  - New "Mounted Filesystems" window ("View" -> "Show Mounted Filesystems" or
    Ctrl-M):

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-filesystems-window.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-filesystems-window.png)

    This shows information about "normal" mounted filesystems (excluding system
    mounts like /dev, /proc, /sys, bind mounts, Btrfs subvolumes) with usage
    data as reported by the filesystem itself. The "Reserved" column shows the
    disk space reserved for the root user on that filesystem, "Free" is the
    available space for non-privileged users.

    Sometimes the "Used" size reported here may be quite different from what
    QDirStat reports after reading that complete filesystem; this can be due to
    metadata, journals or snapshots.

  - Added a document about Btrfs free size and how different tools tend to show
    different values: [Btrfs-Free-Size.md](doc/Btrfs-Free-Size.md)



- 2020-05-27

  - Implemented [GitHub Issue #129](https://github.com/shundhammer/qdirstat/issues/129):
    _Option to continue reading at all mount points at once_.

    Previously, you could choose to either always cross filesystems while
    reading directories, or you could later open the parent branch of the mount
    point in the tree view and then use "Continue reading at mount point" from
    the main menu / context menu.

    Now you can change this setting temporarily for the current program run; it
    will continue reading at "normal" mount points, i.e. excluding system
    mounts like `/dev`, `/proc`, `/sys` and also bind mounts or network mounts; but
    it still reads Btrfs subvolumes.

  - This new option needed a check box in the "Open Directory" dialog; so from
    now on, QDirStat no longer uses the generic Qt file dialog, it has a custom
    one:

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-open-dir-dialog.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-open-dir-dialog.png)

    In addition to that new check box at the bottom, there is now also a new
    "places" widget for quick access to the user's home directory and all
    ("normal", see above) mounted filesystems, including network mounts (NFS,
    Samba / Cifs).


- 2020-03-19

  - Added a config option to ignore hard links.

    This is useful for a very small number of use cases. Hard links are not
    very common anymore in today's Linux / BSD / Unix-like systems, so most
    users won't have to bother with this at all.

    By default, QDirStat sums up the disk space for a file with multiple hard
    links for each hard link's share of the overall size: If a file with 1 MB
    has 4 hard links, for each of those 4 links QDirStat adds 1/4 of the size
    (i.e., 256 kB) to the parent directory. If all those 4 links are in the
    same directory, that's very simple: They add up to 4 * 256 kB = 1 MB, so
    the sum is correct.

    If those hard links are all in different directories, each directory only
    gets part of that disk space allocated, because in fact they share the disk
    space among each other; the total disk space sum taking all those
    directories into account is still correct, of course.

    The trouble starts when you want to make a backup of only one of those
    directories: Even though the disk space is still shared with other
    directories, on the backup medium, you still need the disk space for that
    complete file, i.e. the full 1 MB, not only that directory's share (256
    kB). With a lot of hard-linked files, that can add up to a lot of
    difference between what QDirStat displays and what disk space you actually
    need for the backup.

    There was a user who makes heavy use of that, and for that kind of use case
    there is now the option to ignore hard links: In that case, QDirStat sums
    up the complete size (the full 1 MB) for each hard link of the file.

    While that is useful for this special case, and you can now see the total
    size that you will need for your backup medium for that one directory, the
    total size higher up in the directory tree where more than one of those
    directories that share hard linked files with each other is off: That file
    now appears 4 times with 1 MB each, so it will add up to 4 MB.

    So please use that config option only when you are aware of the
    consequences; this is a specialized option for rare, specialized use
    cases. It basically makes sense only if the other hard links are all
    outside the subtree that QDirStat displays.

    If in doubt, leave this option off (which is the default).

    More details at [GitHub Issue #124](https://github.com/shundhammer/qdirstat/issues/124).


- 2020-03-06

  - Fixed the internal cache writer (the one called from the _File_ menu, not
    the _qdirstat-cache-writer_ Perl script): For files with multiple hard
    links, it wrote the wrong size to the cache file: The result of _size /
    links_, not _size_.

    This was part of [GitHub Issue #124](https://github.com/shundhammer/qdirstat/issues/124).

- 2020-02-24

  - Fixed crash when terminating with Ctrl-Q while reading directories
    [(GitHub Issue #122)](https://github.com/shundhammer/qdirstat/issues/122).


--------------


- 2020-02-13 **New stable release: 1.6.1**

  **Summary:**

  - Much better handling for "permission denied" errors while reading directories

  - Now showing the exact byte size (134 495 994 Bytes instead of 128.3 MB)
    upon mouse click in the tree (right click) and in the details panel (left
    or right click)

  - New optional tree column "Oldest File" (not enabled by default)

  - Bug fix: Support for dark widget themes in file size histogram window


  **Details:**

  - If you start QDirStat with insufficient permissions, you could easily
    overlook large subtrees in the filesystem that consume disk space, but were
    not visible to you. They did get a special icon, but you would have to open
    the parent directory in the tree view to see that.

    Now, QDirStat notifies you in several ways:

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-light.png" height="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-err-dirs-light.png)

    - All parent directories that contain a subtree that could not be read are
      now displayed in dark red in the tree view.

    - The _Size_ field of those directories as well as other accumulated values
      (_Items_, _Files_, _Subdirs_) are now preceded with a "greater than" sign
      to indicate that there is most likely more, but that information could
      not be retrieved: ">7.2 MB" indicating that it's at least 7.2 MB and most
      likely more than that, but we don't know because one or more
      subdirectories could not be read.

    - A message panel in the main window between the tree view and the treemap
      with a message that some directories could not be read.

      You can close the message with the `[x]` close button on its right side,
      but you can also simply leave it open. This is a lot less obtrusive than
      a pop-up dialog, yet less temporary than a message in the bottom status
      line that will disappear in a few seconds or when anything else is
      reported.

    - Clicking on the "Details..." link in that message opens a separate window
      to report all directories that could not be read (typically because of
      insufficient permissions).

      This window is non-modal, i.e. you can still interact with the main
      window when it is open. Click on any directory that it lists to locate it
      in the main window: The tree view will open that branch and scroll to
      make it visible.

      [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unreadable-dirs-window.png" height="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-unreadable-dirs-window.png)

    - In addition to the "locked folder" icon, unreadable directories are shown
      in bright red in the tree view.

    - When an unreadable directory is selected in the tree view, the details
      panel now shows a large padlock icon and a message "[Permission Denied]",
      and the permissions are highlighted in red.

  - You can now see the exact size in bytes both in the tree view and in the
    details panel: 134 495 994 Bytes instead of 128.3 MB. The field is still
    (somewhat) human readable with thousands separators (using blanks to avoid
    confusion with different decimal / thousands separators in different
    languages).

    This can make it easier to compare sizes with other tools that report them
    in bytes or that insist in using 1000-based units (QDirStat uses 1024-based
    size units: 1 kB = 1024 Bytes; 1 MB = 1024 kB; 1 GB = 1024 MB; etc.).

    Not using tool tips that appear automatically was a conscious decision:
    This level of detailed information is not needed that often, and tool tips
    get in the way whenever the mouse cursor lingers too long at an active
    spot. More often than not a tool tip obscurs other content that the user
    might want to read at that very moment. This is why in QDirStat in the rare
    cases that you are interested in those exact numbers, you have to click:

    - In the tree view, right-click a size field (a left click is used for
      selecting an item in tree views, so the context menu is pressed into
      service for that purpose (only for the size column)).

      [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size.png)

    - In the details panel, use left or right click. To indicate what fields
      can be clicked, they are now underlined when the mouse hovers over them.

      [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size-2a.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size-2a.png)

      _Hovering over fields that can be clicked shows them underlined, very
      much like a hyperlink._

      [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size-2b.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size-2b.png)

      _Clicking (left or right mouse button) shows more details: In this case,
      the exact byte size._

  - There is now an optional new column "Oldest File" that shows the timestamp
    (the mtime) of the oldest file in a subtree. This is strictly about files;
    directories, symlinks and special files (block or character devices, FIFOs
    etc.) are ignored for this.

    This may be useful to spot some old cruft, for example leftover dot files
    in your home directory. Many programs generate such files when you start
    them for the first time, and they are rarely cleaned up when they fall out
    of use.

    Notice that this column is not enabled by default. If you would like to use
    it, switch to layout L2 or L3, right-click the tree header to open the
    columns context menu, select _Hidden Columns_, then _Show Column "Oldest
    File"_.

    Of course you can also sort by this column to see the oldest files first
    (or last).

  - Fixed text color in histogram in dark widget themes
    [(GitHub Issue #117)](https://github.com/shundhammer/qdirstat/issues/117).


--------------------


- 2020-02-12

  - Fine-tuned error handling: If there is a "permission denied" error while
    reading a directory, this will now be shown much more clearly: There is now
    a clear distinction between "permission denied" any other generic "read
    error".

  - For directories that could not be read because of insufficient permissions,
    now also showing an additional "locked" icon in the details panel, and
    coloring the permissions field in red to point to the cause of the problem.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unreadable-dirs-window.png" height="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-unreadable-dirs-window.png)

    This color can be configured in `~/.config/QDirStat/QDirStat.conf`:

        [DetailsPanel]
        DirReadErrColor=#ff0000

- 2020-02-11

  - Now also showing the exact byte size of all size fields (of 1 kB and above)
    upon click in the "Details" panel. All clickable fields there are now
    underlined just like a hyperlink when the mouse hovers above them.

    I thought long and hard about using tool tips for that, but tool tips have
    a nasty habit of getting in the way and obscuring other information that
    the user might want to read; so I decided to use explicit mouse clicks.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size-2a.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size-2a.png)

    _Hovering over fields that can be clicked shows them underlined, very much
    like a hyperlink._

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size-2b.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size-2b.png)

   _Clicking (left or right mouse button) shows more details: In this case, the
   exact byte size._



- 2019-12-22

  - Brought back the exact byte size as the context menu for the "Size" column in the tree:

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-byte-size.png" height="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-byte-size.png)

    I.e. a right click on the size now shows the exact number of bytes to avoid
    confusion between those people who know what a Megabyte or Gigabyte is and
    those who introduced the confusion with 1000-based units vs. the good old
    1024-based units that everybody in IT uses (including Microsoft everywhere
    in all versions of Windows).

    _Back in the early days of computing, everything was easy: a Kilobyte was
    1024 bytes, a Megabyte was 1024 Kilobytes, and Gigabytes were Science
    Fiction. Then some morons came along who insisted on 1000-based units like
    everywhere in Physics. And they and the Real Engineers introduced a foul
    compromise: Rename 1024-based units to Kibibytes, Mibibytes etc. and
    redefine Kilobytes to be 1000 bytes, Megabytes to become 1000 Kilobytes
    etc.; since that day there is confusion what is what._

    **QDirStat always used and always will use 1024-based units.**


- 2019-11-02

  - Added a separate window to report all directories that could not be read
    (typically because of insufficient permissions). This window is opened only
    on request: When the user clicks on the "Details..." link in the panel
    message that reports that some directories could not be read.

    This window is non-modal, i.e. you can still interact with the main window
    when it is open. Click on any directory that it lists to locate it in the
    main window.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unreadable-dirs-window.png" height="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-unreadable-dirs-window.png)

  - Directories that could not be read are now also correctly reported in the
    unpackaged files view. Since they are empty when they could not be read,
    they will always end up in an `<Ignored>` branch which was previously not
    taken into account when reporting directories with read errors. This is now
    fixed.


- 2019-08-24

  - If directories could not be read (typically because of insufficient
    permissions), now also posting a warning message in a new message panel.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-light.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-err-dirs-light.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-dark.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-err-dirs-dark.png)

    You can close the message with the `[x]` close button on its right side,
    but you can also simply leave it open. This is a lot less obtrusive than a
    pop-up dialog, yet less temporary than a message in the bottom status line
    that will disappear in a few seconds or when anything else is reported.

    The infrastructure for this enables multiple such messages that can be
    closed in any order (or not at all) as the user likes. Each message can
    have a "Details..." link (not used in these examples) to provide more
    information.


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
    oldest file in a subtree. This is strictly about files; directories,
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

  - Reduced the display update interval in the packages view. While reading
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
    when there is such a performance bottleneck), no, it was something as
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


--------


- 2018-11-07 **New stable release: V1.5**

  Summary:

  - (Optional) breadcrumbs navigation

  - (Optional) a new "details" panel for the currently selected item

  - (Optional) new tree columns:
    owner, group, permissions both in "rwxrwxrwx" and in octal format

  - Package manager support for the major Linux package managers (dpkg / rpm /
    pacman) to see what package a system file belongs to.

  - Quick-switchable different layouts for the main window

  - A new "General" page in the configuration dialog

  - Show in the window title if the program runs with root permissions (`sudo`
    etc.)

  - (Optional) show the URL in the window title

  - Some bug fixes

  For more details, see the [full release announcement](https://github.com/shundhammer/qdirstat/releases/tag/1.5).


- 2018-11-03

  Added showing release notes (in an external web browser) to the "Help" menu
  ("What's New in this Release").


- 2018-10-26

  The pre-1.5 Beta is out: [QDirStat-1.4.97-Beta](https://github.com/shundhammer/qdirstat/releases/tag/1.4.97-Beta)


- 2018-10-21

  - Updated all screenshots to the latest Git source version in preparation for the
    upcoming pre-1.5 Beta.

  - Added a little left margin to the "Latest MTime" column in the tree to make
    it look less overcrowded.


- 2018-10-20

  - Reintroduced showing the elapsed time during directory reading.

    You might have noticed that QDirStat's performance has decreased with all
    the kernel patches to work around the Meltdown and Spectre security
    problems caused by CPU design; system calls have become a lot more
    expensive in terms of performance now. And QDirStat does a LOT of system
    calls while reading directories: `opendir()` / `readdir()` for each
    directory in the tree and `lstat()` for every single file or directory
    found in the tree: My 6.8 GB root filesystem has 275,000 items total and
    25,500 directories which means well over 300,000 system calls. Every single
    one of them now causes kernel page tables to be cleared and restored for
    every switch between user space and kernel space, and that means quite some
    performance impact.

    This all means that it's now worthwhile again to display the elapsed time
    during directory reading. It used to be over in a heartbeat, so it wasn't
    worthwhile to display that; but that's different now.

  - Added a _General_ page to the configuration dialog for miscellaneous
    settings.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-general.png" width="300">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-config-general.png)

    This is a bit unorganized (as might be expected for misc settings), but
    those misc settings had accumulated over time, so I decided to finally add
    such a page.

    The settings were always accessible, but only by editing the config file
    (`~/.config/QDirStat/QDirStat.conf`) manually. For some settings you might
    still have to do that: Some are too exotic for general use, some others
    would confuse the average user much more than they would help.


- 2018-10-19

  - Added different layouts to the main window:

    You can now switch between three (at the moment) different layout options
    for the upper half of QDirStat's main window.

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L1.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-file-L1.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L2.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-file-L2.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L3.png" width="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-file-L3.png)

    - Layout 1 (short): Display only a bare minimum of columns in the tree
      view, but show the new details panel on the right side. This is to
      minimize clutter in the tree view, yet keep all the information
      available.

    - Layout 2 (classic): Display the same columns as always in the tree view
      and additionally the new details panel on the right side.

    - Layout 3 (full): Display all available columns in the tree view,
      including the new file owner, group, permissions in both "rwxrwxrwx" and
      octal. But don't display the new display side panel so there is enough
      screen space to show all those columns.

    Switching between the layouts is as easy as pressing one of the three new
    buttons in the tool bar: L1, L2, L3. Or use the corresponding entries in
    the "View" menu. Or use the Alt-1, Alt-2, Alt-3 key combinations.

    For each view, you can individually configure what columns to display and
    whether or not to display the new details side panel and / or the new
    breadcrumbs navigator.

    The column widths are still configured globally (and this will remain like
    that); I experimented with that and found it _very_ awkward to have to set
    all of them individually for each layout.

  - We are approaching a new release (I guess you figured that with all the
    recent changes). Most likely there will be at least one Beta release (if
    there are problems maybe more) before the 1.5-stable release.

  - There is still some (limited) time to add even more package managers if
    anybody feels inclined to submit patches. But please be advised that I
    can't do much maintenance for any of those since I don't have any system
    with them readily available to test anything; the ones that I have use
    _dpkg_ or _rpm_.

  - Fixed [GitHub Issue #86](https://github.com/shundhammer/qdirstat/issues/86):
    CacheWriter errors go unnoticed much too easily.

    This was a direct result of me giving in to nagging people wanting features
    of very limited value: The status bar flickering like crazy all the time
    because somebody back some time ago had wanted the current treemap tile's
    path and size in the status bar as the mouse hovers over it.
    [GitHub Issue #30](https://github.com/shundhammer/qdirstat/issues/30)

    The effect is that nobody pays attention anymore at all to that status bar
    because it's flickering all the time anyway, so it has become useless as a
    tool to report anything of importance; and status bar messages are gone
    immediately, too, when the user happens to move the mouse across the
    treemap.

    As a consequence, I just disabled that by default; if anybody really wants
    it, edit the QDirStat config file (`~/.config/QDirStat/QDirStat.conf`) and
    enable it again:

        UseTreemapHover=true

    _Update 2018-10-20: This can now be changed in the new "General" page of
    the configuration dialog; no need to edit the config file manually._


- 2018-10-18

  - Added support for the _pacman_ package manager for Manjaro and Arch Linux.

    This should now cover the most common package managers for Linux to find
    out what package a system file belongs to: _dpkg_, _rpm_, _pacman_.

    As mentioned before, higher-level package managers such as _apt_, _zypper_,
    _yum_ or front-ends like _synaptic_, _PackageKit_ all use one of the
    lower-level package managers internally, so it doesn't matter if you never
    used one of the lower-level tools before.

  - Tried to add support for the _pkg_ package manager ("pkg info") for FreeBSD
    / OpenBSD, but unfortunately it only knows what package owns files of
    _ports_ / _3rd party_, not for the base system.

    The helpful people in the _#freebsd_ IRC channel on _freenode_ confirmed
    that. So this will have to wait until there is useful support for it from
    the system side on those platforms. _BSD people, let me know!_


- 2018-10-16

  - QDirStat now shows `[root]` in the window title if it is running with root
    privileges.

  - If invoked with `sudo`, now restoring the owner of the config files to the
    real user (if possible) if those file are in the user's home directory.
    Previously on some systems they were owned by _root_ which meant they were
    no longer writable by the real user, silently discarding all subsequent
    changes to the configuration (including window sizes etc).

    This might be different depending on how `sudo` is configured on a system;
    on SUSE, it uses the root user's home directory, on Ubuntu, the home
    directory of the user who invoked `sudo`.


- 2018-10-07

  Added better classification of files to the new file details view:

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L2.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-file-L2.png)

  - MIME Category - this is the same as in the treemap colors, the same as you
    can configure in the _MIME Categories_ page in the QDirStat configuration
    dialog. By default, it shows very broad categories ("Documents, "Music",
    "Images", "Videos", "Source Files", ...), but you can configure them to
    much finer detail if you wish.

  - System file or not. This is important if you want to run any cleanup
    actions in that directory; you probably don't want to mess with system
    files, even if running QDirStat as root.

    A system file in this context is a file that is either owned by a system
    user (a UID < 500) or that is located in a known system directory (/usr,
    /lib, ..., but not /usr/local).

  - For system files, the package that this file belongs to. This gives a
    surprising amount of insight (it was surprising to me, at least) where all
    the disk space on the system directories goes to, most importantly some of
    the big blobs in the tremap.

    As of now, this is supported for Linux systems using a package manager
    based on _dpkg_ or on _rpm_:

    - Debian

    - Ubuntu / Kubuntu / Xubuntu / Lubuntu

    - SUSE (openSUSE Tumbleweed or Leap, SLES)

    - Red Hat (Fedora, RHEL)

    ... and dozens more (basically all that are based on any of the above).

    This works by running `dpkg -S` or `rpm -qf` as external commands, so this
    is a somewhat expensive operation. To keep the user interface responsive,
    QDirStat now has an "adaptive timer" for updating that information:
    Normally, the result is shown instantly, but if you click around wildly,
    there is a timer that is increased or decreased (thus "adaptive") for a
    delayed update after that timeout (0 / 333 / 1000 / 2500 millisec right
    now).

    It can even handle _rpm_ installed as a foreign package manager on a _dpkg_
    based system (and the other way round); it tries the primary package
    manager first, then any others that are also installed.

    Please notice that _apt_, _synaptic_, _zypper_, _pkgkit_ and whatnot are
    all higher level package managers that ultimately use one of the low level
    ones, so even if you only use a higher level package manager, it still
    works without restriction.

    If your system does not use _dpkg_ or _rpm_, those who can are invited to
    contribute patches for other package managers; it's really simple:

    https://github.com/shundhammer/qdirstat/blob/master/src/PkgManager.cpp#L140

    The API is not final yet; there may be more regexp support in the near
    future (so it will only get simpler). But you get the idea.

--------------------------------------------------

- 2018-10-03

  - Added a new details view next to the tree view.
    Of course this can be disabled (Menu _View_ -> uncheck _Show Details Panel_).

    This view shows context-sensitive information about the currently selected
    item(s); see also the screenshots (still not complete, but you get the idea):

    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L2.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-file-L2.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-dir.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-dir.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-dot-entry.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-dot-entry.png)
    [<img src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-multi-sel.png" height="250">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-details-multi-sel.png)

    What that details view displays so far:

    - File: name, type (file / symlink / block device / character device /
      FIFO), size, user, group, permissions both as rwxrwxrwx and octal,
      modification time.

    - Directory: name with appended slash to easier recognize what it is, type
      (directory), subtree total size, total items, total files, total
      subdirs, latest modification time in the entire subtree; directory own
      size, user, group, permissions both as rwxrwxrwx and octal, modification
      time of the directory node itself (that was never shown before).

    - <Files> pseudo directory: Only the subtree information like for a
      directory, but no user / group / permissions since there is no
      counterpart for it on the filesystem.

    - Multi-selection: Number of selected items, total size, number of files,
      number of directories, number of files in any subtrees of those
      directories. This should show a bit more clearly what is affected if you
      choose to delete all that selected stuff.

    The view can scroll, so extremely long file names (as are common e.g. below
    `.git/objects` directories don't blow this view up to take all the screen
    space away from the tree and the treemap.


  - Unified the different size columns into one: No more "Subtree Size" and
    "Own Size" separately, but now just "Size". The reasoning used to be that
    there was no way to tell how large the directory node itself was. But that
    information is now readily available in the details view if anybody is
    really interested.

  Plans for the immediate future:

  - Add some package manager support, at least for the very common ones on
    Linux (_deb_ and _rpm_ so far): Find out what package a system file belongs
    to and show it (on demand or after a short delay) in the details view. This
    can be done even as non-root with some commands like `dpkg -S` or `rpm
    -qf`. There will probably be support for adding your own command lines in
    the config file if you use another package manager.

  - Some simple checks if a file is probably a system file; for example, files
    below `/usr/bin` (and other very common system directories) and/or files
    that are owned by a user with a UID < 500 (or so) are very likely system
    files that should be very careful to delete. The details view should show
    you that; maybe there should also be a warning in the cleanup actions if
    any such files are affected by a potentially destructive cleanup action.

  - Easily switchable different layouts for the tree columns and the details
    view:

    - Minimalistic: Only the bare essentials of columns plus the details view.

      This should remove a lot of the current clutter on the screen, and it
      should display the same information, only rearranged a bit. How often do
      you really need fields like number of items / files / subdirs in the tree
      view? They would be most useful there if you wish to sort the tree after
      any of them. How often do you do that? And with the new details view, the
      information is still available, but does not clutter the table.

    - Default: Pretty much what you can see now in the new screenshots

    - Maximum information: All columns, including the new user / group /
      permissions columns, but (to make space) no details view

    If it makes sense (not sure yet), maybe also user defined views (say, 3 of
    them?) that you can define to your personal liking. I kinda like how camera
    makers use that (U1 / U2 on Nikon cameras, C1 / C2 / C3 on Canon and
    Panasonic). I am not completely sure yet just how useful that is; I'll need
    to experiment.


- 2018-10-02 Implemented a _breadcrumbs_ widget to show the current path and
  for easier navigation up the directory hierarchy. See also the new
  screenshots.

  Of course this can be disabled (Menu _View_ -> uncheck _Show Current Path_).


- 2018-09-27 Fixed [GitHub issue #84](https://github.com/shundhammer/qdirstat/issues/84):
  Crash if picking up a cache file in the starting directory (subtree root).

  This was a crash that happened when users had used the supplied
  _qdirstat-cache-writer_ script to generate cache files in certain directories
  for faster directory scanning (at the price of having slightly outdated
  data), and QDirStat was not started with that cache file as a command line
  argument, but it would find it while reading the directory at the starting
  directory (and only there). This was probably broken for a long time, yet
  nobody had noticed (or nobody bothered to write a bug report). This might be
  an indication that this cache file feature is not widely used, so only a
  small number of users seem to be affected. Anyway, this is now fixed.


- 2018-09-23 Implemented [GitHub issue #80](https://github.com/shundhammer/qdirstat/issues/80):
  Show directory/file user (owner), group and permissions in the tree view.

  Those new columns are not visible by default. Use the context menu in the
  tree columns header to enable them. They might be useful for some users; for
  others, they may be just more clutter being displayed.

  Sorting by those columns is somewhat basic; both user and group are sorted by
  the numeric uid or guid; the permissions are sorted by the numeric value of
  the mode field. Looking up user or group names is a somewhat costly
  operation. It might also make sense to order system files first (user with
  UID 0, i.e. root; other system users also have small UIDs.)

  See issue #80 for a screenshot.


- 2018-08-26 Noah Davis (noahdvs) contributed a new application icon for QDirStat:

  <img src="https://github.com/shundhammer/qdirstat/blob/master/src/icons/qdirstat.svg" height="64">

  I had made the old icon myself back in early 2000 for the first KDirStat 0.86
  release. I never liked it very much, but the new application just needed an
  icon. Finally somebody with more artistic talent than myself made one that
  looks a lot more professional. Thank you, Noah!


- 2018-04-26 Implemented [GitHub issue #77](https://github.com/shundhammer/qdirstat/issues/77):
  Show the current URL in the window title. This is disabled by default.
  You can enable it manually in the config file (`~/.config/QDirStat/QDirStat.conf`):

      [MainWindow]
      ...
      UrlInWindowTitle=true

  Some day this will be configurable in a new tab in the configuration dialog,
  but right now there are only so few options that would go to such a "General"
  or "Misc" tab that it would look pretty lost and unorganized.


- 2018-02-08 Fixed [GitHub issue #74](https://github.com/shundhammer/qdirstat/issues/74):
Crash deleting folder while scan is in progress

  This fix comes on two levels:

  - A fix on the GUI-level that simply makes sure that the "move to trash"
    action in the tool bar / context menu is disabled while a directory tree is
    read.

  - A fix on the low-level internal classes for the in-memory directory tree
    and the read job queue: This now also makes sure that whenever a subtree is
    deleted from the outside (e.g. because of cleanup actions), any pending
    directory read jobs for that subtree are removed from the job queue.

- 2018-02-03 Fixed [GitHub issue #72](https://github.com/shundhammer/qdirstat/issues/72):
Allow to specify install prefix

  You can now install to another location like `/usr/local` if desired.
  The default remains `/usr`. See section _Install to a Custom Directory_.


- 2018-01-12 Some small fixes:

  - Fixed duplicate keyboard shortcut for actions "Stop Reading" (now: Ctrl-S)
    and "Copy URL to Clipboard" (still Ctrl-C).

  - Fixed exclude rules not matching correctly against direct root directory
    children: A rule that should match "/var" now works correctly. You'd need
    to specify "//var" in the rule which of course was wrong.

  - Now only adding the default exclude rule ".snapshot" once. If you remove
    that one, it should no longer keep reappearing if there are no other
    exclude rules.

  - Fixed some minor issues in the exclude rule configuration dialog with some
    widgets not being correctly disabled if they made no sense to use.


----------------------------


- 2017-06-04 **New stable release: V1.4**

  It's about time to ship all those latest changes.

- 2017-06-04 Fixed problem with directories that have read, but not execute
  permissions thanks to _slodki_:

  In that case, you'd get a warning in the log for every entry in such a
  directory, and it would get the wrong icon (a locked folder) and null values
  for all fields. Now checking for execute and read permission of the directory
  in advance and not even trying to read any contents (because the values would
  be bogus anyway).

- 2017-05-31 Fixed [GitHub Issue #61](https://github.com/shundhammer/qdirstat/issues/61):
Files and directories with UTF-8 special characters in the name not read
correctly when built with Qt 4.x

  This happened only when QDirStat was built against Qt 4.x, but that is the
  default for NHellFire's PPA, so this affects all Ubuntu users who installed
  QDirStat from that PPA.

  Thanks to _slodki_ who pointed this problem out!

- 2017-05-12 Checked code with [Coverity](https://scan.coverity.com/)

  Coverity offers free static code analysis for Open Source projects.
  This is really an outstanding tool, and it does a really thorough analysis.

  You might be glad to hear that while it complained about some minor things,
  there was not a single issue that would have been user relevant (let alone
  any security problems - there were none). Still, I did my best to fix the
  small complaints it had, and now we are down to zero outstanding defects
  reported by Coverity in QDirStat's 130,000 lines of code.

- 2017-04-21 More consistency between file type and size statistics

  Like the new file size statistics window, the older file type statistics
  window now uses the currently selected directory (in the tree view), not
  always the tree's toplevel directory. If nothing is selected, it still uses
  the toplevel directory.

  That means that F3 no longer toggles that window, but re-populates it with
  the currently selected directory instead. This is consistent with the F2
  view.

  Of course, the "Locate Files by Type" window now is also restricted to that
  subtree which actually gives it better functionality if you know that you
  want to locate files only there.

  This means that you can now select a subdirectory in the tree, open the file
  type statistics for it (F3 key), then select any filename extension (suffix)
  in that window and then open the file size statistics (F2 key) for that file
  type in that subtree.

  Previously, you would have to start QDirStat to show only that directory,
  then open the file type statistics window (F3), then the file size statistics
  window (F2) from there.


- 2017-04-14 _Let's do some real statistics_

  It's been a while since the last official news here, but I wasn't idle during
  that time:

  ![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram.png)

  This is a whole new kind of statistics in QDirStat showing how file sizes are
  distributed. You can start that for any selected directory (menu _View_ ->
  _File Size Statistics_ or F2) or from the _File Type Statistics" window if
  you select any filename suffix (extension) there and then _File Type_ ->
  _Size Statistics_ (or F2). In the latter case, you can see how large all your
  photos (.jpg), your videos (.mp4) or whatever are.

  This new statistics window deals with a lot of things you might have come to
  hate at school or at university, and which your math teacher or your
  statistics professor never explained in a way that mere mortals can
  understand, so I added those explanations as a bonus. There is a landing page
  for that in that new window:

  ![File Size Help](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-stats-help.png)

  Or you might use
  [this](https://github.com/shundhammer/qdirstat/tree/master/doc/stats)
  as a starting point.

  Everybody thinking "I have no clue what this is all about", please have a
  look at the
  [Median, Quartiles and Percentiles Explained](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Median-Percentiles.md)
  document to convince yourself that really **everybody** can easily understand
  this.

  I also opened a
  [GitHub issue to discuss this](https://github.com/shundhammer/qdirstat/issues/60);
  comments are welcome.


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

----------------------------

- 2017-03-05 **New stable release: V1.3**

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
  hand, another Btrfs mounted into the current filesystem is of course treated
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
  ("Could not connect to display"), it does not dump core anymore, but just
  exits with error code 1.


- 2016-12-06 **Warning to Btrfs users** (Fixed as of 2012-12-09)

  If you use QDirStat to scan a Btrfs partition,
  [any subvolumes of that partition are not scanned](https://github.com/shundhammer/qdirstat/issues/39):
  Btrfs subvolumes are treated just like ordinary
  mount points (which, to all intents and purposes, they are). So you might
  wonder why the _df_ command shows your 40 GB root filesystem as 97% full, yet
  QDirStat shows only about 7 GB. The rest might be hidden in subvolumes.

  QDirStat stops reading at mount points - which only makes sense because
  normally you want to know what eats up the disk space on that one partition
  that is filling up, not on any others like /home that are mounted
  there. Unfortunately, a Btrfs subvolume is also just another mount point, and
  QDirStat will stop reading there, too - at /var/log, at /var/spool, at
  /var/lib/libvirt etc.; a typical Btrfs root filesystem has about a dozen
  subvolumes, and all files in them are currently disregarded by QDirStat. You
  can of course click on "Continue reading at mount point" individually in
  QDirStat's directory tree for each one of them, but that's tedious.

  I am working on a solution. One approach would be to check if the current
  filesystem is Btrfs and list its subvolumes, but the Btrfs developers in
  their infinite wisdom decided that `btrfs subvolume list <path>` is a
  privileged operation, so QDirStat would have to use `sudo` with it and prompt
  for the root password (at which point I as a user would terminate the program
  and not use it anymore). **This is broken by design.** A simple info command
  like that should not require root privileges.


- 2016-10-31 (Halloween) **New stable release: V1.1-Pumpkin**

  It's about time for another official release to get the accumulated fixes and
  small changes out into the world. Since today is Halloween, this release
  shall be named _Pumpkin_ (as in the unforgettable Charlie Brown's _Great
  Pumpkin_).

  The last stable release, V1.0, was in mid-May (2016-05-16). Since then, there
  were 5 bug fixes and one small feature (the config file split up into
  independent parts so admins can provide presets to their users without
  overwriting the complete configuration), all described in greater detail
  below.


- 2016-10-23

  - Fixed [GitHub issue #32](https://github.com/shundhammer/qdirstat/issues/32):
    %p does not escape single quotes properly

    If you have a file name like `Don't do this.txt` (with a quote character in
    the name), the shell used when executing a cleanup action with this would
    complain about unmatched single quotes.

    QDirStat had always escaped such single quotes, but not the way common
    shells (Bash, Zsh) expect it: They don't want a backslash in front of that
    embedded single quote. Rather, you need to terminate the string with a
    single quote, escape the embedded quote with a backslash (or put it into
    double quotes), and then re-open the old string with another single quote.

    Thus, `'Don't do this'` becomes `'Don'\''t do this'`.

    This is certainly not what most people expect. I just wonder how much other
    software is out there that does it the intuitive (yet wrong) way: Just
    escape the single quote with a backslash (`'Don\'t do this'`).

    Of course, such file names should be avoided entirely, but you can't help
    some slightly broken MP3 ripper program doing it, so it needs to be handled
    correctly.

  - Fixed [GitHub issue #31](https://github.com/shundhammer/qdirstat/issues/31):
    Segfault with cleanup action while reading directories

    Now disabling cleanups that have a refresh policy other than "No Refresh"
    while directory reading is in progress; otherwise the re-read when the
    cleanup action has finished clashes with the directory read already in
    progress.

    This is not an optimal solution, but a very pragmatic one; the optimal
    solution might queue updates and execute them after the main read is done.

  - Fixed [GitHub issue #33](https://github.com/shundhammer/qdirstat/issues/33):
    Added command line option `--slow-update` (or `-s`) for slow remote X connections.


- 2016-08-12

  - Fixed [GitHub issue #23](https://github.com/shundhammer/qdirstat/issues/23):

    The internal cache writer would sometimes generate incorrect cache files
    because of buggy URL escaping resulting in an empty file name and thus
    invalid cache file syntax. This affected file names with colons (which is
    weird, but legal).

    One of these days I'm going to throw out all that QUrl stuff and replace the
    few things that I need with something that actually works consistently and
    not just under optimum conditions.


- 2016-08-10

  - Fixed [GitHub issue #22](https://github.com/shundhammer/qdirstat/issues/22):

    Cache files containing the root filesystem would not display correctly or
    segfault under certain conditions. This is now fixed.

  - Added "Refresh All" action to the main window tool bar. I had consciously
    avoided that because it's just too tempting to re-read the complete
    directory tree rather than think about what actually might have changed and
    then refresh just that, but it has become so common to use that action in
    web browsers that I found myself missing that more and more. And re-reading
    is not that expensive on today's mainstream PCs.


- 2016-07-02

  - Fixed [GitHub issue #21](https://github.com/shundhammer/qdirstat/issues/21):

    When started from a desktop menu, i.e. without any command line parameters,
    QDirStat would not prompt for a directory to read, but read the current
    directory (typically the user's home directory) right away.

  - More graceful handling for nonexisting paths specified on the command
    line: It now no longer just throws an exception right after starting the
    program (which looks like a crash to the unwary user), but posts an error
    popup instead and then asks for a directory to read.


- 2016-06-29

  - V1.01 (Development version)

  - Split up config file into four separate ones below ~/.config/QDirStat:

    - QDirStat.conf
    - QDirStat-cleanup.conf
    - QDirStat-exclude.conf
    - QDirStat-mime.conf

    This should make it much easier for site administrators to provide their
    own site-wide cleanup actions, exclude rules, or MIME categories. I did
    this with this in mind:

      http://moo.nac.uci.edu/~hjm/kdirstat/kdirstat-for-clusters.html

    Here, they describe how users should overwrite their KDirStat config file
    with one provided by the site admin so all users have those carefully
    crafted cleanup actions. But that also means that all other settings get
    lost each time there is a change in any of those commands, and users have
    to update that config file again.

    With the latest change, it is now possible to only replace the cleanup
    action config (QDirStat-cleanup.conf) and leave everything else untouched.

    Notice that this is far from a perfect solution; all cleanup actions the
    user added himself still get lost. But doing this perfectly might pretty
    quickly become an overengineered solution that would be hard to understand
    for everybody.

    As for migration from previous single-file configurations, QDirStat does
    that automatically: It reads the single file and moves the respective parts
    where they belong. No need to bother with any migration scrips or anything
    like that.


- 2016-05-16 **First stable release: V1.0**

  After 3 months of Beta phase and 3 Beta releases, here is finally the
  official first stable release of QDirStat: Version 1.0.

  In terms of source code, there were very little changes from the last Beta
  (0.98-Beta3 from 2016-04-08) and no real code change (only the version number
  increased) from the last check-in from 2016-04-11. This version can really be
  considered stable in the truest sense of the word. It was not rushed out the
  door, and there were no hectic last minute changes. It is well tested, and
  the community had ample opportunity to report any problems.


- 2016-04-11

  - _buxit_ reported
    [GitHub issue #16](https://github.com/shundhammer/qdirstat/issues/16)
    and contributed the fix for it shortly afterwards:
    When clicking in the treemap, the corresponding item in the tree view was
    not always scrolled into the visible area. Now it is.

- 2016-04-08

  - Beta 3

  - Fixed
    [GitHub issue #15](https://github.com/shundhammer/qdirstat/issues/15):

    After a cleanup action is executed that needs refreshing the affected
    subtree, the parent directory is selected, which is intentional so the
    user's focus is not thrown off completely. There was a bug when you
    selected an item in the treemap afterwards, that change was not correctly
    propagated to the internal selection model: The parent directory remained
    selected (which was wrong), and the newly selected item was just added to
    the selection, i.e. that item and (typically) its parent directory was
    selected. When a potentially dangerous cleanup operation was now started,
    it would affect not only that item, but also the directory; and, worse,
    that directory often went out of the visible scope of the tree view. Yes,
    the confirmation popup would ask for both of them, but you all know how
    quickly users click away those popups without really reading them.
    This bug is now fixed.

  - Improved the confirmation popup. Now highlighting directories much more if
    there is a "mixed" selection, i.e., both directories and non-directories
    are selected at the same time:

    ![New cleanup confirmation popup](https://cloud.githubusercontent.com/assets/11538225/14390476/8b022c9a-fdb7-11e5-8eef-a5ba304d3bab.png)

- 2016-03-20

  - Beta 2

    Beta 1 has now been out for 6 weeks, and I have not received one single bug
    report during that time. Maybe it's just business as usual, and people
    just keep waiting for others to do the testing, while they themselves are
    waiting for a stable release. Well, okay, so let them have their way: The
    current code is now officially V0.92 Beta 2. And it won't be another 6
    weeks; the next versions will come a lot more quickly. Once V1.0 final is
    out, any bug reports will have to wait until there is time to work on
    them. So, folks, use those Betas wisely.

    BTW those who actually did test it will find that QDirStat is a lot more
    stable even in Beta 1 than other pieces of software in their official final
    release.

- 2016-02-27

  - Debian / Ubuntu packaging contributed by Nathan Rennie-Waldock.
    He also made a PPA repository available for various Ubuntu versions - see
    [Ubuntu packages](https://github.com/shundhammer/qdirstat#ubuntu) below.


- 2016-02-06

  - Added tab for exclude rules configuration to the config dialog (see
    screenshot above). That's it: That was the last missing major feature.

    **I hereby declare QDirStat to be Beta.**

    _Please use the GitHub issue tracker for any bug reports._


  - Exclude rules can now optionally match against the full path again. I had
    changed this for just the directory name without the path by default, which
    makes regexps a lot simpler. You can now select the old behaviour, too, if
    you wish. This is configurable in the exclude rules tab of the config
    dialog.

  - Made the config file format of the new view header columns human readable
    and editable. The first version from yesterday used the native format of
    Qt's QHeaderView -- a QByteArray in hex encoding. This was a sorry excuse
    for a settings format - not usable for anybody, not legible, much less
    editable. Trolls, WTF? Pretty and usable formats everywhere else, and a
    glorified (well, not even glorified) hexdump here?

    I hope some admins who might want to provide ready-made config files for
    their users will appreciate that. If not, this is just for consistency's
    sake; I want to be able to read and edit my config file as I like, even
    without any graphical config dialogs.

  - The tree view now uses "middle eliding" for texts that don't fit into a
    column. It used to elide at the end of the text, but that's not necessarily
    useful for long file names; they often differ only at the end with lots of
    text at the start in common. So, now it's no longer "VeryLongTextBlurb...",
    but "VeryLongTe...foo.o" if anything needs to be cut off. Of course, this
    makes most sense with the new column width modes, otherwise your column
    will simply be resized wide enough to fit everything in.

- 2016-02-05

  - Extended the context menu of the tree view header columns -- see latest
    screenshot above. The configuration is now saved and restored when entering
    the program. You can move the columns around (i.e. change their order),
    hide columns, and choose between automatic column width ("auto size") or
    setting it manually ("interactive size") for each column individually or
    for all columns at once.

  - You can now read a cache file directly from the command line:

        ````
        qdirstat --cache cache-file
        ````

  - Fixed GitHub issue #9:
    [qdirstat-cache-writer creates broken cache file if some directory lacks Exec flag](https://github.com/shundhammer/qdirstat/issues/9)

  - Fixed GitHub issue #10:
    [incorrect handling of sparse files with 0 allocated blocks](https://github.com/shundhammer/qdirstat/issues/10)

- 2016-02-02

  - Fixed a bug where directory names with a colon ":" were cut off when
    reading a cache file, thus all files and directories below that point could
    not find their parent directory, so that complete branch was cut off.

  - Much improved performance for treemaps of large directory trees: Now not
    rebuilding the treemap immediately when the user resizes the window or
    drags the splitter dividing the main window, but just scheduling an update
    in 200 milliseconds. If another rebuild is requested during this time, the
    previous one is discarded. The net effect is that the treemap now is
    rebuilt only once, not for every pixel size change of the treemap
    subwindow, so the application remains responsive during dragging the
    splitter or resizing the main window.

- 2016-02-01

  - Fixed GitHub issue #6:
    [NullPointerException when reading cache file](https://github.com/shundhammer/qdirstat/issues/6)

    The DirTreeModel and the DirCacheReader were somewhat out of sync with
    regard to which directory is ready for display in the tree view.

- 2016-01-30

  - Added a context menu for the tree view header. It's still very limited, but
    you can now turn off auto-resizing of the tree columns for the current
    session.

  - Added a .desktop file so QDirStat should now show up in the menu of the
    major desktop environments (KDE, GNOME, Xfce, Unity, ...) and in their file
    managers' "Open With" menus when you right-click a directory.

- 2016-01-29

  - Since the missing tabs in the config dialog will also have a list of things
    at the left and details of the one current item of those things at the
    right, I tried to move out the common part of this as a base class. Since
    the things those config tabs manage have different types, I tried a C++
    template class. But **it turns out that in this year 2016 Qt's moc still
    does not support templates. WTF?!**

  - 21:00 (Grrrr) Okay, I found a workaround, albeit a pretty ugly one: Work
    with void pointers and forced type casts. Yuck. That's being bombed back to
    the early 1990s - we had to do this kind of stuff back with OSF/Motif in
    plain C all the time. Type safety was unknown back then; you could get all
    kinds of nasty surprises by casting pointers slightly wrong, and the
    compiler had no chance (even if it hadn't been that crappy SunOS C
    compiler, but a decent GCC) to catch any of this.

    25 years later, and we are still stuck with that kind of stone age
    programming - just because some tool never got ported to the 21st
    century. Sigh.

    Yet another day of development completely wasted due to insufficiencies of
    underlying tools. Just great. I am getting fed up with this.


- 2016-01-22

  - Improved usability of refreshing the tree after cleanup actions: They used
    to leave the tree behind with nothing selected, the branch the user just
    worked in closed (which is natural since it needed to be re-read from disk)
    and scrolled to another position - maximum disorientation for the user. Now
    the parent directory is selected, giving at least some hint where the
    action took place. It's not optimal yet, but much better than before.

  - Improved usability of the tree widget: When an item in the treemap is
    selected, all other branches in the tree are now collapsed before the new
    branch is opened. But this required working around some design flaws in the
    underlying _QTreeView_ class.

    **Rant:** Trolls, didn't it ever occur to you that if you are _constantly_
    using that _d->expandedIndexes_ from _QTreeViewPrivate_ in the _QTreeView_
    public class, derived widgets might need that information, too? There is
    **no way** to access the currently expanded items other than cheating in
    some creative way. Seriously, I am not going to duplicate that bookkeeping
    with the _expanded()_ and _collapsed()_ signals, always being off by some
    items or not getting the information that items were removed (or listen to
    half a dozen more signals for even more advanced bookkeeping). If a widget
    class cannot provide that kind of elementary information to its derived
    classes, it's poorly designed. Period.


- 2016-01-18

  - Applied Qt4 compatibility patches from Michael Matz. The only nontrivial
    issue was a thin wrapper around QProcess to make it store the program to
    execute and its arguments in the constructor and use those later with a
    plain start() without any more arguments.

- 2016-01-16

   - The MIME categories and the corresponding treemap colors can now be
     configured - see screenshot above. Yes, this is a real treemap widget as a
     preview for the colors, complete with a demo directory tree with a random
     number of files with random sizes (i.e. it looks different for each
     invocation). That part was the last major feature that was missing; now
     it's only little stuff that's left (still quite a number of it, though).

   - Treemap colors are now no longer fixed; there is now a rules engine called
     MimeCategorizer. It uses a new class MimeCategory that groups MIME types
     (by filename, not by magic numbers in the file) into broad categories like
     "Documents", "Videos", "Music", "Images". Each of these categories has a
     list of filename extensions that belong to it (".mp4", ".wmv", ".mov"
     etc. for "Videos", for example). The categorizer uses a very fast lookup
     map for the vast majority of the rules (simple file extensions), but it
     can also use more powerful wildcards wherever you like them.

   - The log file is now created per user: It's now /tmp/qdirstat-$UID.log,
     which for most Linux home users (with only one user account on the
     machine) is typically /tmp/qdirstat-1000.log .

- 2016-01-15

  - Added new macros for use within cleanups: %terminal and %filemanager. They
    are expanded to the terminal window or file manager application,
    respectively, of the current desktop (KDE, GNOME, Xfce, ...). I just
    wasted four hours (that could have been put to better use adding missing
    features - grrrr) because KDE's konsole misbehaves in every way possible
    (leading today's WTF count with 3):

    - It won't let me start it in the background from within QDirStat; it
      simply exits. I tried all kinds of command line arguments (--nofork,
      --hold), I tried to wrap it into a subshell, into the _nohup_ command -
      nothing helped. WTF?

    - It exits when QDirStat exits. Well, since it won't let me start it in the
      background, that's not too surprising. Still, if it does its own fork(),
      WTF?

    - It doesn't give a damn about the current directory you start it from, it
      needs its --workdir command line argument. WTF?

  - Added %d macro for cleanups: This is the directory name with full path. For
    directories, this is the same as %p. For files, this is their parent
    directory's %p.

  - %terminal : Terminal window application of the current desktop; one of

      - konsole
      - gnome-terminal
      - xfce4-terminal
      - lxterminal
      - eterm
      - xterm  (fallback)

  - %filemanager : File manager application of the current desktop; one of

      - konqueror
      - nautilus
      - thunar
      - pcmanfm
      - xdg-open (fallback)

  - Which desktop is used is determined by the _$XDG_CURRENT_DESKTOP_
    environment variable. Currently supported:

    - KDE
    - GNOME
    - Unity
    - Xfce
    - Lxde
    - Enlightenment
      (no clue how to start its file manager, though - using xdg-open here)

  - Users can override this with the _$QDIRSTAT_DESKTOP_ environment variable,
    so you can get, say, the Xfce terminal or file manager despite currently
    running KDE if you set

        export QDIRSTAT_DESKTOP="Xfce"

  - Of course, you can still simply use your favourite file manager if you
    simply use its command instead of %filemanager in the default "Open File
    Manager Here" cleanup action.

  - Added new standard cleanup actions:

    - Git clean. Start "git clean -dfx" in the current item's directory. This
      is relevant for developers or for people who regularly build software
      from Git repositories.

    - Clear directory contents. This removes everything inside a directory, but
      leaves the directory itself intact. ~/.thumbnails or browser cache
      directories are typical candidates for this with their ever-growing
      content: You probably want to keep the directory, but get rid of
      everything inside it.

  - Redefined the semantics of the _file manager_ cleanup action: It no longer
    tries to open files with the corresponding application depending on MIME
    type, it now always opens a file manager in that directory (which can open
    the file in its app if you want that). It's now also renamed to "open file
    manager here". If you still want the old behaviour, you can easily add your
    own cleanup action with the "xdg-open %p" command.

  - Added "Troubleshooting" section in this document.

  - Cleanup actions that have an icon are now added to the tool bar. Right now,
    only some of the predefined actions have an icon. There is currently no way
    for the user to specify an icon for a cleanup action.


- 2016-01-13 Added "move to trash", this time as a normal action in the "Edit"
  menu, the toolbar and the context menus. This is a real implementation of the
  XDG Trash specification. It does not rely on outside tools that might or
  might not be available.

- 2016-01-12 We have a first MacOS X port! Sonja Krause-Harder volunteered to
  install a Qt development environment (11 GB! Yikes!) on her MacBook. I would
  have expected some changes, but a simple "qmake; make" just did the
  job. Amazing! -- The major challenge was to find where "qmake" gets installed
  in that MacOS Qt environment.

- 2016-01-10 Went through the old KDirStat changelog and found a few bugs that
  I had fixed there -- and promptly repeated with the new QDirStat:

  - Disable cleanups while reading directories. This would result in a segfault.

  - No longer showing the path of the current directory being read. This sped
    up reading /usr on my machine from 9.5 sec to 1.5 sec (!).

- 2016-01-09 Cleaned up this README.md file. It had grown much too long.

- 2016-01-08 Cleanups are now configurable - see screenshot.

  - Cleanup actions can now be added, deleted, and moved up or down the list.
    There is no longer a hard limit to the number of cleanup actions; create as
    many as you like. Of course, your screen size is still the limit for those
    menus. ;-)

  - In addition to the old cleanup parameters, you can now configure the output
    window behaviour. The default is "show after timeout" with a timeout of
    half a second. This may sound pretty short, but I started with 3 seconds
    and found that it felt sluggish. A modern PC can get a lot of things done
    in half a second; yet waiting for more than that feels like an eternity. So
    if any action takes longer than that, an output window pops up. Of course,
    if there is any error, it pops up anyway.

  - You can now configure the shell to use. I was weighing the pros and cons of
    always using either /bin/sh or the user's login shell, and I found that
    there is no killer argument in favour or against either option. For
    example, I use the _zsh_, and while it's a great interactive shell, it did
    give me problems for that "remove junk files" cleanup: "rm -f *.o *.bak *~"
    -- when any of the wildcards cannot be expanded because there is no such
    file, it complains. Okay, you can wrap the whole command in "/bin/bash -c",
    but that's yet another indirection, so now you can configuare /bin/bash for
    that particular cleanup action. On the other hand, for some things I might
    want my original shell environment, so I do want my login shell by default.
    This is now the default behaviour: Try $SHELL (the user's login shell), and
    if that environment variable is not set or whatever is set there is not
    executable, it falls back to /bin/bash and then /bin/sh. And you can still
    enter your own in an editable combo box (but not "ruby" or "perl" because
    the "-c" option is still added automatically).

- 2016-01-05 I admit I had never really liked the way the output of cleanup
  actions was typically hidden. Most of the times I couldn't care less, but
  sometimes there were error messages that mostly went unnoticed - such as no
  permissions to convert a directory into a compressed tarball. Now we have
  something new: A process watcher window that looks very much like a terminal
  window. The commands and their output are displayed there: Normal output
  (stdout) in amber, error output (stderr) in red. It will be configurable for
  each individual cleanup action if this window is desired: You can choose to
  always open it, to not open it at all -- or to have it automatically open
  when there is any output on stderr. And there is also a checkbox to
  automatically close it when the cleanup process finishes successfully. This
  is all not 100% perfect yet, but it works quite well already.

- 2016-01-04 Cleanups are back. They still need some work, and you'll have to
  edit the config file in ~/.config/QDirStat/QDirStat.conf to configure
  anything, but they work.

- 2016-01-03 We are getting nearer to the target:

  - Settings are read from and written to the settings file in all relevant
    places. This includes what columns to display and their order. See section
    'Settings' below.

  - Refreshing the selected tree branch, continuing reading at mount points and
    at excluded directories now works.

  - Context menus are back. I am somewhat proud of the way my new ActionManager
    handles that with Qt introspection to avoid duplicating stuff: I wanted to
    keep the QActions in the Qt Designer file. That ActionManager keeps a
    pointer to the MainWindow that is the parent of all those QActions, and
    attached views can search that widget tree for action names (yes, that
    works only if you keep those QObject names tidy - which I am doing anyway).

  - Found some more icons for those actions.

  - Exclude rules are now greatly simplified. They no longer get the entire
    path to match which requires quite complex regexps, they only get the last
    path component - i.e., no longer "/work/home/sh/src/qdirstat/src/.git", but
    only ".git". You can now even tell the exclude rule to use a simplfied
    syntax: "FixedString" or "Wildcard" in addition to the normal "RegExp".

  - Still missing (this list is getting considerably shorter):
    - Cleanups
    - Settings dialog

- 2016-01-01  New Year release
  - Added mouse operations to the treemap. Some where there in the old kdirstat
    (but I guess not many users knew about them), some are new:

    - Left click: Select item and make it the current item.
    - Ctrl+Left click: Add item to selection or toggle selection.
    - Middle click: Select the current item's parent. Cycle back at toplevel.
    - Double click left: Zoom treemap in.
    - Double click middle: Zoom treemap out.
    - Mouse wheel: Zoom treemap in or out.

  - Reliably sort by pending read jobs while reading.

  - Fixed crashes if wildly clicking in the tree while reading. Yes, I
    know... "Doctor, it hurts when I do that!" - "Then don't do that."

- 2015-12-31  New Year's Eve release

  - Added the URL of the current item in the status bar at the bottom of the
    main window. If more than one item is selected, it displays the total sum
    of all selected items there.

  - Treemap zoom in / out / zoom reset works. The treemap now automatically
    zooms out if an item outside its current scope is clicked in the tree view.

  - Added more menu and toolbar actions and icons for many of them.

  - The treemap's red "current item" rectangle is now stippled instead of a
    solid line if the current item is not also selected (users can do that with
    ctrl-click).

  - Added "about" dialogs for the program and the used Qt version.

- 2015-12-30  Treemap rendering now works as expected, and selecting items
	      (including multi-selection with shift-click and ctrl-click in the
	      tree view and ctrl-click in the treemap view) works. It was a bit
	      of a challenge to avoid Qt signal ping-pong between the selection
	      model object and the two views.

- 2015-12-28  Treemaps are back. It's not perfect yet, but the basic rendering
	      works. I admit I was somewhat scared of that part, but the
	      transition from Qt3 QCanvas to QGraphicsScene / QGraphicsView
	      went much smoother than I had expected. I am very glad I don't
	      have to dig deep into the math again with those cushioned
	      treemaps; that part worked completely unchanged. :-)

- 2015-12-27  The tree view now supports _extended_ selection, i.e. you can
	      shift-click to select a range of items or ctrl-click to select or
	      deselect individual items. This was the most requested feature
	      for the last KDirStat. This means you can now select more than
	      one item at once to move it to the trash can etc. (once cleanup
	      actions are back).

- 2015-12-25  Christmas release

  - Sorting is now done internally in the DirTreeModel, and it's blazingly
    fast. It uses lazy sorting - postponing sorting until the last possible
    moment for each tree branch. Maybe the branch will never get visible, and
    then it doesn't need to be sorted at all. The QSortProxyModel is gone.

  - Reading cache files finally works again. It was quite some hassle to find
    all the places where the invisible root item that is required for the
    QTreeView / QAbstractItemModel make a difference. I hope now I caught all
    of them.

  - Fixed some bugs that resulted in segfaults. Well, it's a development
    version. Such things happen.

  - Removed the section about K4DirStat in this README.md; that information was
    outdated. It turns out I hadn't looked at the most recent sources of
    K4DirStat - that was entirely my own stupidity. My apologies. Since right
    now I don't have an informed opinion about K4DirStat, I preferred to remove
    that section entirely for the time being. Looks like K4DirStat is indeed
    ported to Qt5 now.

- 2015-12-20  First usable preview version - see screenshot above. It's still
	      pretty rough, and sorting via the QSortProxyModel seems to be
	      awfully slow once a number of tree branches were ever
	      opened. Looks like I'll have to do that myself, too.

- 2015-12-18  Found the crippling bugs that made the DirTreeModel do crazy
	      things. None of the Qt classes proved to be helpful to find that
	      - they just happily do the crazy things. That's what I call poor
	      design. Now there is a first working QDirStat with a useful
	      display tree, including icons (but no percentage bar graph yet).

- 2015-12-07  First working DirTreeModel -- still minimalistic, but working.

- 2015-12-06  Created tree model based on QAbstractItemModel.
	      Compiles, but dumps core so far.

- 2015-12-05  Imported and ported directory tree data classes.
	      Hammered through the compiler, but nothing usable so far.

- 2015-11-28  QDirStat project is being set up. Nothing usable so far.


## KDirStat

KDirStat is the predecessor to QDirStat. QDirStat is an 80% rewrite of the KDE3
based KDirStat.

KDirStat home page: http://kdirstat.sourceforge.net/

Sources: https://github.com/shundhammer/kdirstat

### KDirStat History

_(Incomplete list, just highlighting important releases)_

- 2006-06-01 KDirStat 2.5.3: The last KDE3 based version.

- 2006-01-08 KDirStat 2.5.2:

  - New: Read and write cache files

- 2005-02-22 KDirStat 2.4.4

  - New: Handle hard links and sparse files

- 2003-01-30 KDirStat 2.3.5

  - New: colored treemap

- 2003-01-05 KDirStat 2.3.3

  - New: Treemaps (monochrome only)

  - Communication between treemap and tree list view: Select an item in one
    view, and it is automatically selected in the other one, too.

- 2002-02-25 KDirStat 2.0.0

  - Complete rewrite for KDE 2 / Qt 2

  - Doing internal caching and (very) lazy creating of QListViewItems for
    improved performance and resource consumption

- 2000-01-21  KDirStat 0.86 for KDE 1 announced -- the first public version.
