 #include <iostream>
#include <algorithm>
#include <cmath>
#include<utility>
#include<cstring>

using namespace std;

void Change_in_RGB(unsigned char* pi, int check, int x, int y, int displacement, double factor)
{
	for (int i = 0; i < x * y * check; i++)
	{
		double t = pi[i];
		t = (t - displacement * 1.0) * factor;
		if (t > 255)
			t = 255;
		if (t < 0)
			t = 0;
		pi[i] = round(t);
	}
}

void Get_YCbCr(unsigned char* pi, unsigned char* cpypi, int x, int y)
{
	for (int i = 0; i < x * y; i++)
	{
		cpypi[i * 3] = round(0.299 * pi[i * 3] + 0.587 * pi[i * 3 + 1] + 0.114 * pi[i * 3 + 2]);
		cpypi[i * 3 + 1] = round(128 - 0.168736 * pi[i * 3] - 0.331264 * pi[i * 3 + 1] + 0.5 * pi[i * 3 + 2]);
		cpypi[i * 3 + 2] = round(128 + 0.5 * pi[i * 3 ] - 0.418688 * pi[i * 3 + 1] - 0.081312 * pi[i * 3 + 2]);
	}
}
void Change_Y_in_YCbCr(unsigned char* cpypi, int x, int y, int check, double displacement, double factor)
{
	for (int i = 0; i < x * y; i++)
	{
		int ch = (cpypi[i * check] - displacement) * factor;
		ch = min(ch, 255);
		ch = max(ch, 0);
		cpypi[i * check] = ch;
	}
}

unsigned char UC(double t)
{
	if (t > 255)
		return 255;
	if (t < 0)
		return 0;
	return round(t);
}

unsigned char findmin(unsigned char* pi, int x, int y, int check)
{
	unsigned char minim = 255;
	for (int i = 0; i < x * y * check; i++)
		minim = min(pi[i], minim);
	return minim;
}

unsigned char findmax(unsigned char* pi, int x, int y, int check)
{
	unsigned char maxim = 0;
	for (int i = 0; i < x * y * check; i++)
		maxim = max(maxim, pi[i]);
	return maxim;
}

void Dependent_Max_RGB(unsigned char* pi, int x, int y, int check)
{
	unsigned char minim = findmin(pi, x, y, check);
	double maxim = findmax(pi, x, y, check);
	Change_in_RGB(pi, check, x, y, minim, (255.0 / (maxim * 1.0 - minim * 1.0)));
	cout << minim * 1.0 << " " << (255.0 / (maxim * 1.0 - minim * 1.0)) << endl;
}

pair <int, int> Max_Min_counted(unsigned char* pi, int x, int y, int check)
{
	int arr[270];
	int maxim = -1, minim = 256;
	for (int i = 0; i <= 257; i++)
		arr[i] = 0;
//	cout << "!" << endl;
	for (int i = 0; i < x * y * check; i++)
	{
		arr[pi[i]]++;
	}
	double counter = 0, fl = 1;
	for (int i = 256; i >= 0; i--)
	{
		counter += arr[i];
		if ((counter / double(x * y * check) > 0.0039))
		{
			maxim = i;
			i = -5;
		}
	}
	counter = 0;
	for (int i = 0; i < 256; i++)
	{
		counter += arr[i];
		if (counter / double(x * y * check) > 0.0039)
		{
			minim = i;
			i = 300;
		}
	}
//	cout << minim * 1.0 << " " << maxim * 1.0 << endl;
	pair <int, int> Tech_pair;
	Tech_pair = make_pair(minim, maxim);
	return Tech_pair;
}

