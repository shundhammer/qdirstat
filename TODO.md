# TO DO


## Config Dialog

- (Done) Should be based on Qt Designer forms

- (Done) Maybe use a separate designer form for each tab page

- Check if tab pages are a good option, or if it should be a selection widget

- Data columns to display + order (NameCol is alwas there and #1)


- Definitely add config option to "force English" - as much as translations are
  welcomed, I do not agree to translate everything to death, in particular when
  the English term is much more widely used. This happens a lot in German
  translations. I don't want to be at the mercy of the German translator; I
  want to be able to use my original English texts -- without switching my
  entire desktop and shell environment to English or the POSIX locale.

- Config dialog page for exclude rules

- Config dialog page for treemap colors: Map MIME types to colors

- Not sure if all those crazy treemap parameters need to be user
  configurable. They are pretty much expert-only, and most of them look pretty
  scary to normal people. I might leave them in the config file, but most
  likely anybody who really wants to change any of them will need to use his
  favourite editor.


## Cleanups

### Move to Trash

- Implement XDG trash spec, however old and abandoned it is



## General

### Translations

Add translation stuff. Setting up the Qt translator classes is easy enough. The
translation file(s) will probably also end up in the resource file to avoid
being dependent on external files (they can always go missing). But then, since
it's not KDE any more, the number of supported languages will be really small:
I can supply a German translation, and maybe I can talk my Czech, Spanish and
maybe Russian colleagues into contributing their languages, but there won't be
many more than that.

### Online Help

I know that most users can't be bothered to read that stuff anyway, but I spent
a lot of work for the old help files; porting them should be easy enough. But
that will definitely be only for English and maybe (!)  for German. There is no
way to maintain that much text in any more languages.



## DirTreeView

- closeAllExcept()

- Column widths
  - Add context menu to header to unlock widths?


## Some Time in the Future

### Reports?

- Format selected items in useful format and export to clipboard / to file?

- Text, CSV, YaML?





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
