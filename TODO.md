# TASKS PENDING

Here I will list the pending tasks and the bugs and issues that should be addressed.

## High Priority

* Test it with real chess data, in order to find possible errors. A function that reads chess moves in ascii and playis it in the board.
* Comment the .hh files, maybe also in de cpp files
* The ENGINE
* Include const at those parameters passed by reference, also const in those functions that do not change anything from the object.

## Low Priority

* PieceMoveToAlgebraic could write also # and + if is checkmate or check.
* Improve the GUI, adding sound (easy) and visualize the movement (hard, i think)
* In board.hh, boardStateLog increases its size in each turn. It might cause huge delays when duplicating a Board instance.
* Undo move.

## Bugs and issues

* Update targeted bitmap takes too long or maybe it is calculate legal moves. Those before moving a piece.