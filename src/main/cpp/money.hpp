#include <cmath>
#include <algorithm>
#include <iomanip>

class Money {
private:
    long dollars;
    int cents;
    void normalize();
    void setDollars(long dollars);
    void setCents(int cents);
public:
    explicit Money(long dollars = 0, int cents = 0) : dollars(dollars), cents(cents) {
        normalize();
    }
    long getDollars() const;
    int getCents() const;
    Money operator+(const Money& other) const;
    Money operator-(const Money& other) const;
    Money operator*(double factor) const;
    Money operator/(double divisor) const;
    bool operator<(const Money& other) const;
    bool operator>(const Money& other) const;
    bool operator<=(const Money& other) const;
    bool operator>=(const Money& other) const;
    bool operator==(const Money& other) const;
};

struct MoneyHasher{
  std::size_t operator()(const Money& money) const{
    return std::hash<long>()(money.getDollars()) ^ (std::hash<int>()(money.getCents()) << 1);
  }
};