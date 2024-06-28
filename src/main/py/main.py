import book

if __name__ == "__main__":
    ob = book.OrderBook()
    ob.add_order("buy", "albert_miller", 1, 33.55)
    ob.get_limits_dll()
    print(book.OrderBook().generate_order_id())


'''
we should be able to add an order to the book 
get the status of an order 
cancel an order from the book 
get the current status/spread of the book 
number of orders / limits 




drwxr-xr-x   9 almiller  staff     288 Jun 28 16:23 book-0.0.1.dist-info
-rwxr-xr-x   1 almiller  staff  203416 Jun 28 16:23 book.cpython-312-darwin.so
'''