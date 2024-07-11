import book

if __name__ == "__main__":
    ob = book.OrderBook()
    bid_order_id = ob.add_order(book.OrderType.bid, "albert", 6, 101)
    ob.add_order(book.OrderType.ask, "albert", 6, 100)
    
    num_bid_orders = ob.num_orders(book.OrderType.bid)
    num_ask_orders = ob.num_orders(book.OrderType.ask)
    
    num_bid_limits = ob.num_limits(book.OrderType.bid)
    num_ask_limits = ob.num_limits(book.OrderType.ask)

    lowest_ask  = ob.prominent_limit(book.OrderType.bid)
    highest_bid = ob.prominent_limit(book.OrderType.ask)

    ask_limits = ob.get_limits(book.OrderType.bid, 10)    
    bid_limits = ob.get_limits(book.OrderType.ask, 10)    

    order_is_in_queue = ob.order_in_queue(book.OrderType.bid, bid_order_id)

    matches = ob.process()
    print(matches)


'''
cancel an order from the book 
'''