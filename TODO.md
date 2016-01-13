# QDirStat To Do List

This is roughly ordered by priority.


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


## Ready-made Packages for SUSE and Ubuntu

- Write a .spec file for SUSE RPMs

- Write a .control file for Ubuntu .deb packages

- Use the openSUSE build service to create RPMs (and .deb packages?)


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


### MacOS X Support

- Find out how well "make install" works, in particular for the other subdirs
  (doc/, scripts/) fix any problems with that

- Find out how to package a MacOS app

- Find a volunteer for a MacOS X platform maintainer


### Windows Support

- Create DirReadJob subclass that strictly uses Qt classes (QFileInfo) and no
  POSIX system calls

- 2015-12-25: Suy (Alejandro Exojo) suggested to use QStorageInfo (available
  since Qt 5.4) to find out platform-independently on what device a directory
  is. This doesn't sound too bad. This is something worthwhile to investigate
  in more detail.

