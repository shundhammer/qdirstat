# QDirStat Development History

This is more verbose than a traditional change log, thus the unusual name of
this file.

See the README.md file for the latest news:

https://github.com/shundhammer/qdirstat/blob/master/README.md


## QDirStat History

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

    Cache files containing the root file system would not display correctly or
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

  - More graceful handling for nonexisting paths specified on the commmand
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

    Yet another day of develpment completely wasted due to insufficiencies of
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
    respectively, of the current desktop (KDE, GNOME, Xfce, ...).  I just
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
    window behaviour.  The default is "show after timeout" with a timeout of
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
	      works.  I admit I was somewhat scared of that part, but the
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
	      things.  None of the Qt classes proved to be helpful to find that
	      - they just happily do the crazy things. That's what I call poor
	      design.  Now there is a first working QDirStat with a useful
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

