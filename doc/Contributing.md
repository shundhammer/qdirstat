# Contributing to QDirStat

Contributions are welcome. Please follow the rules and guide lines in this
document:


## Guide Lines

Use the same style as the existing code.

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

Use blank lines liberally. No Rubocop-style code.

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
nontrivial data types (everything beyond a pointer or an int etc.), and the type
directly otherwise. Use your common sense.

Use Qt types wherever possible. Do not introduce types that are also available
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
- Does the function transfer ownership of any objects it gets pointers to (as
  parameters) to itself or wherever?

If you do that right, you might as well leave the implementation to somebody else.


**Do not** insert any doc template without content before each function.
This is worse than no documentation at all: Everybody has to wade through tons
of useless empty forms that don't contain any content whatsoever.

Been there. Done that. Hated it.

Be careful when copying and pasting documentation from some other place.
Misleading documentation is worse than no documentation at all.


### Legalese in the Sources

**Do not** copy 30+ lines of legalese bullshit into any source file. One line
in the header like

    License: GPL V2 - see file LICENSE

is plenty. Seriously, what are those people thinking who put all that legalese
into source files? Sure, they listened to spineless corporate lawyers who just
want to make sure. But those lawyers are not going to have that crap smack up
their faces every time they open a file for editing. We developers do.

You lawyers out there, can you hear me? **This crap is in the way! Nobody wants
to read that!** It's in the way of people trying to do their jobs! We could
construct a harassment case from this!


### The GitHub Workflow

See separate document [GitHub-Workflow.md](https://github.com/shundhammer/qdirstat/blob/master/doc/GitHub-Workflow.md)


### Getting Help for Contributors

- Use the Qt reference documentation.
- Install and study carefully the available Qt examples for reference.
- Read the (extensive!) documentation in the QDirStat header files.
- Use Stack Overflow.
- Use Google.
- Make sure you can build a basic Qt program.
- Make sure you have the compiler and other developer tools set up correctly.
- Make sure you have the relevant -devel (SUSE) or -dev (Ubuntu/Debian)
  packages installed.

If you did all of the above (and only then!) and still couldn't figure out
what's wrong:

- Use IRC (HuHa in #yast on libera.chat IRC)
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


## Further Reading

- https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
