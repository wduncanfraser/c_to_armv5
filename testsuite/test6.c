//Nestedflow control
int main()
{
	int n;
	int m;

	n = 0;
	m = 10;

	while(n == 0)
	{
		--m;

		if(n == m)
		{
			n = 1;
		}
	}

	return n;
}