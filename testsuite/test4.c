//Basic flow control
int main()
{
	int a;
	int b;

	a = 2;
	b = 2;

	if(a == b)
	{
		b = 4;
		a = 7;
	}

	return a + b;
}