# QDirStat / scripts

## qdirstat-cache-writer

This is a Perl script that can be used by system administrators to scan
directory trees in cron jobs over night and view the result with QDirStat
whenever it is convenient - without creating I/O load on the machine you are
scanning. You can also use that script to scan directories on a server and view
the result on any machine that has an X11 desktop running. The server doesn't
need any more infrastructure than a normal Perl installation (i.e., no X11 /
KDE / Gnome required).

Using those cache files considerably speeds up QDirStat's reading process. To
give some rough numbers, on my laptop it takes QDirStat about 3 minutes to scan
/usr. Reading the same information from a cache file takes 3-5 seconds.

On the downside, the disk content may have changed in the meantime. A cache
file is outdated by definition. But it may still give you some rough ideas. And
there are large directory trees that hardly ever change.

Or you may be a system administrator with a NFS server that houses home
directories, and every now and then you have to check exactly who of your users
again managed to fill up that filesystem to 95%. One thing you cannot do (or
your users will hate you for it) is start QDirStat during working hours to scan
all those home directories. So do that with the qdirstat-cache-writer Perl
script in a cron job running in the middle of the night and view the result
with QDirStat during your normal office hours.

 
For large directories (archives etc.) that don't change that much, you can also
generate a QDirStat cache file (either with the Perl script or with QDirStat
itself) and save it to that corresponding directory.

If QDirStat finds a file .qdirstat.cache.gz in a directory, it checks if the
toplevel directory in that cache file is the same as the current directory, and
if it is, it uses the cache file for that directory rather than reading all
subdirectories from disk. If you or the users of that machine use QDirStat often,
this might take a lot of I/O load from the server.


There is currently no indication that cached values are displayed. This will
have to change.

When you click the "Reload" button, the directory tree is really scanned (just
in case you might have thought the cache file is read again). This is
intentional.

Tip: If you generate cache files with "qdirstat-cache-writer -l", they will
become somewhat (~20%) larger, but you can also use them as a replacement for
"locate". Simply use "zgrep" in such a file and ignore the size, mtime
etc. fields.


See also [QDirStat-for-Servers.md](https://github.com/shundhammer/qdirstat/blob/master/doc/QDirStat-for-Servers.md)
