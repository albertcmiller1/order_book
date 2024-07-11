import book, random, time

def ipo(ob):
    ipo_bid_order_ids = []
    for _ in range(100):
        ipo_bid_order_ids.append(
            ob.add_order(book.OrderType.bid, "tsla_ipo", 1, 100.00)
        )
    return ipo_bid_order_ids

def print_book(ob, matches, cnt, time_taken):
    print(f"""
        <------------- {cnt:,} ---------------->
        num_bid_orders: {ob.num_orders(book.OrderType.bid):,}
        num_ask_orders: {ob.num_orders(book.OrderType.ask):,}
        
        num_bid_limits: {ob.num_limits(book.OrderType.bid):,}
        num_ask_limits: {ob.num_limits(book.OrderType.ask):,}

        highest_bid: {ob.prominent_limit(book.OrderType.bid):,}
        lowest_ask: {ob.prominent_limit(book.OrderType.ask):,}

        lowest_ask_limits: {ob.get_limits(book.OrderType.ask, 10)}
        highest_bid_limits: {ob.get_limits(book.OrderType.bid, 10)}

        num_matches: {len(matches):,}
        <------------- {time_taken:,} ---------------->
    """)

def add_random_order(ob):
    highest_bid = ob.prominent_limit(book.OrderType.bid)
    lowest_ask = ob.prominent_limit(book.OrderType.ask)

    is_not_set = min(highest_bid, lowest_ask)<0
    if (not is_not_set) and (highest_bid > lowest_ask): 
        raise Exception("???")

    if highest_bid < 0: 
        highest_bid = 100
    if lowest_ask < 0:
        lowest_ask = 100
    
    r1 = round(random.random()*10 % 3)
    r2 = round(random.random(), 2)
    if r1==0:
        ob.add_order(book.OrderType.bid, "albert", 1, highest_bid + r2)
    elif r1==1: 
        ob.add_order(book.OrderType.bid, "albert", 1, highest_bid - r2)
    elif r1==2: 
        ob.add_order(book.OrderType.ask, "albert", 1, lowest_ask + r2)
    else: 
        ob.add_order(book.OrderType.ask, "albert", 1, lowest_ask - r2)
    
    matches = ob.process()
    return matches


if __name__ == "__main__":
    ob = book.OrderBook()
    ipo(ob)
    cnt=0
    start_time = time.time()
    iterations = 1000000
    while True: 
        cnt+=1
        matches = add_random_order(ob)
        if cnt%iterations==0:
            end_time = time.time()
            print_book(ob, matches, cnt, end_time - start_time)
            start_time = time.time()
