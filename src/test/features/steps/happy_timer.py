from behave import given, when, then
import book, time
from common import Stimulate

@given('an empty order book')
def step_impl(context):
    context.ob = book.OrderBook()
    context.stimulate = Stimulate(context.ob)

    assert context.ob.num_orders(book.OrderType.bid) == 0
    assert context.ob.num_orders(book.OrderType.ask) == 0
    assert context.ob.num_limits(book.OrderType.bid) == 0
    assert context.ob.num_limits(book.OrderType.ask) == 0
    assert context.ob.prominent_limit(book.OrderType.bid).getDollars() == -1
    assert context.ob.prominent_limit(book.OrderType.bid).getCents() == -1

@when('we randomly add bids and asks for XX seconds')
def step_impl(context):
    context.stimulate.stimulate()


@then('the order book will process them swiftly')
def step_impl(context):
    assert len(context.matches) == 10
    assert context.ob.num_orders(book.OrderType.bid) == 90
    assert context.ob.num_orders(book.OrderType.ask) == 0
    assert context.ob.num_limits(book.OrderType.bid) == 1
    assert context.ob.num_limits(book.OrderType.ask) == 0
    assert context.ob.prominent_limit(book.OrderType.bid).getDollars() == 10
    assert context.ob.prominent_limit(book.OrderType.bid).getCents() == 0
    assert context.ob.prominent_limit(book.OrderType.ask).getDollars() == -1
    assert context.ob.prominent_limit(book.OrderType.ask).getCents() ==- 1