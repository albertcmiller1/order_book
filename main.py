from build import book
from src.main.py.main import arg_parse

ob = book.OrderBook()
print(ob.generate_order_id())
arg_parse()