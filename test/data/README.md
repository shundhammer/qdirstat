# QDirStat Test Data

## Test Directory

The `test-dir.tar.bz2` tarball contains:

- Files with special characters in their names

- Sparse files

- Files with hard links

- Various symlinks: Valid, broken, pointing to directories without permissions

- A directory without execute permissions

- A directory without read permissions

- A zero length file


## Packing the Tarball

    sudo tar cjvf test-dir.tar.bz2 -S test

**Don't forget** `sudo`, or the directories with restrictive permissions will
not be included into the tarball!

**Don't forget** -S (--sparse), or the sparse files will be blown up to full
size in the tarball!


## Unpacking the Tarball

Plain and simple

    tar xvf test-dir.tar.bz2

no `sudo` necessary and no special option to handle the sparse files correctly.
