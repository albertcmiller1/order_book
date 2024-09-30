import random, time, book

class Stimulate:
    def __init__(self, order_book): 
        self.ob = order_book
        self.total_matches = 0
        self.total_add_time        = 0
        self.total_processing_time = 0
        self.best_processing_time  = float("inf")
        self.worst_processing_time = float("-inf")
        self.best_add_time  = float("inf")
        self.worst_add_time = float("-inf")

    def format_money(self, m):
        dollars = m.getDollars()
        cents   = m.getCents()
        if cents < 10 and cents >= 0:
            return float(f"{dollars}.0{cents}")
        return float(f"{dollars}.{cents}") if m.getDollars()>0 else -1

    def print_book(self, itrs, total_loop_time):
        num_bid_orders = self.ob.num_orders(book.OrderType.bid)
        num_ask_orders = self.ob.num_orders(book.OrderType.ask)
        num_bid_limits = self.ob.num_limits(book.OrderType.bid)
        num_ask_limits = self.ob.num_limits(book.OrderType.ask)
        highest_bid = self.format_money(self.ob.prominent_limit(book.OrderType.bid))
        lowest_ask  = self.format_money(self.ob.prominent_limit(book.OrderType.ask))
        highest_bid_limits = [self.format_money(m) for m in self.ob.get_limits(book.OrderType.bid, 10)]
        lowest_ask_limits  = [self.format_money(m) for m in self.ob.get_limits(book.OrderType.ask, 10)]
        orders_and_matches = (self.ob.num_orders(book.OrderType.bid) + self.ob.num_orders(book.OrderType.ask) + self.total_matches*2)
        print(f"""
            <------------- {itrs:,} ---------------->
            
            num_bid_orders: {num_bid_orders:,}
            num_ask_orders: {num_ask_orders:,}
            
            num_bid_limits: {num_bid_limits:,}
            num_ask_limits: {num_ask_limits:,}

            highest_bid: {highest_bid}
            lowest_ask:  {lowest_ask}

            highest_bid_limits: {highest_bid_limits}
            lowest_ask_limits:  {lowest_ask_limits}

            total_number_of_matches:  {self.total_matches:,}
            all_orders+(matches*2):   {orders_and_matches:,}

            best_add_time:  {self.best_add_time * (10**6)} mS
            worst_add_time: {self.worst_add_time * (10**6)} mS

            best_processing_time:  {self.best_processing_time * (10**6)} mS
            worst_processing_time: {self.worst_processing_time * (10**6)} mS

            total_add_time:        {self.total_add_time:,} S
            total_processing_time: {self.total_processing_time:,} S
            <------------- {total_loop_time:,} ---------------->
        """)

    def process_matches(self):
        process_start = time.time()
        matches       = self.ob.process()
        self.total_matches += len(matches)
        return matches, time.time() - process_start

    def place_order(self, shares=1, dollars=10, cents=0, orderType=book.OrderType.bid, user="albert"):
        start = time.time()
        order_id = self.ob.add_order(
            orderType, 
            user, 
            shares, 
            book.Money(dollars, cents)
        )
        return order_id, time.time()-start

    def add_random_order(self):
        highest_bid = self.ob.prominent_limit(book.OrderType.bid)
        lowest_ask  = self.ob.prominent_limit(book.OrderType.ask)

        if highest_bid.getDollars() < 0: 
            highest_bid = book.Money(100, 0)
        if lowest_ask.getDollars() < 0:
            lowest_ask = book.Money(100, 0)
        
        r1, r2, r3 = (
            round(random.random()*10 % 3), 
            int(round(random.random(), 2) * 10),
            int(round(random.random(), 2) * 10)
        ) 
        bidOrderType = book.OrderType.bid
        askOrderType = book.OrderType.ask
        if r1==0:   return self.place_order(r3, highest_bid.getDollars(), highest_bid.getCents() + r2, bidOrderType)
        elif r1==1: return self.place_order(r3, highest_bid.getDollars(), highest_bid.getCents() - r2, bidOrderType)
        elif r1==2: return self.place_order(r3, lowest_ask.getDollars(), lowest_ask.getCents() + r2, askOrderType)
        else:       return self.place_order(r3, lowest_ask.getDollars(), lowest_ask.getCents() - r2, askOrderType)

    def stimulate(self):
        itr = total_loop_time = 0
        iterations = 1000000
        while True: 
            itr+=1
            order_id, add_time = self.add_random_order()
            matches, processing_time = self.process_matches()
            
            self.total_matches  += len(matches)
            self.total_add_time += add_time 
            self.total_processing_time += processing_time
            total_loop_time += (add_time + processing_time)
            
            self.best_add_time  = min(self.best_add_time, add_time)
            self.worst_add_time = max(self.worst_add_time, processing_time)
            self.best_processing_time  = min(self.best_processing_time, processing_time)
            
            self.worst_processing_time = max(self.worst_processing_time, processing_time)
            if itr%iterations==0:
                self.print_book(itr, total_loop_time)
                total_loop_time = 0

if __name__ == "__main__":
    Stimulate(book.OrderBook()).stimulate()