# TO DO

## MainWindow

- write window size / position to config upon exit

- Restore old window size / position

- translation support


## DirTreeView

- refreshSelected()

- continueReading() (mount point, excluded item)

- restore dotEntry (after finalizeLocal() ) when continuing reading

- closeAllExcept()

- Column widths
  - Add context menu to header to unlock widths?


## Config

- readConfig() everywhere


## Config Dialog(s)

- Qt designer forms

- Data columns to display + order (NameCol is alwas there and #1)

- ...

## Cleanups

- port old code

- Make actionCollection a QObject and add the cleanups as QActions to it


### Move to Trash

- Implement XDG trash spec, however old and abandoned it is


### Reports?

- Format selected items in useful format and export to clipboard / to file?

- Text, CSV, YaML?

