from common import Stimulate
from behave import given, when, then
import book 

@given('access to an empty order book')
def step_impl(context):
    context.ob = book.OrderBook()
    context.stimulate = Stimulate(context.ob)
    assert context.ob.num_orders(book.OrderType.bid) == 0
    assert context.ob.num_orders(book.OrderType.ask) == 0
    assert context.ob.num_limits(book.OrderType.bid) == 0
    assert context.ob.num_limits(book.OrderType.ask) == 0
    assert context.ob.prominent_limit(book.OrderType.bid).getDollars() == -1
    assert context.ob.prominent_limit(book.OrderType.bid).getCents() == -1

@when('bids and asks are added')
def step_impl(context):
    for _ in range(100):
        context.ob.add_order(
            book.OrderType.bid, 
            "albert", 
            10, 
            book.Money(10, 0), 
        )
    for _ in range(10):
        context.ob.add_order(
            book.OrderType.ask, 
            "albert", 
            10, 
            book.Money(10, 0), 
        )
    context.matches = context.ob.process()

@then('the order book can corectly process matches')
def step_impl(context):
    assert len(context.matches) == 10
    assert context.ob.num_orders(book.OrderType.bid) == 90
    assert context.ob.num_orders(book.OrderType.ask) == 0
    assert context.ob.num_limits(book.OrderType.bid) == 1
    assert context.ob.num_limits(book.OrderType.ask) == 0
    assert context.ob.prominent_limit(book.OrderType.bid).getDollars() == 10
    assert context.ob.prominent_limit(book.OrderType.bid).getCents() == 0
    assert context.ob.prominent_limit(book.OrderType.ask).getDollars() == -1
    assert context.ob.prominent_limit(book.OrderType.ask).getCents() ==-1