pair <int, int> Double_Max_Min_counted(unsigned char* cpypi, int x, int y, int check)
{
	int arr[270];
	int maxim = -1, minim = 256;
	for (int i = 0; i < 260; i++)
		arr[i] = 0;
	int l;
	double t;
	for (int i = 0; i < x * y; i++)
	{
		t = cpypi[i * check];
		l = round(t);
		arr[l]++;
	}
	double counter = 0, fl = 1;
	for (int i = 256; i >= 0; i--)
	{
		counter += arr[i];
//		cout << i << " " << arr[i] << " " << counter << " " << counter * 1.0 / x / y * 100.0 << endl;
		if (counter  / double(x * y) > 0.0039)
		{
			maxim = i;
			i = -5;
		}
	}
	counter = 0;
	for (int i = 0; i < 260; i++)
	{
		counter += arr[i];
		if (counter / double(x * y) > 0.0039)
		{
			minim = i;
			i = 2700;
		}
	}
	pair <int, int> Tech_pair;
	Tech_pair = make_pair(minim, maxim);
	return Tech_pair;
}

void Change_Counted_YCbCr(unsigned char* cpypi, int x, int y, int check)
{
	pair<int, int> Tech_pair;
	Tech_pair = Double_Max_Min_counted(cpypi, x, y, check);
	int maxim, minim;
	maxim = Tech_pair.second;
	minim = Tech_pair.first;
//	cout << minim << " " << maxim << endl;
	if ((maxim == 256) || (minim == -1))
	{
		cerr << "Your picture was not created for this frail algorithm";
		exit(1);
	}
	cout << minim << " " << (255.0 / (maxim * 1.0 - minim * 1.0)) << endl;
	Change_Y_in_YCbCr(cpypi, x, y, check, minim, (255.0 / (maxim - minim)));
}


void Change_Counted_RGB(unsigned char* pi, int x, int y, int check)
{
	pair<int, int> Tech_pair;
	Tech_pair = Max_Min_counted(pi, x, y, check);
	double maxim = Tech_pair.second, minim = Tech_pair.first;
//	cout << maxim << " " << minim << endl; // tech_cout
	if ((minim == 256) || (maxim == -1))
	{
		cerr << "Your picture was not created for this frail algorithm";
		exit(1);
	}
	Change_in_RGB(pi, check, x, y, minim, (255.0 / (maxim * 1.0 - minim * 1.0)));
	cout << minim * 1.0 << " " << (255.0 / (maxim * 1.0 - minim * 1.0)) << endl;
}

void Get_RGB(unsigned char* cpypi, unsigned char* pi, int x, int y)
{
	double Some_copy;
	for (int i = 0; i < x * y; i++)
	{
		Some_copy = round(cpypi[i * 3] + 1.402 * (cpypi[i * 3 + 2] - 128));
		pi[i * 3] = UC(Some_copy);
		Some_copy = round(cpypi[i * 3] - 0.34414 * (cpypi[i * 3 + 1] - 128) - 0.71414 * (cpypi[i * 3 + 2] - 128));
		pi[i * 3 + 1] = UC(Some_copy);
		Some_copy = round(cpypi[i * 3] + 1.772 * (cpypi[i * 3 + 1] - 128));
		pi[i * 3 + 2] = UC(Some_copy);
	}
}

int findminY(unsigned char* cpypi, int x, int y, int check)
{
	int minim = 1000;
	for (int i = 0; i < x * y; i++)
	{
		minim = min(minim, int(cpypi[i * check]));
	}
	return minim;
}

int findmaxY(unsigned char* cpypi, int x, int y, int check)
{
	int maxim = -1;
	for (int i = 0; i < x * y; i++)
	{
		maxim = max(maxim, int(cpypi[i * check]));
	}
	return maxim;
}

void Dependent_Max_YCbCr(unsigned char* cpypi, int x, int y, int check)
{
	int minim = findminY(cpypi, x, y, check);
	int maxim = findmaxY(cpypi, x, y, check);
	Change_Y_in_YCbCr(cpypi, x, y, check, round(minim), (255.0 / (round(maxim) - round(minim))));
	cout << round(minim) << " " << (255.0 / (round(maxim) - round(minim))) << endl;
}

