# QDirStat
Qt-based directory statistics: KDirStat without any KDE -- from the original KDirStat author.

(c) 2015 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

License: GPL V2

## Overview

This is a Qt-only port of the old Qt3/KDE3-based KDirStat, now based on the
latest Qt 5.


## Motivation / Rant

Now that KDE has been losing direction more and more, it's time to come up with
a desktop agnostic version of KDirStat - one that can be used without the
hassle of an ever-changing desktop environment like KDE. KDE has become a
moving target for developers. Much effort has gone into creating eminently
essential things like a yellow note desktop widget (a.k.a. _Plasmoid_) that can
be rotated by 7.834°, but basic usability has gone downhill during that
process.

With the latest Plasma 5 (as of late 2015), there are some KDE apps that use
KDE 5.x, and some that still use KDE 4.x, with the overall effect that settings
have been duplicated (systemsettings5 vs. systemsettings, anyone?), thus the
look and feel between those apps is different, and settings became unaccessible
for the end user (again: systemsettings5 vs. systemsettings).

Worse, users get treated to a mail client that uses a MySQL database back-end
called _Akonadi_. Not only is that the most fragile construction I've ever
seen so far, it also leaves half a Gigabyte worth of log files in my home
directory (!!) due to poor default configuration (both on Kubuntu and on SUSE,
probably also on all other distros). Argh. I found that the most important tool
for that stuff was the _akonadiconsole_ which I needed numerous times to fix a
borked mail environment. But I am a system developer. How on earth is a normal
user supposed to fix that kind of problem? A problem that wouldn't exist in the
first place if the developers hadn't choosen the most complicated and most
fragile combination of tools?

What was once a great desktop environment has become a playground for
developers who don't seem to talk to each other - and, worse, who don't give a
shit about their users anymore.

Time for KDirStat to move away from this insanity. Time to become
independent. KDirStat had never used that much of the KDE infrastructure to
begin with; what little it used (icons, for example) can be replaced by very
little own infrastructure. The KDE libs are just not worth the hassle anymore.


## Current Development Status

- 2015-11-28  Project is being set up. Nothing usable so far.


## Screen Shots

None yet. Stay tuned.


## Building

### Build Environment

Make sure you have a working Qt 5 build environment installed. This includes:

- C++ compiler (gcc recommended)
- Qt 5 runtime environment
- Qt 5 header files


#### Ubuntu

Install the required packages for building:

    sudo apt-get install build-essential qtbase5-dev

Dependent packages will be added automatically.

Recommended packages for developers:

    sudo apt-get install qttools5-dev-tools qtbase5-doc qtbase5-doc-html qtbase5-examples

See also

http://askubuntu.com/questions/508503/whats-the-development-package-for-qt5-in-14-04


#### SUSE

Install the required packages for building:

    sudo zypper install -t pattern devel_C_C++
    sudo zypper install libqt5-qtbase-common-devel libqt5-qttools-devel

**FIXME:** Untested. This needs to be tested and verified.


### Compiling

Open a shell window, go to the QDirStat source directory, then enter these
commands:

    qmake
    make


### Installing

    sudo make install

or

    su -c make install


## Contributing

Once the project setup is complete, contributions will be welcome.


### Contribution Guide Lines

Use the same style as the existing code. Look at the old KDE 3 kdirstat for
examples.

Indentation is 4 blanks. One tab is 8 blanks. Do not indent with single tabs
for each indentation level and thus fuck up everybody else's editor. Letting
the editor automatically replace 8 blanks with a tab is okay, but indentation
level is still 4 blanks.

Brace style is braces **always** on a new line. Use blanks after opening and
before closing parentheses:

    if ( someCondition )
    {
        doSomething( arg1, arg2 )
    }

**No** K&R style indentation:

    if (someCondition) {         // WRONG!!!
        doSomething(arg1, arg2)  // WRONG!!!
    }

Use blank lines liberally. No Rubocop-Style code.

Use CamelCase for classes and camelCase for variables. Do not use
snake_case. Prefix member variables with an underscore: _someValue. Prefix its
setter with Set...(), use only the name (someValue()) for the getter, **Not**
getSomeValue():

    private:
        SomeType _someValue;    // member variable

    public:
        const & SomeType someValue() const;              // getter
        void setSomeValue( const SomeType & newValue );  // setter

