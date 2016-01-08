# QDirStat
Qt-based directory statistics: KDirStat without any KDE -- from the original KDirStat author.

(c) 2015 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2016-01-08


## Overview

This is a Qt-only port of the old Qt3/KDE3-based KDirStat, now based on the
latest Qt 5.


## Screenshots

![Main Window Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png)

_Main window screenshot_

![Cleanup Action Output Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png)

_Screenshot of output during cleanup actions. Of course this window is purely optional._

![Cleanup Action Configuration Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-config.png)

_Screenshot of cleanup configuration._



## Current Development Status

**Alpha Quality -- V0.8**

Usable, but still Alpha.

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
    - Double click right: Zoom treemap out.
    - Double click middle: Reset treemap zoom.
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

- 2015-11-28  Project is being set up. Nothing usable so far.



## Motivation / Rant

Now that KDE has been losing direction more and more, it's time to come up with
a desktop agnostic version of KDirStat - one that can be used without the
hassle of an ever-changing desktop environment like KDE. KDE has become a
moving target for developers. Much effort has gone into creating eminently
essential things like a yellow sticky note desktop widget (a.k.a. _Plasmoid_)
that can be rotated by 7.834°, but basic usability has gone downhill during
that process.

With the latest Plasma 5 (as of late 2015), there are some KDE apps that use
KDE 5.x, and some that still use KDE 4.x, with the overall effect that settings
have been duplicated (systemsettings5 vs. systemsettings, anyone?), thus the
look and feel between those apps is different, and settings became unaccessible
for the end user (again: systemsettings5 vs. systemsettings).

Worse, users get treated to a mail client that uses a MySQL database back-end
called _Akonadi_. Not only is that the most fragile construction I've ever
seen so far, it also leaves half a Gigabyte worth of log files in my home
directory (!!) due to poor default configuration (both on Kubuntu and on SUSE,
probably also on all other distros). Argh. I found that the most important tool
for that stuff was the _akonadiconsole_ which I needed numerous times to fix a
borked mail environment. But I am a system developer. How on earth is a normal
user supposed to fix that kind of problem? A problem that wouldn't exist in the
first place if the developers hadn't chosen the most complicated and most
fragile combination of tools?

What was once a great desktop environment has become a playground for
developers who don't seem to talk to each other - and, worse, who don't give a
shit about their users anymore.

Time for KDirStat to move away from this insanity. Time to become independent.
KDirStat had never used that much of the KDE infrastructure to begin with; what
little it used (icons, for example) can be replaced by very little own
infrastructure. The KDE libs are just not worth the hassle anymore.

To put all that into perspective: I have been a KDE user since about 1998. I
liked it very much during the times of KDE 1, KDE 2 and KDE 3. Linux and KDE
are my main desktop (Windows only for gaming).

When KDE 4 came along, I tried it, but went back to KDE 3 - back and forth
several times. KDE 4 stabilized somewhat over recent years, but it still has
its quirks -- a lot of them. Too many in my opinion. And with Plasma 5, for me
it clearly went over the top. There may be people who like it, but I am not
among them.


### New Stuff

- The tree view now supports _extended_ selection, i.e. you can shift-click to
select a range of items or ctrl-click to select or deselect individual
items. This was the most requested feature for the last KDirStat. This means
you can now select more than one item at once to move it to the trash can
etc. (once cleanup actions are back).

- Icons are now compiled into the source thanks to Qt's resource system; now
it's just one binary file, and nothing will go missing. No more dozens of
little files to handle.

- The build system is now Qt's _QMake_. I got rid of that _AutoTools_
(Automake, Autoconf, Libtool) stuff that most developers find intimidating with
its crude M4 macro processor syntax. QMake .pro files are so much simpler, and
they do the job just as well (and no, it will definitely never be that fucking
_CMake_. I hate that stuff. It's just as much as a PITA as the AutoTools, just
not as portable, no usable documentation, changing all the time, and those
out-of-source builds drive most developers crazy).

