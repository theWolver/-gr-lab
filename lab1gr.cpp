#include <iostream>
#include <cmath>
#include <cstring>
#include<stdio.h>

using namespace std;

void HorizontalReflectionC(unsigned char* pi, int x, int y, int check)
{
    int i;
    for (i = 0; i < check * x * y; i ++)
        if ((i % (check * x)) < x / 2 * check)
            swap(pi[i], pi[check * x + i - 2 * (i % (check * x)) + 2 * (i % check) - check]);
}

void VerticalReflectionC(unsigned char* pi, int x, int y, int check)
{
    for (int i = 0; i < check * x * y; i ++)
        if ((i / (x * check)) < (y / 2))
            swap(pi[i], pi[check * x * y - x * check - check * x * (i / (x * check)) + (i % (check * x))]);
}

void RigthTurnC(unsigned char* pi, int x, int y, unsigned char* pix, int check)
{
    int k = 0;
    for (int i = 1; i <= x; i++)
        for (int j = y - 1; j >= 0; j--)
        {
            for (int t = 0; t < check; t ++)
            {
                int otv = j * x * check + i * check - check + t;
                pix[k + t] = pi [otv];
            }
            k = k + check;
        }
}

void LeftTurnC(unsigned char* pi, int x, int y, unsigned char* pix, int check)
{
    int k = 0;
    for (int i = x; i > 0; i--)
        for (int j = 0; j < y; j++)
        {
            for (int t = 0; t <= check - 1; t ++)
            {
                int otv = j * x * check + i * check - check + t;
                pix[k + t] = pi [otv];
            }
            k = k + check;
        }
}

void ColorInversionC(unsigned char* pi, int x, int y, int check)
{
    for (int i = 0; i < check * x * y; i ++)
        pi[i] = 255 - pi[i];
}

int main(int argc, char* argv[])
{
    char s[3];
    s[2] = '\0';
    int i, x, y, z, ch;
    FILE *fi = fopen(argv[1], "rb");
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
    if (strcmp(s,"P5") == 0)
        check = 1;
    if (strcmp(s,"P6") == 0)
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
        cout << "Just death";
        fclose(fi);
        return 1;
    }
    unsigned char *pi = new unsigned char [x * y * check];
    unsigned char *cpypi = new unsigned char[x * y * check];
    if (pi == NULL)
    {
        cout << "Out of memory";
        fclose(fi);
        return 1;
    }
    if (cpypi == NULL)
    {
        cout << "Out of memory";
        fclose(fi);
        return 1;
    }
    if (fread(pi, sizeof(unsigned char), x * y * check, fi) < x * y * check)
    {
        cerr << "Not enough bytes";
        fclose(fi);
        return 1;
    }
    if (argc != 4)
    {
        cerr << "Error no command";
        fclose(fi);
        return 1;
    }
    ch = atoi(argv[3]);
    fclose(fi);
    if ((ch < 0) || (ch > 4))
    {
        cerr << "Wrong command" << endl;
        return 1;
    }
    FILE *fo = fopen(argv[2], "wb");
    if (fo == NULL)
    {
        cout << "OUTPUT ERROR\n";
        return 1;
    }
    switch (ch)
    {
        case 0:
            ColorInversionC(pi, x, y, check);
            fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
            fwrite(pi, sizeof(unsigned char), x * y * check, fo);
            break;
        case 1:
            HorizontalReflectionC(pi, x, y, check);
            fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
            fwrite(pi, sizeof(unsigned char), x * y * check, fo);
            break;
        case 2:
            VerticalReflectionC(pi, x, y, check);
            fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
            fwrite(pi, sizeof(unsigned char), x * y * check, fo);
            break;
        case 3:
            RigthTurnC(pi, x, y, cpypi, check);
            fprintf(fo, "%s\n%i %i\n%i\n", s, y, x, z);
            fwrite(cpypi, sizeof(unsigned char), x * y * check, fo);
            break;
        case 4:
            LeftTurnC(pi, x, y, cpypi, check);
            fprintf(fo, "%s\n%i %i\n%i\n", s, y, x, z);
            fwrite(cpypi, sizeof(unsigned char), x * y * check, fo);
            break;
        default:
            cerr << "Wrong command";
            break;
    }
    delete [] pi;
    delete [] cpypi;
    fclose(fo);
    return 0;
}