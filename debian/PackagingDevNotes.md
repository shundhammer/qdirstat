# Packaging dev notes

To run the result, in a clean environment:

```bash
$ docker run -itv `realpath build`:/build -p 5900:5900 -it ubuntu:focal /bin/bash
```
and then, inside:
```bash
apt-get update
apt-get install -y x11vnc xvfb
mkdir ~/.vnc
x11vnc -storepasswd 1234 ~/.vnc/passwd
x11vnc -forever -usepw -create -ncache 10 -ncache_cr

# and for qdirstat:
apt-get install -y  `realpath build/qdirstat_1.7.1-22_amd64.deb`
```

Of course, package should be test installed on a "Live" system instead