- QDirStat now has its own log file. Right now it logs to /tmp/qdirstat.log,
but this will probably change in the future. No more messages on stdout that
either clobber the shell you started the program from or that simply go
missing.

- It should still compile and work with Qt4. I didn't test it, nor is this a
supported scenario. But given the emphasis on QML / Qt Quick with Qt5, not much
changed in the general area of item views or the other widgets used here. If
anybody needs Qt4 support and is willing to invest the time, (reasonably small)
patches for Qt4 compatibility are welcome -- see also section "Contributing"
below.


### Promised New Features

- There will no longer be a limit on cleanup actions. Users will be able to add
new ones to their hearts' content. In the old KDirStat, there were 8
predefined and 10 user cleanup actions.

- Treemap colors, the respective file categories (pictures, videos, compressed
archives, ...) and their filename extensions (*.jpg, *.png; *.mp4, *.wmv, ...)
will be configurable.


## Settings

Stuff that can be configured in ~/.config/QDirStat/QDirStat.conf:

    [Cleanup_01]
    Active=true
    AskForConfirmation=false
    Command=xdg-open %p
    Hotkey=Ctrl+K
    ID=cleanupOpenInFileManager
    Icon=:/icons/file-manager.png
    OutputWindowAutoClose=false
    OutputWindowPolicy=ShowAfterTimeout
    OutputWindowTimeout=0
    Recurse=false
    RefreshPolicy=NoRefresh
    Title=Open in &File Manager
    WorksForDir=true
    WorksForDotEntry=true
    WorksForFile=true

    [Cleanup_02]
    Active=true
    AskForConfirmation=false
    Command=x-terminal-emulator --workdir %p
    Hotkey=Ctrl+T
    ID=cleanupOpenInTerminal
    Icon=:/icons/terminal.png
    OutputWindowAutoClose=false
    OutputWindowPolicy=ShowNever
    OutputWindowTimeout=0
    Recurse=false
    RefreshPolicy=NoRefresh
    Title=Open in &Terminal
    WorksForDir=true
    WorksForDotEntry=true
    WorksForFile=false

    ...
    ...

    [DataColumns]
    Columns=NameCol, PercentBarCol, PercentNumCol, TotalSizeCol, OwnSizeCol, TotalItemsCol, TotalFilesCol, TotalSubDirsCol, LatestMTimeCol

    [DirectoryTree]
    CrossFileSystems=false
    TreeIconDir=:/icons/tree-medium/
    UpdateTimerMillisec=333

    [ExcludeRules]
    Rules\1\CaseSensitive=true
    Rules\1\Pattern=.snapshot
    Rules\1\Syntax=FixedString
    Rules\size=1

    [MainWindow]
    ShowTreemap=true
    StatusBarTimeOutMillisec=3000
    WindowPos=@Point(-2 0)
    WindowSize=@Size(1157 650)

    [OutputWindow]
    CommandTextColor=#ffffff
    DefaultShowTimeoutMillisec=500
    StdErrTextColor=#ff0000
    StdoutTextColor=#ffaa00
    TerminalBackground=#000000
    TerminalFont="DejaVu Sans Mono,10,-1,5,50,0,0,0,0,0"

    [PercentBar]
    Background=#a0a0a0
    Colors=#0000ff, #800080, #e7932b, #047100, #b00000, #ccbb00, #a2621e, #009492, #d95e00, #00c241, #c26cbb, #00b3ff
    PercentBarColumnWidth=180

    [Treemaps]
    AmbientLight=40
    CurrentItemColor=#ff0000
    CushionGridColor=#808080
    CushionShading=true
    DirFillColor=#107db4
    EnsureContrast=true
    FileFillColor=#de8d53
    ForceCushionGrid=false
    HeightScaleFactor=1
    MinTileSize=3
    OutlineColor=#000000
    SelectedItemsColor=#ffff00
    Squarify=true


There will not be a settings dialog for all that; some things are just too
arcane for normal people to configure (the treemap parameters, for example).