Use a const reference for the setter parameter and the getter return type for
nontrival data types (everything beyond a pointer or an int etc.), and the type
directly otherwise. Use your common sense.

Use Qt types whereever possible. Do not introduce types that are also available
in a Qt version. In particular, do not use STL or Boost types unless there is
no other reasonable way.

For Qt dialogs, use Qt Designer forms whenever possible. I know that many Qt
developers don't like Qt Designer (typically based on bad experiences years
back), but it's really worthwhile to use it. This avoids having tons of dumb
boilerplate code just for creating widget trees.


### Documenting Classes and their Methods

Document classes and their methods in their header file. Document them
immediately. If you don't do it right away, it will never happen. Don't wait
for somebody else to do that for you - nobody will be willing to do the
inconvenient part of the job for you.

Undocumented stuff will mercilessly be rejected.

In my 30+ years of programming I came to the conclusion that it helps immensely
to write down a function name in the header file and **immediately** write its
documentation:

- What does it do? (Do not just repeat the function name! **Explain** what it does!)
- What are the parameters?
- What does it return?
- In particular for functions returning bool: Which case returns 'true', which
  case returns 'false'?
- What happens in fringe cases? (returning NULL pointer?)
- If the function creates any objects: Who owns them?
- Does the function transfer ownership of any objects it gets pointers to as
  parameters to itself or wherever?

If you do that right, you might as well leave the implementation to somebody else.


**Do not** insert any doc template without content before each function.
This is worse than no documentation at all: Everybody has to wade through tons
of useless empty forms that don't contain any content whatsoever.

Be careful when copying and pasting documentation from some other place.
Misleading documentation is worse than no documentation at all.


### Getting Help for Contributers

- Use the Qt reference documentation.
- Install and study carefully the available Qt examples for reference.
- Use Stack Overflow.
- Use Google.
- Make sure you can build a basic Qt program.
- Make sure you have the compiler and other developer tools set up correctly.
- Make sure you have the relevant -devel (SUSE) or -dev (Ubuntu/Debian)
  packages installed.

If you did all of the above (and only then!) and still couldn't figure out
what's wrong:

- Use IRC (#qdirstat on irc.freenode.net; fallback: contact HuHa in #yast on
  freenode IRC)
- Write a mail

It is a matter of professional courtesy to first use what is freely available
on the web before you consume somebody else's time and expertise. It is utterly
rude to let somebody else point you to the exact part of the Qt documentation
you couldn't be bothered to read.

Does this sound unfriendly? Well, maybe - but: Been there, done that, got the
fucking T-shirt -- countless times. This is not how I want to spend my spare
time.

There are some people who keep arguing that "it takes hours for me to read
through all the documentation, whereas you as an expert can give me the right
answer within a minute".

Well, yes, it might only take a minute to write the answer in IRC, but then
that newbie won't learn anything except that it pays off to disturb people
rather than learn for himself. And reading the documentation is always a good
investment of time; it is never wasted. In the process of searching for an
answer you will come across many things you hadn't known - and that might just
be useful for your next question.

Plus, it will take the expert that one or two minutes to write the answer to
IRC - and then **15-20 minutes to recover from the interrupt**, to restore the
fragile buildings in his mind he needs to solve the problem he was originally
working on.

Please keep in mind that every such interrupt will cost 20-30 minutes of time
on average, and a working day only has 16-24 such time slots; and those experts
tend to be in high demand on many different communication channels (IRC,
personal mail, mailing lists, bugzilla, phone, people walking into the office).

So, should you join the project as a newbie?

If you have patience, self-discipline and are willing to learn (which includes
reading the Qt documentation first), and, most importantly, if you are willing
to stay with the project and not let some unfortunate guy clean up half-ready
stuff, yes.

Just keep in mind that others are doing this because (a) it's fun for them
and/or (b) because they want that piece of software to be successful. Educating
newbies and cleaning up after them is very low on most developers' list of fun
things to do. They still do it, but it's painful for them. Please help
minimizing that pain.
