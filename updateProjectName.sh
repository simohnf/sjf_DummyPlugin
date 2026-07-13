#!/bin/bash
echo "Please the new name for the project"
  read name

spaces="${name// /\ }"
underscores="${name// /_}"
allCaps=$(tr '[a-z]' '[A-Z]' <<< $underscores)


s1='s/Dummy Plugin/var1/g'
sed -i "" "${s1/var1/$name}" sjf_DummyPlugin/CMakeLists.txt
s2='s/DummyPlugin/var1/g'
sed -i "" "${s2/var1/$underscores}" sjf_DummyPlugin/CMakeLists.txt
s3='s/DUMMY_PLUGIN/var1/g'
sed -i "" "${s3/var1/$allCaps}" sjf_DummyPlugin/CMakeLists.txt

mv sjf_DummyPlugin "$underscores"

d="$PWD/$underscores"


git -C $d checkout --orphan fresh-start
git -C $d add -A
git -C $d submodule update --init --recursive
git -C $d commit -m "Initial commit"

git -C $d branch -M main
git -C $d remote remove origin
git -C $d reflog expire --expire=now --all
git -C $d gc --prune=now --aggressive
