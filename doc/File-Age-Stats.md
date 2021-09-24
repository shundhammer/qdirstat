# The QDirStat File Age Statistics Window

See also [GitHub issue #172](https://github.com/shundhammer/qdirstat/issues/172).

QDirStat can break down the _age_ of files (based on their modification time)
in a subtree by year and, for the current year and the year before, by
months. This is strictly for files; directories, symlinks etc. are disregarded.

This is a new view opened from the menu with _View_ -> _Show File Age
Statistics_ or with the `F4` key. If a directory is selected, it starts with
that directory, otherwise with the complete subtree.

![File Age Statistics: Years](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-years.png)

For each year, it shows:

- The number of files that were last modified in that year

- The percentage of those files relative to the selected subtree, both as a
  percent bar and as a number

- The total size of files that were last modified in that year

- The percentage of those files relative to the selected subtree, both as a
  percent bar and as a number.



## Breaking Down to Months

For the current year and the year before, you can expand the months (click on
the little arrows on the left).

![File Age Statistics: Years and Months](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-months.png)


## Directories with no Activity for some Years

![File Age Statistics: Years and Months](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-age-months.png)

The list always starts with the current year, even if there was no activity in
the subtree in that year (i.e. no file was modified in that year). All inactive
years up to the last year with any activity are displayed.

This helps to quickly see at a glance that there was a while with no activity,
even without reading the year numbers.


## Drilling Down Deeper into Subdirectories

Simply leave the _File Age Statistics_ window open and click on another
directory in the main window's tree view, and the _File Age Statistics_ window
is automatically updated with the data for that directory.

You can also switch that behaviour off when you uncheck the _Sync with Main
Window_ check box at that bottom of the _File Age Statistics_ window. In that
case, you can always simply hit the `F4` key again to update the window.


## Locating Files from that Year / Month

If there are no more than 1000 files in the selected year or month, you can use
the _Locate_ button to open another pop-up window that lists those files. Click
on one of them to select it in the main window; the main window scrolls to that
branch, selects the file and updates the _Details_ panel with it.

This is limited to 1000 files because it becomes very unwieldy at some point,
and performance suffers heavily. If you find yourself wanting to see more than
1000 files in a subtree, break it down to deeper subdirectories (see above).


## Use Case

This view was inspired by the discussion in
[GitHub issue #165](https://github.com/shundhammer/qdirstat/issues/165)
where an admin requested this for his users that had accumulated large amounts
of data and lost their overview.

It had become necessary to move some of those data to archive media, but the
users had lost the overview what was current and what was not, i.e. which parts
were good candidates to be moved to archive media.

Imagine a large machine with a large storage array used by a great number of
scientists collecting research data over many years. Researchers come and go;
new ones take over their projects and their data for their research. Others
leave when they are finished with their degree. Even with a best effort of
documentation (which is wishful thinking to begin with), after some time it
becomes murky what set of research data is what, and which of them are in
active use. Somebody new will not dare to get rid of anything; even if that
only means moving it to archive media where it's less easily accessible.

While this _File Age Statistics_ is by no means a general solution, it can
contribute to regain some kind of overview: If a directory branch has been
completely unmodified for many years, this is an indication that it may not be
in active use.

Of course, this is not guaranteed: It is very well possible that they are
actively using the data sets from 1995, 2000, this year and the year before,
and the old data sets are a reference that does not change. Only the users can
really tell. But the file age may give them additional hints.


## Usage Hints

It depends on the use case which columns in the _File Age Statistics_ are more
important: The number of files in that year (or month) or their total
size. That's why both of them are displayed.

When it comes to the relevance of data in a subdirectory, the sheer number of
files may be important: Even a lot of small files that accumulated may distract
and get in the way, especially when scripting (`find` commands come to mind)
over large directory trees.

When that is not an issue, but disk space is running out, of course the total
size of those files may be more important.

One use case is showcased in
[GitHub issue #172](https://github.com/shundhammer/qdirstat/issues/172):

A photo collection that grew over the years. In that case, the number of files
is much more important than their total size: During those years, camera
technology rapidly advanced, moving from 3 Megapixels up to 10, then 12, then
24. That means that photos taken in earlier years with considerably less
Megapixels are of course much smaller, so for photo activity during those years
the size is not a good measure; but the number of photos per year is.


### Get an Overview Quickly

Start at the top of the subtree. Arrange the windows so you can see both the
important part of the main window and the _File Age Statistics_ window.

If the _File Age Statistics_ for the toplevel don't give a good overview (which
is likely), click on its first subdirectory in the main window's tree
view. Watch the _File Age Statistics_ window.

Then use the _Cursor Down_ key in the main window to go to the next
subdirectory. Watch the _File Age Statistics_ window; but it's enough to give
it just a glance: Watch how the inactive years (the greyed-out entries)
change. Move to the next subdirectory etc.; you will now have a first
impression on the age distribution among that first subdirectory level.

If that also doesn't shed much light, go to the next deeper subdirectory level
in the main window. Since the main window's tree view orders the directories by
size by default, starting from the top is most promising.

If repeating that over several subdirectory levels still doesn't give you an
overview, this may not be the right tool for the job; it's not a catch-all
solution for every situation.


## Related Discovery Actions

Don't forget that there are also actions in the _Discover_ menu to immediately
find the oldest and the newest files in a directory tree. This is meant for
individual (or at least small numbers) of very old or very new files.


## Related Main Window Tree View Columns


### Last Modified

In the main window, the _Last Modified_ tree column shows the latest
modification time in that subtree, but in _including_ directories and
symlinks. That may or may not be what you need; it depends on the task.

Notice that a directory's modification time is updated whenever a file is
created or deleted there, or whenever a file is moved to or out of that
directory.


### Oldest File

This may be little known since it is not enabled by default, but there is a
similar column _Oldest File_ in the main window's tree view that shows the
modification time of the oldest file (not directory, not symlink) in that
subtree. This may be useful to drill down the tree view for very old files.

To activate that column, switch to an appropriate main window layout
(preferable L2 or L3), right-click on the headers of the tree view to open the
context menu, then select _Hidden Colunns_ -> _Show Column "Oldest File"_.

This functionality may be superseded by the relatively new _Show Oldest Files_
action in the _Discover_ menu.



## Reference

- [GitHub issue #172](https://github.com/shundhammer/qdirstat/issues/172)
- [GitHub issue #165](https://github.com/shundhammer/qdirstat/issues/165)
