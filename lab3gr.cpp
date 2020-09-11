 #include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include<stdio.h>

using namespace std;
double Matr[8][8] =
{
    {0, 48.0 / 64, 12.0 / 64, 60.0 / 64, 3.0 / 64, 51.0 / 64, 15.0 / 64, 63.0 / 64},
    {32.0 / 64, 16.0 / 64, 44.0 / 64, 28.0 / 64, 35.0 / 64, 19.0 / 64, 47.0 / 64, 31.0 / 64},
    {8.0 / 64, 56.0 / 64, 4.0 / 64, 52.0 / 64, 11.0 / 64, 59.0 / 64, 7.0 / 64, 55.0 / 64},
    {40.0 / 64, 24.0 / 64, 36.0 / 64, 20.0 / 64, 43.0 / 64, 27.0 / 64, 39.0 / 64, 23.0 / 64},
    {2.0 / 64, 50.0 / 64, 14.0 / 64, 62.0 / 64, 1.0 / 64, 49.0 / 64, 13.0 / 64, 61.0 / 64},
    {34.0 / 64, 18.0 / 64, 46.0 / 64, 30.0 / 64, 33.0 / 64, 17.0 / 64, 45.0 / 64, 29.0 / 64},
    {10.0 / 64, 58.0 / 64, 6.0 / 64, 54.0 / 64, 9.0 / 64, 57.0 / 64, 5.0 / 64, 53.0 / 64},
    {42.0 / 64, 26.0 / 64, 38.0 / 64, 22.0 / 64, 41.0 / 64, 25.0 / 64, 37.0 / 64, 21.0 / 64}
};
double matr4x4[4][4] =
{
    {7.0 / 16.0, 13.0 / 16.0, 11.0 / 16.0, 4.0 / 16.0},
    {12.0 / 16.0, 16.0 / 16.0, 14.0 / 16.0, 8.0 / 16.0},
    {10.0 / 16.0, 15.0 / 16.0, 6.0 / 16.0, 2.0 / 16.0},
    {5.0 / 16.0, 9.0 / 16.0, 3.0 / 16.0, 1.0 / 16.0}
};

double gamma_correction_to_linear(double pi, double gamma)
{
    return round(255.0 * pow(pi / 255.0, 1.0 / gamma));
}

double gamma_correction_fr_linear(double pi, double gamma)
{
    return round(255.0 * pow(pi / 255.0, gamma));
}

double sRGB_correction(double pi)
{
    if (pi / 255.0 <= 0.0031308)
        return min(round((12.92 * pi / 255.0) * 255.0), 255.0);
    else
        return min(round((1.055 * pow(pi / 255.0, (1.0 / 2.4)) - 0.055 ) * 255.0), 255.0);
}

double from_sRGB_to_RGB(double pi)
{
    if (pi / 255.0 <= 0.04045)
        return min((round(pi / 255.0 / 12.92) * 255.0), 255.0);
    else
        return min(round(pow(((200.0 * pi / 255.0 + 11) / 211.0), 12.0 / 5.0) * 255.0), 255.0);
}

void drawgradient(double* pi, int x, int y, double gamma)
{
	double copp;
    for (int i = 0; i < x * y; i++)
	{
		copp = (i % x) / (x * 1.0 / 255.0);
        if (gamma == 0)
            pi[i] = from_sRGB_to_RGB(copp);
        else
            pi[i] = gamma_correction_fr_linear(copp, gamma);
	}
}



unsigned char NearestInPallet(unsigned char pixel, int sizeofPallet)
{
    int copy = pixel >> (8 - sizeofPallet);
    int cop1 = 0;
    for (int i = 1; i * sizeofPallet <= 8; i++)
    {
        int sdv = 8 - i * sizeofPallet;
        cop1 += copy << sdv;
    }
    return cop1;
}
void OrderedDithering(double* pi, int x, int y, int Bits, double gamma)
{
    double cpy = 255.0 / (Bits * 1.0);
    double t;
    double techt;
    for (int i = 0; i < x * y; i++)
    {
        if (gamma == 0)
            t = sRGB_correction(pi[i]) + round(cpy * (Matr[(i % x) % 8][(i / x) % 8] - 0.5));
        else
            t = gamma_correction_to_linear(pi[i], gamma) + round(cpy * (Matr[(i % x) % 8][(i / x) % 8] - 0.5));
        t = max(t, 0.0);
        t = min(t, 255.0);
        t = NearestInPallet(round(t), Bits);
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(t, Bits);
    }
}

