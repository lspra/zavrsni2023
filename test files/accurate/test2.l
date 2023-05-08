name(a = uint16, b = int16) = int8 {
    return a + b;
}

class A {
    private:
        a = int8;
    public:
        A(b = int8) {
            a = b;
        }
}

main(x = uint16) {
    A(name(x, -x));
    # print(A.a);
    if(x > 11) {
        for(i = 0; (i >> 2) < 17; i++++++) {
            if (i % 2)
                continue;
            else if(i**3 == 18)
                break;
        }
    }
}