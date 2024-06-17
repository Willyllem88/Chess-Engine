# TASKS PENDING

Here I will list the pending tasks. Just to not forget those ideas that might benefit the project.

## TO DO

* Give some information when executing without arguments
* Handle all arguments
* A function that translates chess algebraic notation to ijMove
* The ENGINE
* Comment the .hh files, maybe also in de cpp files
* Include const at those parameters passed by reference, also const in those functions that do not change anything from the object.

## FIX SHORT TERM

* A move can be inputed with algebraic chess notation through the console. This features is being done in the MyApp class, at the bottom of handle event.
* It will render only when a move its done.

# FIX LONG TERM

* Test it with real chess data, in order to find possible errors. A function that reads chess moves in ascii and playis it in the board.
* Improve the GUI, adding sound (easy) and visualize the movement (hard, i think)
* In board.hh, boardStateLog increases its size in each turn. It might cause huge delays when duplicating a Board instance.