int main(int argc, char* argv[])
{
	char s[100];
	int i, x, y, z, ch;
	FILE* fi = fopen(argv[1], "rb");
	FILE* fo = fopen(argv[2], "wb");
	if (fi == NULL)
	{
		cerr << "INPUT ERROR\n";
		return 1;
	}
	if (fscanf(fi, "%s", s) == 0)
	{
		cerr << "Critical damage name" << endl;
		fclose(fi);
		return 1;
	}
	int check = 0;
	if (strcmp(s, "P5") == 0)
		check = 1;
	if (strcmp(s, "P6") == 0)
		check = 3;
	if (check == 0)
	{
		cerr << "Wrong type of image";
		fclose(fi);
		return 1;
	}
	if (fscanf(fi, "%i", &x) == 0)
	{
		cerr << "Critical damage size of picture" << endl;
		fclose(fi);
		return 1;
	}
	if (fscanf(fi, "%i", &y) == 0)
	{
		cerr << "Critical damage size of picture" << endl;
		fclose(fi);
		return 1;
	}
	if (fscanf(fi, "%i\n", &z) == 0)
	{
		cerr << "Critical damage pixel dimension" << endl;
		fclose(fi);
		return 1;
	}
	if ((x < 0) || (y < 0) || (z < 0))
	{
		cerr << "Just death";
		fclose(fi);
		return 1;
	}
	unsigned char* pi = new unsigned char[x * y * check];
	unsigned char* cpypi = new unsigned char[x * y * check];
	if (pi == nullptr)
	{
		cerr << "Out of memory";
		fclose(fi);
		return 1;
	}
	if (cpypi == nullptr)
	{
		cerr << "Out of memory";
		fclose(fi);
		return 1;
	}
	if (fread(pi, sizeof(unsigned char), x * y * check, fi) < x * y * check)
	{
		cerr << "Not enough bytes";
		fclose(fi);
		return 1;
	}
	int conversion, displacement;
	double factor;
	conversion = atoi(argv[3]);
	if (conversion < 2)
	{
		if (argc < 6)
		{
			cerr << "Wrong Input of Commands";
			exit(1);
		}
		displacement = atoi(argv[4]);
		factor = strtod(argv[5], NULL);
		if (displacement < -255 || displacement > 255)
		{
			cerr << "Wrong displacement";
			return 1;
		}
		if (factor < 1.0 / 255.0 || factor > 255)
		{
			cerr << "Wrong factor";
			return 1;
		}
	}
	if (conversion < 0 || conversion > 5)
	{
		cerr << "Wrong type of conversion";
		return 1;
	}
	switch (conversion)
	{
	case 0:
		Change_in_RGB(pi, x, y, check, displacement, factor);
		break;
	case 1:
		if (check == 3)
		{
			Get_YCbCr(pi, cpypi, x, y);
			Change_Y_in_YCbCr(cpypi, x, y, check, displacement, factor);
			Get_RGB(cpypi, pi, x, y);
		}
		else
		{
			// if check == 1 -> YCbCr == RGB
			Change_in_RGB(pi, x, y, check, displacement, factor);
		}
		break;
	case 2:
		Dependent_Max_RGB(pi, x, y, check);
		break;
	case 3:
		if (check == 3)
		{
			Get_YCbCr(pi, cpypi, x, y);
			Dependent_Max_YCbCr(cpypi, x, y, check);
			Get_RGB(cpypi, pi, x, y);
		}
		else
		{
			// same
			Dependent_Max_RGB(pi, x, y, check);
		}
		break;
	case 4:
		Change_Counted_RGB(pi, x, y, check);
		break;
	case 5:
		if (check == 3)
		{
			Get_YCbCr(pi, cpypi, x, y);
			Change_Counted_YCbCr(cpypi, x, y, check);
			Get_RGB(cpypi, pi, x, y);
		}
		else
		{
			// sameX2
			Change_Counted_RGB(pi, x, y, check);
		}
		break;
	default:
		break;
	}
	fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
	fwrite(pi, sizeof(unsigned char), x * y * check, fo);
	delete[] pi;
	delete[] cpypi;
	fclose(fo);
	return 0;
}
