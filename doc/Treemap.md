# The Treemap in QDirStat

## Treemaps in General

In general, a treemap is a graphical method to visualize a hierarchical
structure, also known as a _tree_. It shows the whole tree at once with each
item of the tree coresponding to a rectangle in the treemap.

What makes the treemap special is that the area (the size) of each rectangle
corresponds to a _value_ of the item, so you can instantly see how any
rectangle compares to the others.

In the context of disk space, that value is of course the disk space used by
that item.

But it's not limited to that: For example, in a treemap of countries and their
respective military spending, the area of a treemap item can correspond to that
military spending.

A sales organization could use it to visualize the revenue by business units,
departments and teams; or by product groups and products.

It is important to note that the grouping follows the hierarchy, so the
business units or departments remain visible: Their total revenue is the total
of their subunits which are clustered together.


## QDirStat's Treemap


[<img width="900" src="https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png">](https://raw.githubusercontent.com/shundhammer/qdirstat/master/screenshots/QDirStat-main-win.png)

_Main window screenshot: Tree view (upper half), treemap (below), details panel (right)._


QDirStat is about disk usage, so it uses the disk usage (the size) of each file
or directory for the area of each treemap rectangle (_tile_).


## Colors

But showing the sizes is not all: The treemap also uses colors to show the _type_
(the _MIME category_) of each file.

By default, documents are displayed in blue, images in cyan, music in yellow,
videos in light green; executable files in magenta, libraries in orange etc.,
but you can configure that to your own liking: The number of categories and
what you want them to represent, the filename extensions, the color. See menu
"Edit" -> "Configure" -> tab "MIME Categories".

That means that you can get a rough idea how your disk space is used just by
looking at the colors.


## Connected Views

If you spot something that makes you curious what it is (for example a
particularly large rectangle), click on it: It will be selected in the treemap
(notice the red frame around it), in the tree view above (it will scroll to
that item and open its parent branches), and the details panel on the right
will show details about it.

It works the same when you select an item in the tree view: The corresponding
treemap tile will get the red selection frame, and the details panel will show
details about it.


## Actions: Menus, Context Menus, Toolbar

When an item is selected, no matter in which view (the tree view or the
treemap), you can start _actions_ on it: Right-click to open the context menu,
or use one of the small icons in the _toolbar_ below the main menu menu; or one
of the menus of the menu bar. The most interesting one in this context is the
"Cleanup" menu where you can move the current item to the trash (which can be
undone from your desktop's trash can icon), delete it immediately (which cannot
be undone), or lots of other actions that are more or less destructive.

There are also non-destructive actions to help you inspect the item in more
detail; for example "Check File Type" which starts the `file` command for this
item and shows you the result. Or you can start a file manager in that
directory, or a terminal window.

You can configure those cleanup actions. Use menu "Edit" -> "Configure" -> Tab
"Cleanup Actions".


## Treemap Interaction: Zoom, Select, Multi-Select

Select a treemap tile and use your mouse wheel to zoom in and out again: Each
click on the wheel zooms the treemap one directory level further in or out.

Alternatively, use `Ctrl`+`+` and `Ctrl`-`-` like in all modern Internet
browsers, and `Ctrl`+`0` (digit zero) to reset the zoom. You can also use the
toolbar actions with the magnifying glass icons.

Clicking on a treemap tile selects it, and it gets a red selection frame. Use
`Ctrl`-`Click` to add or remove one more tile to the selection; you can start
an action for all of them at once. The other selected tiles get a yellow
rectangle marker.

If you right-click a treemap tile, that will always select it first and _then_
open the context menu. If you have a multi-selection, make sure the mouse
cursor is actually on the last selected tile, or hold the `Ctrl` button before
opening the context menu, otherwise the previous selection may be gone.


## Showing the Hierarchy

In a deeply nested directory tree, it' sometimes interesting to see in the
treemap what belongs to the directory of the currently selected item, and what
does not.

Use the _middle_ mouse button to select, and you will see white rectangles
around the current item's parent, grandparent etc.; up to almost the top
level. Everything outside that tree branch will be dimmed out a bit.

Click outside the current branch (in the dimmed-out area) to get it back to
normal without white frames and without anything dimmed out.

You can use the middle mouse click pretty much like a left click in the
treemap; use `Ctrl` to extend the selection.


## Moving Subwindow Borders

Drag on the divider between the tree view and the treemap to give either one
more or less space. Your last settings are stored in the QDirStat main configuration
file at `~/.config/QDirStat/QDirStat.conf`.


## Hiding the Treemap Completely

Sometimes it's useful to get rid of the treemap momentarily to have more screen
space for the tree view. Use the `F9` function key to quickly toggle it off or
on.


## Getting Rid of the Tree View

Some (few) users wanted to use _only_ the treemap and get rid of the tree view
completely.

This is technically possible (just drag the divider between the two views all
the way up), and sometimes it may be useful to see a particularly aesthetically
pleasing treemap in a better resolution and with less distracting GUI elements.

But it is strongly advised not to do this for normal work: The primary view is
the tree view. It and the "Details" panel on the right (which would be gone as
well) show all the detailed information that QDirStat is all about.

The treemap is intended as a secondary view which is no doubt very useful, but
it has its limitations. For example, you only see the full path and the size of
the currently selected item there, that's it.

If anybody finds that sufficiently useful to work with QDirStat on that basis
alone, that is their choice, but any complaints about missing information or
functionality that are now no longer accessible will be rejected.


## Fading to Grey: MinTileSize and Lots of "Little Stuff"

In a very large directory tree with many (10,000 or 100,000 or more) files, the
scaling will make some treemap tiles very small; a 4k file doesn't get much
visibility in a 1.2 GB home directory.

QDirStat's treemap layout algorithm "cuts off" treemap tiles below a certain
size: `MinTileSize` in the config file, by default 3. There is not anything
useful to render in a tile that small; it would only become pixel mush.

So some directories contain a partly grey area to their bottom right: That is
where those tiny tiles would go, but it would be useless to render them; they
are too small to be identified or to be clicked.

This is not a graphical error, it's a feature to boost performance, and to
prevent you from accidentially clicking on something that you have no way of
knowing what it is.

The information that such a grey area gives you is that there is a lot of
little stuff that you can't see on that zoom level, so zoom in if you are
really interested.

To make this a bit more deterministic, select any other tile on that directory
level, then use the "Up one level" action (Icon: Arrow up; key: `Alt`+`Cursor
Up`) to select the parent directory, then zoom in all the way (mouse wheel or
`Ctrl`+`+`).


# Use Cases

## Fat Blobs

If you see any very large tiles, click on them and check what they are. Watch
the "Details" panel. If you are in a system directory, it will check if it's
probably a system file, and what installed software package it belongs to, if
any.

If it's in your home directory, you might rediscover that _CoolLinux.iso_
installation ISO image that you downloaded some time ago, then forgot about
it.

The nice thing about this is that it doesn't matter at all how many levels deep
in the directory hierarchy it's hiding away: You can see it instantly without
opening any branches in the tree view. That one click on the fat treemap tile
will do that for you, and then you can decide if you want to keep it, get rid
of it, move it to a better location, compress it or whatever.


## Where did my Download Go?

We've all been there... You click on something in your Inernet browser, and it
starts to download something; a movie or whatever. OK, so let's watch
it. But... where did it go? Why doesn't the browser _ask_ me where to store it?

Solution: Start QDirstat and select your home directory. Chances are that now
you have big fat bright green blob somewhere deep in some obscure path below
your home directory. That's your new movie; open a file manager there and move
it to a more suitable location so you have realistic chance to find it again.


## Browser Caches Everywhere

In these modern days, you need several different Internet browsers for
different tasks; while one web app only wants to work with Firefox, another one
insists on Chrome / Chromium. And if your favorite browser is Opera, you have
to cope with three of them. Ah yes, and Thunderbird as a mail client which is
also just another incarnation of Mozilla, just like Firefox.

And every single one of them insisits on creating its own cache directories,
and every single one of them consumes at least 50 MB, sometimes 500 MB. And
none of them is content with only one cache directories: They have several
caches for web pages, for images, for Javascript code, for their startup, for
thumbnails. And they are spread all over the place; not only in `~/.cache/`
which is designed for that exact purpose, no, they also pollute `~/.config/`,
and sometimes directories directly below your home directory.

It would be quite okay if their "clear caches" command would actually empty
them all, but none of them does that; they empty _some_ of them, but never all.

That makes backing up the home directory a royal PITA: You'll never see what
your carefully crafted `rsync` command actually does; its output will always be
polluted by a gazillion lines about all the cruft in those cache directories.

What you can do is to start collecting bookmarks of such directories and then
go to each one and clear its content.

You can identify cache directories by a name containing "Cache", and by the
visual appearance in the treemap: If you found a very large area with a large
number (1000 or more) of grey tiles (e.g. they cannot be identified what they
are) of similar size that belong together (which the white borders after a
middle-click will show you), you may have spotted a cache directory.

It looks like you can actually spot a cache directory from outer space (ok,
from a near earth orbit) with a treemap.


# Further Reading

- [Wikipedia: Treemapping](https://en.wikipedia.org/wiki/Treemapping)

- [Ben Shneiderman: Treemaps @ UMaryland](http://www.cs.umd.edu/hcil/treemap-history/index.shtml)

- [SequoiaView @ TU Eindhoven, NL](https://www.win.tue.nl/sequoiaview/) (for Windows)

- TU Eindhoven, NL [PDF Download](https://www.win.tue.nl/~vanwijk/ctm.pdf)
  "Cushioned Treemaps" paper by van Wijk, van de Wetering @ TU Eindhoven, NL from 10/1999

- [PDF Download](https://www.win.tue.nl/~vanwijk/stm.pdf)
  "Squarified Treemaps" paper by Bruls, Huizing, van Wijk @ TU Eindhoven, NL

