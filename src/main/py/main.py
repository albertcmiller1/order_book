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
        all_orders + (matches*2): {(ob.num_orders(book.OrderType.bid) + ob.num_orders(book.OrderType.ask) + total_matches*2):,}
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
    r3 = int(round(random.random(), 2) * 10)
    if r1==0:
        ob.add_order(book.OrderType.bid, "albert", 1, book.Money(highest_bid.getDollars(), highest_bid.getCents() + r2)), 
    elif r1==1: 
        ob.add_order(book.OrderType.bid, "albert", 1, book.Money(highest_bid.getDollars(), highest_bid.getCents() - r2)), 
    elif r1==2: 
        ob.add_order(book.OrderType.ask, "albert", 1, book.Money(lowest_ask.getDollars(), lowest_ask.getCents() + r2)), 
    else: 
        ob.add_order(book.OrderType.ask, "albert", 1, book.Money(lowest_ask.getDollars(), lowest_ask.getCents() - r2)), 
    

if __name__ == "__main__":
    ob = book.OrderBook()
    cnt = total_matches = 0
    start_time = time.time()
    iterations = 1000000
    while True: 
        cnt+=1
        add_random_order(ob)
        total_matches += len(ob.process())
        if cnt%iterations==0:
            end_time = time.time()
            print_book(ob, cnt, total_matches, end_time - start_time)
            start_time = time.time()