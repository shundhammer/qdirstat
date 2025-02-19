## QDirStat metainfo/ Directory

This is for FlatPak which is maintained **outside** the QDirStat project.

This is really only about one single file that gets installed for a FlatPak, so
this is not installed or handled or anything outside of building the FlatPak.

It is installed to /usr/share/metainfo which is as simple as a shell one-liner:

```
sudo cp io.github.shundhammer.qdirstat.metainfo.xml /usr/share/metainfo
```

There is no reason to install it for an RPM or APK build; it only gets in the
way.
