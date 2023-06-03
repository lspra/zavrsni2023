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
		z() = int8 {

		}
}

main(x = uint16) {
	q = int8[11 + x][23];
	d = q[1][2];
	y = A(name(x, -x));
	print(y.z());
	if(x > 11) {
		for(i = 0; (i >> 2) < 17; i++++++) {
			if (i % 2)
				continue;
			else if(i**3 == 18)
				break;
		}
	}
}