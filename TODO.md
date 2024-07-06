# TASKS PENDING

Here I will list the pending tasks and the bugs and issues that should be addressed.

## High Priority

* ENGINE: change the king value squares depending.
* ENGINE: different weights depending if we are in the early, mid and endgame.
* Handle in a optimal way the threefold repetition.
* Comment the .hh files, maybe also in de cpp files.

## Low Priority

* PieceMoveToAlgebraic could write also # and + if is checkmate or check.
* The undo function acts wierd when an engine in playing. Could use interruptions in order to handle those events.
* Improve the GUI, adding sound (easy) and visualize the movement (hard, i think).
* In board.hh, boardStateLog increases its size in each turn. It might cause huge delays when duplicating a Board instance.

## Bugs and issues

## Possible optimitzations:

* When calculating the targeted squares, use more bitwise operations.