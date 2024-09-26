#include "money.hpp"

void Money::normalize(){
    if (cents >= 100) {
        dollars += cents / 100;
        cents %= 100;
    } else if (cents <= -100) {
        dollars += cents / 100;
        cents = -(std::abs(cents) % 100);
    }

    if (dollars > 0 && cents < 0) {
        dollars -= 1;
        cents += 100;
    } else if (dollars < 0 && cents > 0) {
        dollars += 1;
        cents -= 100;
    }
}

long Money::getDollars() const {
    return this->dollars;
}

int Money::getCents() const { 
    return cents; 
}
void Money::setDollars(long dollars) {
    this->dollars = dollars;
    normalize();
}
void Money::setCents(int cents) {
    this->cents = cents;
    normalize();
}

Money Money::operator+(const Money& other) const {
    return Money(dollars + other.dollars, cents + other.cents);
}

Money Money::operator-(const Money& other) const {
    return Money(dollars - other.dollars, cents - other.cents);
}

Money Money::operator*(double factor) const {
    long totalCents = (dollars * 100 + cents) * factor;
    return Money(totalCents / 100, totalCents % 100);
}

Money Money::operator/(double divisor) const {
    long totalCents = (dollars * 100 + cents) / divisor;
    return Money(totalCents / 100, totalCents % 100);
}

bool Money::operator<(const Money& other) const {
    if (dollars < other.dollars) {
        return true;
    } else if (dollars == other.dollars) {
        return cents < other.cents;
    }
    return false;
}

bool Money::operator>(const Money& other) const {
    if (dollars > other.dollars) {
        return true;
    } else if (dollars == other.dollars) {
        return cents > other.cents;
    }
   return false;
}

bool Money::operator<=(const Money& other) const {
    return !(*this > other);
}

bool Money::operator>=(const Money& other) const {
    return !(*this < other);
}

bool Money::operator==(const Money& other) const {
    return dollars == other.dollars && cents == other.cents;
}