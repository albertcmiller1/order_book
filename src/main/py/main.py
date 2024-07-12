import book, random, time

def format_money(m):
    dollars = m.getDollars()
    cents = m.getCents()
    if cents < 10 and cents >= 0:
        return float(f"{dollars}.0{cents}")
    return float(f"{dollars}.{cents}") if m.getDollars()>0 else -1

def ipo(ob):
    ipo_bid_order_ids = []
    for _ in range(100):
        ipo_bid_order_ids.append(
            ob.add_order(book.OrderType.bid, "tsla_ipo", 1, book.Money(100, 0))
        )
    return ipo_bid_order_ids

def print_book(ob, cnt, total_matchs, time_taken):
    print(f"""
        <------------- {cnt:,} ---------------->
        num_bid_orders: {ob.num_orders(book.OrderType.bid):,}
        num_ask_orders: {ob.num_orders(book.OrderType.ask):,}
        
        num_bid_limits: {ob.num_limits(book.OrderType.bid):,}
        num_ask_limits: {ob.num_limits(book.OrderType.ask):,}

        highest_bid: {format_money(ob.prominent_limit(book.OrderType.bid))}
        lowest_ask: {format_money(ob.prominent_limit(book.OrderType.ask))}

        lowest_ask_limits: {[format_money(m) for m in ob.get_limits(book.OrderType.ask, 10)]}
        highest_bid_limits: {[format_money(m) for m in ob.get_limits(book.OrderType.bid, 10)]}

        total_number_of_matches: {total_matchs:,}
        all_orders + matches: {ob.num_orders(book.OrderType.bid) + ob.num_orders(book.OrderType.ask) + total_matches}
        <------------- {time_taken:,} ---------------->
    """)

def add_random_order(ob):
    highest_bid = ob.prominent_limit(book.OrderType.bid)
    lowest_ask = ob.prominent_limit(book.OrderType.ask)

    bid_and_ask_limits_set = min(format_money(highest_bid), format_money(lowest_ask))>0
    if (bid_and_ask_limits_set) and (format_money(highest_bid) > format_money(lowest_ask)): 
        raise Exception("???")

    if highest_bid.getDollars() < 0: 
        highest_bid = book.Money(100, 0)
    if lowest_ask.getDollars() < 0:
        lowest_ask = book.Money(100, 0)
    
    r1 = round(random.random()*10 % 3)
    r2 = int(round(random.random(), 2) * 10)
    if r1==0:
        return (
            book.OrderType.bid, 
            ob.add_order(book.OrderType.bid, "albert", 1, book.Money(highest_bid.getDollars(), highest_bid.getCents() + r2)), 
            ob.process()
        )
    elif r1==1: 
        return (
            book.OrderType.bid, 
            ob.add_order(book.OrderType.bid, "albert", 1, book.Money(highest_bid.getDollars(), highest_bid.getCents() - r2)), 
            ob.process()
        )
    elif r1==2: 
        return (
            book.OrderType.ask, 
            ob.add_order(book.OrderType.ask, "albert", 1, book.Money(lowest_ask.getDollars(), lowest_ask.getCents() + r2)), 
            ob.process()
        )
    else: 
        return (
            book.OrderType.ask, 
            ob.add_order(book.OrderType.ask, "albert", 1, book.Money(lowest_ask.getDollars(), lowest_ask.getCents() - r2)), 
            ob.process()
        )
    

all_matches = set()
all_bid_orders = set()
all_ask_orders = set()

if __name__ == "__main__":
    ob = book.OrderBook()
    cnt=0
    start_time = time.time()
    iterations = 1000
    total_matches = 0
    while True: 
        cnt+=1
        order_type, order_id, matches = add_random_order(ob)
        if order_type==book.OrderType.bid: 
            all_bid_orders.add(order_id)
        else: 
            all_ask_orders.add(order_id)

        for match in matches: 
            all_matches.add(match.match_id)
            if match.bid_order_id in all_bid_orders:
                all_bid_orders.remove(match.bid_order_id)
            if match.ask_order_id in all_ask_orders:
                all_ask_orders.remove(match.ask_order_id)

        total_matches += len(matches)
        if cnt%iterations==0:
            end_time = time.time()
            print_book(ob, cnt, total_matches, end_time - start_time)
            start_time = time.time()
            break 

    print(f"""
    all_matches: {len(all_matches)}
    all_bid_orders: {len(all_bid_orders)}
    all_ask_orders: {len(all_ask_orders)}
    sum: {len(all_matches) + len(all_bid_orders) + len(all_ask_orders)}
    """)


    '''
    Issue: the sum of all bid and ask orders in the order book + all old matches should equal the number orders added (assuming no branching)?
    Causes: 
        > same order id getting generated? 
        > doubles too close for compare? 
    '''