As you can see, the data columns are already configurable: Except for the
NameCol (where the file or directory name is displayed), everything can be
omitted or rearranged to your liking.


### Windows / MacOS X Compatibility

None yet. Right now, there is only one DirReadJob subclass that uses POSIX
function calls like opendir(), closedir(), lstat(). There should be a
reasonable way to do that on MacOS X since it's a BSD Unix at heart, but I
don't have a Mac to try this.

It should be fairly simple to use plain Qt calls for most of this, but one
feature will clearly go missing then: Stopping at file system boundaries while
scanning a directory tree. If your root filesystem fills up, you need to know
what stuff causes this, not what other fileystems are mounted there. This had
been the single most requested feature of the very first KDirStat, and I
rewrote most of it to make this happen.

The QFileInfo class gives a lot of information about a file, but not the device
(major and minor device number) the file is on, and that's the only way to
figure out if a file or directory is still on the same filesystem as its
parent. And I don't know of a Qt class that would give me that information, so
at that point, it's down to low-level (and non-portable) system calls again.

With Windows, this is probably not a problem, since all directory tree scans
are limited to one drive. Until the day, of course, that Microsoft finally
discovers the magic of having one large integrated filesystem tree, like we
Linux / Unix users have been using since the early 1970s. ;-)

So it might come down to that some day: Use POSIX calls on platforms where they
are available, and QDir / QFileInfo for everything else.


_Update 2015-12-25:_ Suy (Alejandro Exojo) suggested to use QStorageInfo
(available since Qt 5.4) to find out platform-independently on what device a
directory is. This doesn't sound too bad. This is something worthwhile to
investigate in more detail.


## Building

### Build Environment

Make sure you have a working Qt 5 build environment installed. This includes:

- C++ compiler (gcc recommended)
- Qt 5 runtime environment
- Qt 5 header files
- libz (compression lib) runtime and header file


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

The project is still in its early stages, but contributions are welcome.


### Contribution Guide Lines

Use the same style as the existing code.

Indentation is 4 blanks. One tab is 8 blanks. Do not indent with single tabs
for each indentation level and thus fuck up everybody else's editor. Letting
the editor automatically replace 8 blanks with a tab is okay, but indentation
level is still 4 blanks.

Brace style is braces **always** on a new line. Use blanks after opening and
before closing parentheses:

    if ( someCondition )
    {
        doSomething( arg1, arg2 )
    }

**No** K&R style indentation:

    if (someCondition) {         // WRONG!!!
        doSomething(arg1, arg2)  // WRONG!!!
    }

Use blank lines liberally. No Rubocop-style code.

Use CamelCase for classes and camelCase for variables. Do not use
snake_case. Prefix member variables with an underscore: _someValue. Prefix its
setter with Set...(), use only the name (someValue()) for the getter, **Not**
getSomeValue():

    private:
        SomeType _someValue;    // member variable

    public:
        const & SomeType someValue() const;              // getter
        void setSomeValue( const SomeType & newValue );  // setter

Use a const reference for the setter parameter and the getter return type for
nontrival data types (everything beyond a pointer or an int etc.), and the type
directly otherwise. Use your common sense.

Use Qt types whereever possible. Do not introduce types that are also available
in a Qt version. In particular, do not use STL or Boost types unless there is
no other reasonable way.

For Qt dialogs, use Qt Designer forms whenever possible. I know that many Qt
developers don't like Qt Designer (typically based on bad experiences years
back), but it's really worthwhile to use it. This avoids having tons of dumb
boilerplate code just for creating widget trees.


### Documenting Classes and their Methods

Document classes and their methods in their header file. Document them
immediately. If you don't do it right away, it will never happen. Don't wait
for somebody else to do that for you - nobody will be willing to do the
inconvenient part of the job for you.

Undocumented stuff will mercilessly be rejected.

In my 30+ years of programming I came to the conclusion that it helps immensely
to write down a function name in the header file and **immediately** write its
documentation:

