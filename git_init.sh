#make sure that you have built a remote repositorie already
git init
git add -A
git commit -m "网络转发存档"

git remote add origin git@github.com:sundajiang/netlayer.git
git push -u origin master
