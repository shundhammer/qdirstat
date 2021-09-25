
## Troubleshooting

### Can't Move a Directory to Trash

QDirStat does not copy entire directory trees to the trash directory in your
home directory. It tries its best to copy single files there, but for anything
larger, it strictly sticks to the XDG trash specification. So, if you have a
separate /home partition (which is strongly recommended for a lot of reasons),
you cannot move a directory from /tmp to trash because that would mean to move
a directory across filesystems -- from /tmp/somewhere to your
~/.local/share/Trash .

But there is an easy workaround. It's described in the XDG trash spec, but here
is a simple recipe what you can do:

Create a dedicated trash directory on the toplevel (mount point) of that file
system. If it is mounted at /data, do this:

    cd /data
    sudo mkdir .Trash
    sudo chmod 01777 .Trash

Permissions '01777' means "rwx for all plus sticky bit". The sticky bit for a
directory means that only the owner of a file can remove it. The sticky bit is
required for security reasons and by the XDG trash spec (it's also required by
the spec that applications like QDirStat refuse to use that trash directory it
if it is not set).

Now you can move directory trees from /data/somewhere to the trash with
QDirStat. It will end up in /data/.Trash/1000/files/somewhere (if 1000 is your
numerical user ID which is common in Linux for the first created user). Your
desktop's native trash application (your trash icon on the desktop and the file
manager window you get when you click on it) should show it, and you can empty
the trash from there.

In Xfce, this works out of the box. KDE might need a forced refresh (press F5)
in that window.

For USB sticks, this explicit toplevel .Trash directory is usually not
necessary: If you have write permission on its toplevel directory, QDirStat
will (again in compliance to the XDG trash specification) create a trash
directory .Trash-1000 in its toplevel directory which is the fallback if there
is no .Trash directory there. This would also happen automatically on /data and
/ if you had write permission there -- which is, however, very uncommon.

What the major desktops (KDE, GNOME, Xfce) usually do with their native file
managers is to recursively copy the entire directory tree to your home trash
directory and then remove the original. Not only is this time-consuming and
wasteful (copy stuff before deleting?!), it might also be error-prone if that
directory tree contains symlinks, sockets or even just sparse files; and
permissions and timestamps (mtime, ctime, not to mention atime) might or might
not be the same as before. This might become a problem if you decide to restore
that directory tree from the trash.

I thought about emulating this behaviour, but this basically means to
reimplement large parts of what the _rsync_ command does (calling _rsync_ from
within QDirStat might not be such a good idea - what if it's not available or
anything goes wrong?), and frankly, I don't want to do that - in particular not
for something that typically gets deleted shortly afterwards anyway upon "empty
trash".

So, if you have this problem, please use the .Trash directory workaround
described above.
