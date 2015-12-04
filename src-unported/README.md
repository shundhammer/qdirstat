# QDirStat src-unported

Updated: 2015-12-04

This directory contains source files from the old KDirStat project that are
waiting to be ported to QDirStat.


## Intentionally Omitted

- KPacMan

- KFeedback

- KActivityTracker


KPacMan looked out of place pretty soon after it got to KDirStat. I know that
it does have its fans, but unless a graphics designer joins the project who can
do that right, this will not come back.

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

- Treemap stuff. This was based on Qt 3 QCanvas which is completely gone. From
  Qt 4 on, there is QGraphicsView / QGraphicsScene. There is no simple 1:1
  match between that and old QCanvas. This might be quite a challenge.

### Simple and Moderately Difficult Parts

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


- Cleanup Actions - not that hard, just work to do

- A shitload of menu entries and corresponding actions


### To Do Everywhere

- Namespace cleanup. The old classes all live in a namespace KDirStat. Not sure
  if that's really a good idea. Back then (in the early 2000s) I thought it
  might become a lib some day to be used from various apps, but that never
  happened.

- Get rid of the K prefix for everything - classes as well as files. It's not
  KDE any more. At the same time, replacing it with Q might be bad idea since
  Qt itself uses that everywhere.

- Filename cleanup. Files should be named like the class they contain, not all
  lowercase: ExcludeRules.cpp instead of kexcluderules.cpp.

- Move the file from src-unported/ to src/ when it gets ported. The state of
  this directory should reflect the things already done. The goal is to get
  the src-unported/ directory empty.

- When only single functions get ported, remove the original from its
  src-unported/ counterpart. This will hopefully avoid duplicate work.


### Done

- Get rid of the "Updated" header in all source files. Git keeps track of that
  stuff.

