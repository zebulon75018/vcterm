# vcterm
Terminal with docking as Visual Code and integrating with helpfull features...

I'm looking for beta testeur ( charles ( dot ) vidal ( at ) gmail ( dot ) com

# Prerequisity

1. Qt 5
2. qtwebengine


You must clone 3 repository, this will be done by compil.sh:

1. QSingleInstance  https://github.com/Skycoder42/QSingleInstance 
2. Qt-Advanced-Docking-System     https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System
3. qtterminalwidget  : https://github.com/jacob3141/qtterminalwidget
4. font awesome : https://github.com/gamecreature/QtAwesome

to make the projet: 
source compil.sh

after lauch cvterm. 

cvterm man ls : show the man of ls in a webpage.

# Feature
1. Embedded terminals inside one docking applicaton
2. Can show help man page. ( qterm man ls )   ![](https://github.com/zebulon75018/vcterm/blob/main/screenshots/man2.gif)
5. Can send the same command to all opened terminals ![](https://github.com/zebulon75018/vcterm/blob/main/screenshots/sendall.gif)
6. Can open terminal and send a command from a list a command
7. Can change font ( size / family ) 
8. Can  change the background by theme.
9. Drag and drop path or file.   ![](https://github.com/zebulon75018/vcterm/blob/main/screenshots/Peek%202022-03-28%2000-46.gif)
10. Choose files/directory by file/dir dialog  
11. Find text in terminal 
12. Docking like visual code.
![](https://github.com/zebulon75018/vcterm/blob/main/screenshots/Peek%202022-03-28%2023-50.gif)

![From githubuser0xFFFF](https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/blob/master/doc/cfg_flag_FloatingContainerForceNativeTitleBar_true.png)

It's a singleInstance Application , so you can interacte with it with the same cmd.
# Help     
1. name : add a new terminal close to the tab name 
2. man : open a web page for the man 
 
 OPTIONS :

1. -tab : create a terminal in the tab 
2. -righ : position in workspace 
3. -left: position in workspace 
4. -top: position in workspace 
5. -bottom: position in workspace 

# Program your interface , your workspace.
```
./dterm &
sleep 1 
./dterm name Term1 &
sleep 1
./dterm name Term2 &
sleep 1
./dterm name Term3 &
sleep 1
./dterm name Term4 &
sleep 1
./dterm -bottom &
sleep 1
./dterm name Term6 &
sleep 1
./dterm name Term7 &
sleep 1
./dterm name Term8 &
sleep 1
./dterm name Term9 &
sleep 1
./dterm -bottom &
sleep 1
./dterm name Term11 &
sleep 1
./dterm name Term12 &
sleep 1
./dterm name Term13 &
sleep 1
```



![](https://github.com/zebulon75018/vcterm/blob/main/screenshots/envsheel.gif)

# TODO

When I close a windows , not hide it , but close it and close the terminal.

# Video

[![First Demo ](https://img.youtube.com/vi/JBAkvO5jjtQ/0.jpg)](https://youtu.be/JBAkvO5jjtQ)


[![Launch multiprocess ! on all terminal, or launch process in multiterminal ](https://img.youtube.com/vi/pg7EwaA3SE0/0.jpg)](https://youtu.be/pg7EwaA3SE0)

[![New feature ](https://img.youtube.com/vi/YRqGp4bD3Sw/0.jpg)](https://youtu.be/YRqGp4bD3Sw)


# Blog en francais:

https://www.blogger.com/blog/post/edit/1673332321871142560/8502050071487911199

# Other interesting projet

https://gnunn1.github.io/tilix-web/

https://github.com/shi-yan/Commandventure ( still some bug , I got a core dump )


