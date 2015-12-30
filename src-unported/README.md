# QDirStat src-unported

Updated: 2015-12-31

This directory contains source files from the old KDirStat project that are
waiting to be ported to QDirStat.


## Intentionally Omitted

- KPacMan

- KioDirReadJob

- KFeedback

- KActivityTracker


KPacMan looked out of place pretty soon after it got to KDirStat. I know that
it does have its fans, but unless a graphics designer joins the project who can
do that right, this will not come back.

KioDirReadJob depended on KIO slaves to read directories on remote servers -
FTP, HTTP, even Fish (ssh-based). This functionality is gone without
KDE. That's a pity, but this is a little price to be paid to avoid the rest of
the hassle with using the KDE libs.

KFeedback was that form where users could tell their opinion about
KDirstat. But that was not used that often anyway - not nearly enough to
justify the effort that has gone into that part. And the KDE usability people,
like usability people generally tend to do, first discussed that to death and
then decided they didn't want anything like that in general in KDE
applications. So be it.

KActivityTracker was a supporting class for KFeedback that kept track of how
much a user was using the program and after a while (when it was determined
that it made sense) asked if the user wouldn't like to give his feedback about
the program.

Don't you all just hate those dumbass web designers who tell you to do a survey
how much you like their grand web page before you even had a chance to look at
it? Shove a pop-up up your face covering the stuff you are interesting in with
their self-loving marketing bullshit? -- KActivityTracker was made to avoid
exactly this: Ask the user only once you know that he actually used the program
for a while.


## To Do

### The Hard Parts

**Done.**

- Model/view for the dir tree view: Done.

- Treemaps: Done. This went much smoother than I had expected (see below).

That means no less than the hard parts are all done; from now on, it's just a
lot of moderately difficult or simple stuff, but still lots of that.

I don't see any more possible showstoppers ahead. Model/view had almost become
one since those classes don't give any support whatsoever for developers to
find problems, they just happily do crazy stuff if anything (however minor) is
wrong. There are no diagnostics at all, and the famed "model test" is pretty
useless for a dynamic tree model that is displayed while being built.

That means no less than that the road to an official QDirStat 1.0 release is
free (albeit still somewhat bumpy).


### Simple and Moderately Difficult Parts

- Tons of menu entries and corresponding actions

- Config dialog

  - Should be based on Qt Designer forms

  - Check if tab pages are a good option, or if it should be a selection widget

  - Maybe use a separate designer form for each tab page

  - Definitely do not limit the number of cleanup actions any more:
    - Button for "New cleanup"
    - Button for "Delete cleanup"
    - Buttons to change the sort order (up/down)
    - Maybe open a separate popup dialog to edit a cleanup action

  - Definitely add config option to "force English" - as much as translations
    are welcomed, I do not agree to translate everything to death, in
    particular when the English term is much more widely used. This happens a
    lot in German translations. I don't want to be at the mercy of the German
    translator; I want to be able to use my original English texts -- without
    switching my entire desktop and shell environment to English or the POSIX
    locale.

  - Config dialog page for exclude rules

  - Config dialog page for treemap colors: Map MIME types to colors

  - Not sure if all those crazy treemap parameters need to be user
    configurable. They are pretty much expert-only, and most of them look
    pretty scary to normal people. I might leave them in the config file, but
    most likely anybody who really wants to change any of them will need to use
    his favourite editor.

- Cleanup Actions - not that hard, just work to do

- Add translation stuff. Setting up the Qt translator classes is easy
  enough. The translation file(s) will probably also end up in the resource
  file to avoid being dependent on external files (they can always go
  missing). But then, since it's not KDE any more, the number of supported
  languages will be really small: I can supply a German translation, and maybe
  I can talk my Czech, Spanish and maybe Russian colleagues into contributing
  their languages, but there won't be many more than that.

- Online help. I know that most users can't be bothered to read that stuff
  anyway, but I spent a lot of work for the old help files; porting them should
  be easy enough. But that will definitely be only for English and maybe (!)
  for German. There is no way to maintain that much text in any more languages.

- When all obvious features are back, look through src-unported/ if anything is
  still missing. After that, get rid of src-unported/.

- Find more icons for all kinds of actions.


### Done

- Get rid of the "Updated" header in all source files. Git keeps track of that
  stuff.

- Filename cleanup. Files are now named like the class they contain, and not
  all lowercase: ExcludeRules.cpp instead of kexcluderules.cpp.

- Tree model based on QAbstractItemModel. This can be connected to a number of
  Qt's view classes, in particular QTreeView.

- Treemap stuff. This was based on Qt 3 QCanvas which is completely gone. From
  Qt 4 on, there is QGraphicsView / QGraphicsScene. I had feared that would be
  quite a challenge, but this went much smoother than I had expected.

- Common selection model for both the tree and the treemap view with
  multi-selection for both -- and synchronizing the selection with the
  respective other view.
