# QDirStat Development History

This is more verbose than a traditional change log, thus the unusual name of
this file.

See the README.md file for the latest news:

https://github.com/shundhammer/qdirstat/blob/master/README.md


## QDirStat History

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

