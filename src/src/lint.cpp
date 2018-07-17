#include "lint.h"

const static uint32_t MASK = ~0;

static uint32_t strToInt(std::string s){
    uint32_t cnt = 0;
    for(size_t i = 0; i < s.size(); i++){
        cnt = cnt*2 + (s[i] - '0');
    }
    return cnt;
}

static lint add(lint, lint);
static lint subtract(lint, lint);

static lint add(lint a, lint b){
    if(a.pos ^ b.pos){
        return subtract(a, -b);
    }else{
        lint c;
        c.pos = a.pos;
        uint64_t cnt = 0;
        for(int i = 0; i < 16; i++){
            cnt += a.dig[i];
            cnt += b.dig[i];
            c.dig[i] = cnt&MASK;
            cnt >>= 32;
        }
        return c;
    }
}

static lint subtract(lint a, lint b){
    if(a.pos ^ b.pos){
        return add(a, -b);
    }else{
        lint c;
        c.pos = a.pos;
        uint64_t cnt = 0;
        for(int i = 0; i < 16; i++){
            cnt += a.dig[i];
            cnt += (MASK^b.dig[i]);
            c.dig[i] = cnt&MASK;
            cnt >>= 32;
        }
        if(cnt > 0){
            for(int i = 0; i < 16; i++){
                cnt += c.dig[i];
                c.dig[i] = cnt&MASK;
                cnt >>= 32;
            }
        }else{
            c.pos = !c.pos;
            for(int i = 0; i < 16; i++){
                c.dig[i] ^= MASK;
            }
        }
        return c;
    }
}

lint::lint(){
    for(int i = 0; i < 16; i++){
        dig[i] = 0;
    }
    pos = true;
}

lint::lint(int num){
    for(int i = 0; i < 16; i++){
        dig[i] = 0;
    }
    if(num < 0){
        dig[0] = -num;
        pos = false;
    }else{
        dig[0] = num;
        pos = true;
    }
}

lint::lint(long long int num){
    for(int i = 0; i < 16; i++){
        dig[i] = 0;
    }
    if(num < 0){
        pos = false;
        num = -num;
    }else{
        pos = true;
    }
    dig[0] = num&MASK;
    dig[1] = num>>32;
}

lint::lint(std::string num){
    if(num[0] == '-'){
        num = num.substr(1);
        pos = false;
    }else{
        pos = true;
    }
    int k = 0;
    for(int i = (int)num.size()-1; i >= 0; i-= 32){
        std::string t;
        if(i < 32)t = num.substr(0, i+1);
        else t = num.substr(i-31, 32);
        dig[k] = strToInt(t);
        k++;
    }
    while(k < 16){
        dig[k] = 0;
        k++;
    }
}

lint::lint(char* s){
    std::string num(s);
    if(num[0] == '-'){
        num = num.substr(1);
        pos = false;
    }else{
        pos = true;
    }
    int k = 0;
    for(int i = (int)num.size()-1; i >= 0; i-= 32){
        std::string t;
        if(i < 32)t = num.substr(0, i+1);
        else t = num.substr(i-31, 32);
        dig[k] = strToInt(t);
        k++;
    }
    while(k < 16){
        dig[k] = 0;
        k++;
    }
}

lint::lint(const lint& num){
    for(int i = 0; i < 16; i++){
        dig[i] = num.dig[i];
    }
    pos = num.pos;
}

const lint& lint::operator=(int num){
    for(int i = 0; i < 16; i++){
        dig[i] = 0;
    }
    if(num < 0){
        dig[0] = -num;
        pos = false;
    }else{
        dig[0] = num;
        pos = true;
    }
    return *this;
}

const lint& lint::operator=(long long int num){
    for(int i = 0; i < 16; i++){
        dig[i] = 0;
    }
    if(num < 0){
        pos = false;
        num = -num;
    }else{
        pos = true;
    }
    dig[0] = num&MASK;
    dig[1] = num>>32;
    return *this;
}

const lint& lint::operator=(std::string num){
    if(num[0] == '-'){
        num = num.substr(1);
        pos = false;
    }else{
        pos = true;
    }
    int k = 0;
    for(int i = (int)num.size()-1; i >= 0; i-= 32){
        std::string t;
        if(i < 32)t = num.substr(0, i+1);
        else t = num.substr(i-31, 32);
        dig[k] = strToInt(t);
        k++;
    }
    while(k < 16){
        dig[k] = 0;
        k++;
    }
    return *this;
}

const lint& lint::operator=(char* s){
    std::string num(s);
    if(num[0] == '-'){
        num = num.substr(1);
        pos = false;
    }else{
        pos = true;
    }
    int k = 0;
    for(int i = (int)num.size()-1; i >= 0; i-= 32){
        std::string t;
        if(i < 32)t = num.substr(0, i+1);
        else t = num.substr(i-31, 32);
        dig[k] = strToInt(t);
        k++;
    }
    while(k < 16){
        dig[k] = 0;
        k++;
    }
    return *this;
}

const lint& lint::operator=(const lint& num){
    for(int i = 0; i < 16; i++){
        dig[i] = num.dig[i];
    }
    pos = num.pos;
    return *this;
}

