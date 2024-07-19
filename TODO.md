# TASKS PENDING

Here I will list the pending tasks and the bugs and issues that should be addressed.

## High Priority

* Made the myApp class not have the board.hh include. All events handled by the main loop. The way it's handled now might bring problems.
* Comment the .hh files, maybe also in de cpp files.

## Low Priority

* Better files handling, for the engine, for e.g., transposition table in another class etc
* PieceMoveToAlgebraic could write also # and + if is checkmate or check.
* The undo function acts wierd when an engine in playing. Could use interruptions in order to handle those events.
* Improve the GUI, maybe with qt, adding buttons

## Bugs and issues

## Possible optimitzations:

* When calculating the targeted squares, use more bitwise operations.
* When manageCheck() and eliminatePinnedMoves() don't creat class Board copies.