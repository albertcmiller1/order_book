'''
1. create a book (IPO)
2. list all books 
3. place a limit or market order on a book 
4. cancel an order on a book 
5. stream data 
6. get spread 

multipropcess module? 
'''

import book
from typing import Dict, List, Union
from typing import get_type_hints, Optional

class API: 
    def __init__(self): 
        self.books: Dict[str, book.OrderBook] = {}

    def get_book(self, ticker: str, create: bool = True) -> Optional[book.OrderBook]: 
        if create and ticker not in self.books: 
            self.books[ticker]=book.OrderBook()
        return self.books.get(ticker)
    
    def place_order(self, ob: book.OrderBook, shares: int, dollars: int, cents: int, orderType: book.OrderType, user: str) -> str:
        return ob.add_order(
            orderType, 
            user, 
            shares, 
            book.Money(dollars, cents)
        )
    
    def process_matches(self, ob) -> list[book.Match]: 
        return ob.process()

    def format_money(self, m: book.Money) -> float:
        dollars = m.getDollars()
        cents   = m.getCents()
        if cents < 10 and cents >= 0:
            return float(f"{dollars}.0{cents}")
        return float(f"{dollars}.{cents}") if m.getDollars()>0 else -1

    def print_book(self, ob: book.OrderBook) -> None:
        highest_bid = self.format_money(ob.prominent_limit(book.OrderType.bid))
        lowest_ask  = self.format_money(ob.prominent_limit(book.OrderType.ask))

        highest_bid_limits = [self.format_money(m) for m in ob.get_limits(book.OrderType.bid, 10)]
        lowest_ask_limits  = [self.format_money(m) for m in ob.get_limits(book.OrderType.ask, 10)]

        print(f"""
            <----------------------------->
            num_bid_orders: {ob.num_orders(book.OrderType.bid):,}
            num_ask_orders: {ob.num_orders(book.OrderType.ask):,}
            
            num_bid_limits: {ob.num_limits(book.OrderType.bid):,}
            num_ask_limits: {ob.num_limits(book.OrderType.ask):,}

            highest_bid: {highest_bid}
            lowest_ask:  {lowest_ask}

            highest_bid_limits: {highest_bid_limits}
            lowest_ask_limits:  {lowest_ask_limits}
            <----------------------------->
        """)
    
    def print_match(self, match: book.Match) -> None: 
        print(f'''
            match_id: {match.match_id}
            bid_order_id: {match.bid_order_id}
            ask_order_id: {match.ask_order_id}
            shares: {match.shares}
            limit_price: {self.format_money(match.limit_price)}
        ''')

if __name__ == "__main__":
    api = API()
    tsla = api.get_book("TSLA")
    order_id = api.place_order(tsla, 100, 100, 10, book.OrderType.bid, "albert")
    order_id = api.place_order(tsla, 105, 100, 10, book.OrderType.ask, "albert")
    api.print_book(tsla)
    for match in api.process_matches(tsla): 
        api.print_match(match)
    api.print_book(tsla)