lint lint::operator-() const{
    lint tmp;
    for(int i = 0; i < 16; i++){
        tmp.dig[i] = dig[i];
    }
    tmp.pos = !pos;
    return tmp;
}

lint lint::operator+(const lint& num) const{
    lint tmp = add(*this, num);
    if(-tmp == 0)return 0;
    return tmp;
}

lint lint::operator-(const lint& num) const{
    lint tmp = subtract(*this, num);
    if(-tmp == 0)return 0;
    return tmp;
}

lint lint::operator*(const lint& num) const{
    lint tmp;
    int max_i = 15, max_j = 15;
    while(dig[max_i] == 0 && max_i >= 0)max_i--;
    while(num.dig[max_j] == 0 && max_j >= 0)max_j--;
    for(int i = 0; i <= max_i; i++){
        for(int j = 0; j <= max_j; j++){
            int k = i+j;
            uint64_t cnt = dig[i];
            cnt *= num.dig[j];
            while(cnt > 0 && k < 16){
                cnt += tmp.dig[k];
                tmp.dig[k] = cnt&MASK;
                cnt >>= 32;
                k++;
            }
        }
    }
    tmp.pos = !(pos ^ num.pos);
    if(-tmp == 0)return -tmp;
    return tmp;
}

lint lint::operator/(const lint& num) const{
    lint a(*this), b(num), ans;
    a.pos = true;
    b.pos = true;
    while(a >= b){
        lint c = b, d = 1;
        c =  c << 1;
        while(a >= c){
            d = d << 1;
            c = c << 1;
        }
        a = a - d*b;
        ans = ans + d;
    }
    if(-a == 0)a = 0;
    if(pos && !num.pos)ans = -ans;
    if(!pos && num.pos){
        if(a == 0)ans = -ans;
        else ans = -ans - 1;
    }
    if(!pos && !num.pos){
        if(a != 0) ans = ans - 1;
    }
    if(-ans == 0)ans = -ans;
    return ans;
}

lint lint::operator%(const lint& num) const{
    lint a(*this), b(num), ans;
    a.pos = true;
    b.pos = true;
    while(a >= b){
        lint c = b, d = 1;
        c =  c << 1;
        while(a >= c){
            d = d << 1;
            c = c << 1;
        }
        a = a - d*b;
        ans = ans + d;
    }
    if(-a == 0)a = 0;
    if(!pos && num.pos){
        if(a != 0)a = b - a;
    }
    if(!pos && !num.pos){
        if(a != 0)a = b - a;
    }
    return a;
}

static lint shiftLeft(lint num){
    lint ans(num);
    for(int i = 0; i < 16; i++){
        ans.dig[i] <<= 1;
    }
    uint32_t tmp = MASK^(MASK>>1);
    for(int i = 1; i < 16; i++){
        if(num.dig[i-1] >= tmp){
            ans.dig[i] |= 1;
        }
    }
    return ans;
}

static lint shiftRight(lint num){
    lint ans(num);
    for(int i = 0; i < 16; i++){
        ans.dig[i] >>= 1;
    }
    uint32_t tmp = MASK^(MASK>>1);
    for(int i = 0; i < 15; i++){
        if(num.dig[i+1]%2 == 1){
            ans.dig[i] |= tmp;
        }
    }
    return ans;
}

lint lint::operator<<(int num) const{
    lint ans(*this);
    while(num > 0){
        ans = shiftLeft(ans);
        num--;
    }
    return ans;
}

lint lint::operator>>(int num) const{
    lint ans(*this);
    while(num > 0){
        ans = shiftRight(ans);
        num--;
    }
    return ans;
}

bool lint::operator==(const lint& num) const{
    if(pos != num.pos)return false;
    for(int i = 0; i < 16; i++){
        if(dig[i] != num.dig[i])return false;
    }
    return true;
}

bool lint::operator!=(const lint& num) const{
    if(pos != num.pos)return true;
    for(int i = 0; i < 16; i++){
        if(dig[i] != num.dig[i])return true;
    }
    return false;
}

bool lint::operator<=(const lint& num) const{
    if(pos && !num.pos)return false;
    if(!pos && num.pos)return true;
    for(int i = 15; i >= 0; i--){
        if(dig[i] < num.dig[i])return (pos?true:false);
        if(dig[i] > num.dig[i])return (pos?false:true);
    }
    return true;
}

bool lint::operator>=(const lint& num) const{
    if(pos && !num.pos)return true;
    if(!pos && num.pos)return false;
    for(int i = 15; i >= 0; i--){
        if(dig[i] > num.dig[i])return (pos?true:false);
        if(dig[i] < num.dig[i])return (pos?false:true);
    }
    return true;
}

bool lint::operator<(const lint& num) const{
    if(pos && !num.pos)return false;
    if(!pos && num.pos)return true;
    for(int i = 15; i >= 0; i--){
        if(dig[i] < num.dig[i])return (pos?true:false);
        if(dig[i] > num.dig[i])return (pos?false:true);
    }
    return false;
}

bool lint::operator>(const lint& num) const{
    if(pos && !num.pos)return true;
    if(!pos && num.pos)return false;
    for(int i = 15; i >= 0; i--){
        if(dig[i] > num.dig[i])return (pos?true:false);
        if(dig[i] < num.dig[i])return (pos?false:true);
    }
    return false;
}