void Halftone(double* pi, int x, int y, int Bits, double gamma)
{
    double cpy = 255.0 / (Bits * 1.0);
    double t;
    double techt;
    for (int i = 0; i < x * y; i++)
    {
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        t = copp + round(cpy * (matr4x4[(i % x) % 4][(i / x) % 4] - 0.5));
        t = min(255.0, t);
        t = max(0.0, t);
        t = NearestInPallet(round(t), Bits);
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(t, Bits);
    }
}

void WithoutDithering(double* pi, int x, int y, int Bits, double gamma)
{
    for (int i = 0; i < x * y; i++)
    {
//		pi[i] = gamma_correction_to_linear(pi[i], gamma);
        pi[i] = NearestInPallet(pi[i], Bits);
//        pi[i] = gamma_correction_fr_linear(pi[i], gamma);
	}
}
void JarviceDithering(double* pi, double* cpypi, int x, int y, int Bits, double gamma)
{
    for (int i = 0; i < x * y; i++)
    {
        double old;
        if (gamma == 0)
            old = sRGB_correction(pi[i]);
        else
            old = gamma_correction_to_linear(pi[i], gamma);
        double t = old + round(cpypi[i]);
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        if (t < 0)
            t = 0;
        if (t > 255)
            t = 255;
        pi[i] = NearestInPallet(round(t), Bits);
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        double err = old + cpypi[i] - copp;
        if (((i + 1) % x > i % x) && ((i + 1) < x * y))
            cpypi[i + 1] += (err * 7.0 / 48.0);
        if (((i + 2) % x > i % x) && ((i + 2) < x * y))
            cpypi[i + 2] += (err * 5.0 / 48.0);
        if ((i + x) < x * y)
            cpypi[i + x] += (err * 7.0 / 48.0);
        if (((i + x - 2) < x * y) && (i % x > (i + x - 2) % x))
            cpypi[i + x - 2] += ((err) * 3.0 / 48.0);
        if ((i + x - 1 < x * y) && (i % x > (i + x - 1) % x))
            cpypi[i + x - 1] += ((err) * 5.0 / 48.0);
        if ((i + x + 1 < x * y) && (i % x < (i + x + 1) % x))
            cpypi[i + x + 1] += ((err) * 5.0 / 48.0);
        if ((i + x + 2 < x * y) && (i % x < (i + x + 2) % x))
            cpypi[i + x + 2] += ((err) * 3.0 / 48.0);
        if ((i + 2 * x - 2 < x * y) && (i % x > (i + 2 * x - 2) % x))
            cpypi[i + 2 * x - 2] += ((err) * 1.0 / 48.0);
        if ((i + 2 * x - 1 < x * y) && (i % x > (i + 2 * x - 1) % x))
            cpypi[i + 2 * x - 1] += ((err) * 3.0 / 48.0);
        if (i + 2 * x < x * y)
            cpypi[i + 2 * x] += ((err) * 5.0 / 48.0);
        if ((i + 2 * x + 1 < x * y) && ((i + 2 * x + 1) % x > i % x))
            cpypi[i + 2 * x + 1] += ((err) * 3.0 / 48.0);
        if ((i + 2 * x + 2 < x * y) && ((i + 2 * x + 2) % x > i % x))
            cpypi[i + 2 * x + 2] += ((err) * 1.0 / 48.0);
    }
}

