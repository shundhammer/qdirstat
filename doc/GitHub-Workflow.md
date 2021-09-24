# GitHub Workflow for Open Source Projects

(c) 2017 Stefan Hundhammer

License: GNU Free Documentation License


This is the workflow used for Open Source projects like QDirStat and YaST.

Some of this is mandated by the tools used (git, GitHub), some of it is just
best practices.


## Overview

Even though GitHub hosts the original source code repositories for those
projects, you don't work directly in them. Rather, you create your own forked
repository (your _fork_), work in that one, and when you finished something you
want to contribute to the original repository (called _upstream_), you create a
pull request.

That pull request is reviewed by the owner of the original project or by or
senior team members. They might ask you for changes if anything in your pull
request does not meet the project's quality criteria or violates the coding
style. In that case, you add more commits to the pull request, and it is
reviewed again etc. until everybody is satisfied and it is either merged (which
is the normal case) or it is finally rejected (which is a very rare thing).

When your changes are merged, you pull or rebase your fork against _upstream_
again so it is up to date, and then you can freely work on new things.

One holy rule is that **upstream master always has to work**, even between
official releases. This is much easier to achieve when everybody works in their
own fork, preferably in their own branch of their own fork.


## Initial Setup

- If you don't have one yet, create a user account at
  [GitHub](https://www.github.com).

- Make sure to upload your ssh key to your GitHub account.
  [More info...](https://help.github.com/articles/connecting-to-github-with-ssh/)

- Log in at GitHub.

- Fork the original (upstream) repository to your GitHub account.
  You now have your own repo with that name.

  **But it does not automatically sync itself with the upstream repo** as
  others commit changes there after you forked; you have to do that manually
  (see below).

- In a shell on your working machine (preferably Linux), clone your forked repo
  to that machine:

      cd ~/src
      git clone -o mine git@github.com:kilroy/qdirstat.git

  Where _kilroy_ is your GitHub user name which is part of the URL of that
  fork. **Make sure to use the "git@github" URL**, not the "https://" URL: The
  https URL is only useful for pulling (i.e. for read-only access), not for
  pushing (i.e. for read-write access). Since you also want to commit changes
  to that repo, you need read-write access.

  _mine_ is the name of that _remote_. The default would be _origin_, but that
  might lead to confusion because we'll add _upstream_ in a moment, so there
  will be two remotes. If you have the same distinct name for all your forks,
  your life will be considerably easier. You might also call it the same as
  your user name (_kilroy_) here. Just make sure you use the same one for all
  your repos.

  You now have an entry like this in your `.git/config` file in that newly
  cloned repo:

      [remote "mine"]
              url = git@github.com:kilroy/qdirstat.git
              fetch = +refs/heads/*:refs/remotes/kilroy/*

- Add the original repo (_upstream_) as another _remote_ so you can pull /
  fetch from there to keep your fork up to date:

      git remote add upstream git@github.com:shundhammer/qdirstat.git

  You now have two _remote_ entries in your `.git/config`:

      [remote "mine"]
              url = git@github.com:kilroy/qdirstat.git
              fetch = +refs/heads/*:refs/remotes/kilroy/*

      [remote "upstream"]
              url = git@github.com:shundhammer/qdirstat.git
              fetch = +refs/heads/*:refs/remotes/huha/*

  You can also check this with this command:

      git remote -v

      mine      git@github.com:kilroy/qdirstat.git (fetch)
      mine      git@github.com:kilroy/qdirstat.git (push)
      upstream  git@github.com:shundhammer/qdirstat.git (fetch)
      upstream  git@github.com:shundhammer/qdirstat.git (push)

  Notice there is no _origin_ as would be the default if we hadn't used
  `-o mine` during `git clone`. If you forgot that, you can always rename a
  remote later (this affects only your working copy, not the repo on the GitHub
  server):

      git remote rename origin mine

- Make sure your user name and e-mail address are up to date and valid in your
  `$HOME/.gitconfig`:

      [user]
          name = Kilroy Taylor
          email = kilroy@mydomain.com

  You can also use separate user names and e-mail addresses for different
  projects; simply edit .git/config in that project and add a `[user]` section
  there.

  That name and that e-mail address will be recorded for each commit you make,
  so this is where your karma points go and how you will be known to the
  community. So choose that name wisely. Real names are preferred, albeit not
  enforced.



## Common Tasks

### Working in Your Fork

Your fork is yours. You can do there whatever you like. But if you want to
contribute to the upstream project, you should follow some simple rules:

- Keep your fork in sync with upstream as good as possible (see next section
  about rebasing).

- Work in a feature branch for everything you do. This makes your life much
  easier when a pull request takes some time to get accepted: You can quickly
  switch between the pull request and add some more changes there to satisfy
  the reviewers and the next feature you might already be working on.

- Prefix your branches with your user name so you can easily tell them apart
  form any upstream branches.

Example:

You plan to work on a _transmogrify_ feature. So you start from _master_,
create a branch for that and check it out:

    git checkout master
    git branch kilroy-transmogrify
    git checkout kilroy-transmogrify

Now work in that branch and commit your changes there. Don't forget to push it
to your GitHub fork every once in a while:

    git push mine kilroy-transmogrify

At some point, prepare a pull request to get your changes upstream. But before
you do that, you should rebase your branch so it is in sync with upstream
(except for your changes, of course).


### Rebasing (Updating Your Fork)

As mentioned before, when you fork a repo at GitHub, this will not
automatically update itself. As new commits are added to the upstream repo,
your fork will increasingly get out of date, so you have to update it on a
regular basis.

There are two methods: `pull` and `fetch` / `rebase`. As long as you don't do
any changes in your fork, there is no noticeable difference; but when you work
in your fork, i.e. when you commit changes there, `fetch` / `rebase` is highly
recommended to keep the "railway yard" of parallel branches with merge and fork
points in the `gitk` display to a minimum.

First, get the data from the remote server (GitHub):

    git fetch --all

**Make sure to use `--all`, not `-a`** which is something different (yes, this
is a common, stupid, unnecessary pitfall of that git command).

If that command remains silent, there was no change, so everything was still up
to date. If it reports something like

    remote: Counting objects: 21, done.
    remote: Compressing objects: 100% (21/21), done.
    ...

it did fetch some changes. Notice that the changes are only in the `.git/`
subdirectory so far; they are not applied to your source tree yet.

Now **make sure you don't have any pending changes** anymore. Check with

    git status

If it reports any pending changes, you can choose to commit them or to _stash_
them, i.e. put them into temporary storage:

    git stash

you can later retrieve them with

    git stash pop

Now rebase. Typically, you want to do that based on the _master_ branch of
_upstream_:

    git rebase upstream/master

This basically checks where you branched off your working copy, then
temporarily takes away your commits from that point on, then applies the
commits from upstream that have accumulated in the meantime. As a last step, it
tries to apply your commits on top of all that.

Since git commits are basically little more than patches (diffs) on top of
patches, this may or may not work flawlessly. If you are lucky, your commits
still apply cleanly, and you are set: You successfully rebased your repo.

If any of your commits does not apply cleanly, you will have to resolve merge
conflicts and afterwards call

    git rebase --continue

to get to the next commit.

When all is done, you can push the result to your fork. Since the rebase caused
the parent SHAs of your commits to change, you will need to force-push; this is
normal and expected.

    git push -f mine master

or

    git push -f mine branch-name

if you were working in a branch.


### Preparing a Pull Request for Upstream

When you have some changes you would like to get upstream (to contribute to the
upstream project), you create a _Pull Request_.

To do that, make sure those changes are in a separate branch. If you worked on
a separate feature branch like recommended earlier in this document, you can
simply use that one as the branch for the pull request.

But remember to stop working on new features in that branch. As soon as you use
a branch for a pull request, you only commit changes there that were requested
by the reviewers.

Before creating a pull request, rebase your branch against upstream/master once
more and make sure to push your changes (even the latest ones) to your fork:

    git fetch --all
    git rebase upstream/master
    git push mine kilroy-transmogrify

After that, go to your fork in the GitHub web UI, select your branch and click
"New Pull Request". Fill the form with a meaningful description of your changes
and send it off. Now you will have to wait for feedback from the upstream
project owner.

In the meantime, you will probably want to continue working on more
things. Remember to leave the pull request branch alone during that time;
create a new one for your next changes. It is perfectly okay to base that new
branch on the last one that has now become a pull request:

    git branch kilroy-hyperforble
    git checkout kilroy-hyperforble
    ...(work in that branch...)

When you get feedback about your pull request from your reviewers, you might
have to add some more changes to the pull request branch. So switch to that
branch (stash or commit any pending changes to your new working branch during
that time):

    git checkout kilroy-transmogrify
    ...(add requested fixes)...
    git commit -am "Added code review changes"
    git push mine kilroy-transmogrify

Remember that despite the fact that your branch has become a pull request, it
is still hosted in your fork (_mine_) rather than upstream, so you still have
to push to your fork, not to upstream (which you probably can't anyway because
of insufficient permissions).

After that, switch back to your working branch and continue working there:

    git checkout kilroy-hyperforble
    git stash pop      # if you stashed any changes


### Updating Your Fork After Your PR is Merged

Remember that after your pull request has been merged, _upstream/master_ has
changed, so make sure to fetch and rebase:

    git fetch --all
    git checkout master
    git rebase upstream/master

There should be no conflicts (provided you are only working in branches - which
you should).

Don't forget to push the new master to your fork, too:

    git push mine master

No `-f` or `--force` needed here either if you only work in branches.


## Further Reading

https://guides.github.com/activities/contributing-to-open-source/

https://guides.github.com/

https://www.atlassian.com/git/tutorials/merging-vs-rebasing

https://www.udacity.com/course/how-to-use-git-and-github--ud775