- What does it do? (Do not just repeat the function name! **Explain** what it does!)
- What are the parameters?
- What does it return?
- In particular for functions returning bool: Which case returns 'true', which
  case returns 'false'?
- What happens in fringe cases? (returning NULL pointer?)
- If the function creates any objects: Who owns them?
- Does the function transfer ownership of any objects it gets pointers to (as
  parameters) to itself or wherever?

If you do that right, you might as well leave the implementation to somebody else.


**Do not** insert any doc template without content before each function.
This is worse than no documentation at all: Everybody has to wade through tons
of useless empty forms that don't contain any content whatsoever.

Been there. Done that. Hated it.

Be careful when copying and pasting documentation from some other place.
Misleading documentation is worse than no documentation at all.


### Legalese in the Sources

**Do not** copy 30+ lines of legalese bullshit into any source file. One line
in the header like

    License: GPL V2 - see file LICENSE

is plenty. Seriously, what are those people thinking who put all that legalese
into source files? Sure, they listened to spineless corporate lawyers who just
want to make sure. But those lawyers are not going to have that crap smack up
their faces every time they open a file for editing. We developers do.

You lawyers out there, can you hear me? **This crap is in the way! Nobody wants
to read that!** It's in the way of people trying to do their jobs! We could
construct a harassment case from this!



### Getting Help for Contributors

- Use the Qt reference documentation.
- Install and study carefully the available Qt examples for reference.
- Use Stack Overflow.
- Use Google.
- Make sure you can build a basic Qt program.
- Make sure you have the compiler and other developer tools set up correctly.
- Make sure you have the relevant -devel (SUSE) or -dev (Ubuntu/Debian)
  packages installed.

If you did all of the above (and only then!) and still couldn't figure out
what's wrong:

- Use IRC (#qdirstat on irc.freenode.net; fallback: contact HuHa in #yast on
  freenode IRC)
- Write a mail

It is a matter of professional courtesy to first use what is freely available
on the web before you consume somebody else's time and expertise. It is utterly
rude to let somebody else point you to the exact part of the Qt documentation
you couldn't be bothered to read.

Does this sound unfriendly? Well, maybe - but: Been there, done that, got the
fucking T-shirt -- countless times. This is not how I want to spend my spare
time.

There are some people who keep arguing that "it takes hours for me to read
through all the documentation, whereas you as an expert can give me the right
answer within a minute".

Well, yes, it might only take a minute to write the answer in IRC, but then
that newbie won't learn anything except that it pays off to disturb people
rather than learn for himself. And reading the documentation is always a good
investment of time; it is never wasted. In the process of searching for an
answer you will come across many things you hadn't known - and that might just
be useful for your next question.

Plus, it will take the expert that one or two minutes to write the answer to
IRC - and then **15-20 minutes to recover from the interrupt**, to restore the
fragile buildings in his mind he needs to solve the problem he was originally
working on.

Please keep in mind that every such interrupt will cost 20-30 minutes of time
on average, and a working day only has 16-24 such time slots; and those experts
tend to be in high demand on many different communication channels (IRC,
personal mail, mailing lists, bugzilla, phone, people walking into the office).

So, should you join the project as a newbie?

If you have patience, self-discipline and are willing to learn (which includes
reading the Qt documentation first), and, most importantly, if you are willing
to stay with the project and not let some unfortunate guy clean up half-ready
stuff, yes.

Just keep in mind that others are doing this because (a) it's fun for them
and/or (b) because they want that piece of software to be successful. Educating
newbies and cleaning up after them is very low on most developers' list of fun
things to do. They still do it, but it's painful for them. Please help
minimizing that pain.


## Reference

### Original KDirStat

Home page: http://kdirstat.sourceforge.net/

Sources: https://github.com/shundhammer/kdirstat


### K4Dirstat

Home page: https://bitbucket.org/jeromerobert/k4dirstat/wiki/Home

Sources: https://bitbucket.org/jeromerobert/k4dirstat/src