void Siera(double* pi, double* cpypi, int x, int y, int Bits, double gamma)
{
    for (int i = 0; i < x * y; i++)
    {
        double old = pi[i];
        if (gamma == 0)
            old = sRGB_correction(pi[i]);
        else
            old = gamma_correction_to_linear(pi[i], gamma);
        double t = old + round(cpypi[i]);
        if (t < 0)
            t = 0;
        if (t > 255)
            t = 255;
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(round(t), Bits);
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        double err = old + cpypi[i] - copp;
        if ((i % x < (i + 1) % x) && (i + 1 < x * y))
            cpypi[i + 1] += (err * 5.0 / 32.0);
        if ((i % x < (i + 2) % x) && (i + 2 < x * y))
            cpypi[i + 2] += (err * 3.0 / 32.0);
        if ((i % x > (i + x - 2) % x) && (i + x - 2 < x * y))
            cpypi[i + x - 2] += (err * 2.0 / 32.0);
        if ((i % x > (i + x - 1) % x) && (i + x - 1 < x * y))
            cpypi[i + x - 1] += (err * 4.0 / 32.0);
        if (i + x < x * y)
            cpypi[i + x] += (err * 5.0 / 32.0);
        if ((i % x > (i + x + 1) % x) && (i + x + 1 < x * y))
            cpypi[i + x + 1] += (err * 4.0 / 32.0);
        if ((i % x > (i + x + 2) % x) && (i + x + 2 < x * y))
            cpypi[i + x + 2] += (err * 2.0 / 32.0);
        if ((i % x > (i + 2 * x - 1) % x) && (i + 2 * x - 1 < x * y))
            cpypi[i + 2 * x - 1] += (err * 2.0 / 32.0);
        if (i + 2 * x < x * y)
            cpypi[i + 2 * x] += (err * 3.0 / 32.0);
        if ((i % x > (i + 2 * x - 1) % x) && (i + 2 * x + 1 < x * y))
            cpypi[i + 2 * x + 1] += (err * 2.0 / 32.0);
    }
}

void Atkinson(double* pi, double* cpypi, int x, int y, int Bits, double gamma)
{
    for (int i = 0; i < x * y; i++)
    {
        double old = pi[i];
        if (gamma == 0)
            old = sRGB_correction(pi[i]);
        else
            old = gamma_correction_to_linear(pi[i], gamma);
        double t = old + round(cpypi[i]);
        if (t < 0)
            t = 0;
        if (t > 255)
            t = 255;
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(round(t), Bits);
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        double err = old + cpypi[i] - copp;
        if ((i % x < (i + 1) % x) && (i + 1 < x * y))
            cpypi[i + 1] += (err * 1.0 / 8.0);
        if ((i % x < (i + 2) % x) && (i + 2 < x * y))
            cpypi[i + 2] += (err * 1.0 / 8.0);
        if (i + x < x * y)
            cpypi[i + x] += (err * 1.0 / 8.0);
        if ((i + x - 1 < x * y) && (i % x > (i - 1) % x))
            cpypi[i + x - 1] += (err * 1.0 / 8.0);
        if ((i + x + 1 < x * y) && (i % x < (i + 1) % x))
            cpypi[i + x + 1] += (err * 1.0 / 8.0);
        if (i + 2 * x < x * y)
            cpypi[i + 2 * x] += (err * 1.0 / 8.0);
    }
}

void RandomDithering(double* pi, int x, int y, int Bits, double gamma)
{
    double cpy = 255.0 / (Bits * 1.0);
    double t;
    double techt;
    for (int i = 0; i < x * y; i++)
    {
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        t = copp + round(cpy * ((rand() % 64) / 64.0 - 0.5));
        t = min(255.0, t);
        t = max(0.0, t);
        t = NearestInPallet(t, Bits);
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(round(t), Bits);
    }
}

