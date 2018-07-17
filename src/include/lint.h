#ifndef LINT_H
#define LINT_H

#include <string>

struct lint{
public:
    uint32_t dig[16];
    bool pos;

    lint();
    lint(int);
    lint(long long int);
    lint(std::string);
    lint(char*);
    lint(const lint&);

    const lint& operator=(int);
    const lint& operator=(long long int);
    const lint& operator=(std::string);
    const lint& operator=(char*);
    const lint& operator=(const lint&);

    lint operator-() const;
    lint operator+(const lint&) const;
    lint operator-(const lint&) const;
    lint operator*(const lint&) const;
    lint operator/(const lint&) const;
    lint operator%(const lint&) const;
    lint operator<<(int) const;
    lint operator>>(int) const;

    bool operator==(const lint& num) const;
    bool operator!=(const lint& num) const;
    bool operator<=(const lint& num) const;
    bool operator>=(const lint& num) const;
    bool operator<(const lint& num) const;
    bool operator>(const lint& num) const;
};

#endif
