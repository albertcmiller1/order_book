# Fast Limit Order Book

## Overview 
### book
This repo simulates an order book using a binary tree, doubly linked list, and two hash maps. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/) and
[here](https://quant.stackexchange.com/questions/3783/what-is-an-efficient-data-structure-to-model-order-book). Each node of the binary tree holds a unique limit price, and a pointer to a doubly linked list (sorted by time) containing bids and asks. 
### fake trading bots
blah blah blah blah blah blah blah blah blah blah blah blah 

### connecting to the book
blah blah blah blah blah blah blah blah blah blah blah blah 

### infrastructure 
blah blah blah blah blah blah blah blah blah blah blah blah 

## Todo 
* implement trees + DLL
* implement spread check + erase after match
* logic to retract from queue 
* who get's priority in queue? time dependent? what about partial ability to complete transactions due to volume differences 
* boto to post transaction (updated price) to dynamodb
* way to visualize
* how do we allow other processes to connect? websockets? api?
* create simple multi threaded app to connect to process and simulate buys/sells
* host on docker contain / aws ecs