void Floyd_Steinberg(double* pi, double* cpypi, int x, int y, int Bits, double gamma)
{
    for (int i = 0; i < x * y; i++)
    {
        double old;
        if (gamma == 0)
            old = sRGB_correction(pi[i]);
        else
            old = gamma_correction_to_linear(pi[i], gamma);
        double t = old + round(cpypi[i]);
        if (t < 0)
            t = 0;
        if (t > 255)
            t = 255;
        if (gamma == 0)
            t = from_sRGB_to_RGB(t);
        else
            t = gamma_correction_fr_linear(t, gamma);
        pi[i] = NearestInPallet(round(t), Bits);
        double copp;
        if (gamma == 0)
            copp = sRGB_correction(pi[i]);
        else
            copp = gamma_correction_to_linear(pi[i], gamma);
        double err = old + cpypi[i] - copp;
        if ((i / x < y - 1) && (i % x == x - 1))
        {
            cpypi[i + x] += (err * 5.0 / 16.0);
            cpypi[i + x - 1] += (err * 3.0 / 16.0);
        }
        if ((i / x < y - 1) && (i % x == 0))
        {
            cpypi[i + 1] += (err * 7.0 / 16.0);
            cpypi[i + x] += (err * 5.0 / 16.0);
            cpypi[i + x + 1] += (err * 1.0 / 16.0);
        }
        if ((i / x < y - 1) && (i % x > 0) && (i % x < x - 1))
        {
            cpypi[i + 1] += (err * 7.0 / 16.0);
            cpypi[i + x] += (err * 5.0 / 16.0);
            cpypi[i + x + 1] += (err * 1.0 / 16.0);
            cpypi[i + x - 1] += (err * 3.0 / 16.0);
        }
        if ((i / x == y - 1) && (i % x < x - 1))
            cpypi[i + 1] += (err * 7.0 / 16.0);
    }
}


void copy_to_work(unsigned char* pi, double* pi_to_work, int x, int y, int check)
{
    for (int i = 0; i < x * y * check; i++)
        pi_to_work[i] = pi[i];
}

void for_out(unsigned char* pi, double* pi_to_work, int x, int y, int check)
{
    for (int i = 0; i < x * y * check; i++)
        pi[i] = (unsigned char)(round(pi_to_work[i]));
}

int main(int argc, char* argv[])
{
    char s[100];
    int i, x, y, z, ch;
    FILE* fi = fopen(argv[1], "rb");
    if (fi == NULL)
    {
        cerr << "INPUT ERROR\n";
        return 1;
    }
    FILE* fo = fopen(argv[2], "wb");
    if (fo == NULL)
    {
        cerr << "OUTPUT ERROR\n";
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
    double* pi_to_work = new double[x * y * check];
    double* cpypi = new double[x * y * check];
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
    for (int i = 0; i < x * y * check; i++)
    {
        cpypi[i] = 0;
    }
    if (fread(pi, sizeof(unsigned char), x * y * check, fi) < x * y * check)
    {
        cerr << "Not enough bytes";
        fclose(fi);
        return 1;
    }
    copy_to_work(pi, pi_to_work, x, y, check);
    bool gradient = atoi(argv[3]);
    int Bits, TypeOfDithering;
    TypeOfDithering = atoi(argv[4]);
    if ((TypeOfDithering < 0) || (TypeOfDithering > 7))
    {
        cerr << "Wrong command. Go away";
        return 1;
    }
    Bits = atoi(argv[5]);
    double gamma;
    if (argc < 7)
        gamma = 0;
    else
        gamma = stof(argv[6]);
    if (gradient)
        drawgradient(pi_to_work, x, y, gamma);
    switch (TypeOfDithering)
    {
    case 0: WithoutDithering(pi_to_work, x, y, Bits, gamma);
        break;
    case 1: OrderedDithering(pi_to_work, x, y, Bits, gamma);
        break;
    case 2: RandomDithering(pi_to_work, x, y, Bits, gamma);
        break;
    case 3: Floyd_Steinberg(pi_to_work, cpypi, x, y, Bits, gamma);
        break;
    case 4: JarviceDithering(pi_to_work, cpypi, x, y, Bits, gamma);
        break;
    case 5: Siera(pi_to_work, cpypi, x, y, Bits, gamma);
        break;
    case 6: Atkinson(pi_to_work, cpypi, x, y, Bits, gamma);
        break;
    case 7: Halftone(pi_to_work, x, y, Bits, gamma);
        break;
    default: break;
    }
    fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
    for_out(pi, pi_to_work, x, y, check);
    fwrite(pi, sizeof(unsigned char), x * y * check, fo);
    delete[] pi;
    delete[] cpypi;
    delete[] pi_to_work;
    fclose(fo);
    return 0;
}

