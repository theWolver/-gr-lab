#include <iostream>
#include <cmath>
#include <cstring>
#include<stdio.h>
#include <algorithm>
// #include "h2lab.h"

using namespace std;



double gamma_correction_fr_linear(double pi, double gamma)
{
	return round(255.0 * pow(pi / 255.0, 1.0 / gamma));
}

double alpha_blending(double old_p, double new_color, double new_bright)
{
//	cout << "new_p=" << new_bright << " " << new_color << " ans=";
//	cout << (old_p * (1.0 - new_bright) + new_bright * new_color) << endl;
	return old_p * (1.0 - new_bright) + new_bright * new_color;
}

double gamma_correction_to_linear(double pi, double gamma)
{
	return round(255.0 * pow(pi / 255.0, gamma));
}



double from_sRGB_to_RGB(double pi)
{
	if (pi / 255.0 <= 0.0031308)
		return min(round((12.92 * pi / 255.0) * 255.0), 255.0);
	else
		return min(round((1.055 * pow(pi / 255.0, (1.0 / 2.4)) - 0.055) * 255.0), 255.0);
}

double sRGB_correction(double pi)
{
	if (pi / 255.0 <= 0.04045)
		return min((round(pi / 255.0 / 12.92) * 255.0), 255.0);
	else
		return min(round(pow(((200.0 * pi / 255.0 + 11) / 211.0), 12.0 / 5.0) * 255.0), 255.0);
}

void draw_v(unsigned char* pi, int x, int y, int X_start, int Y_start, int X_finish, int Y_finish, double thickness, int brightness, int color, int check, double gamma)
{
	for (int i = Y_start; i < Y_finish; i++)
	{
		double pix, new_p;
		if (gamma == 0)
			pi[i * check * x + X_start * check] = sRGB_correction(pi[i * check * x + X_start * check]);
		else
			pix = gamma_correction_to_linear(pi[i * check * x + X_start * check], gamma);
		pi[i * check * x + X_start * check] = brightness;
		if (gamma == 0)
			pi[i * check * x + X_start * check] = from_sRGB_to_RGB(pi[i * check * x + X_start * check]);
		else
			pi[i * check * x + X_start * check] = gamma_correction_fr_linear(pi[i * check * x + X_start * check], gamma);
	}
}

void draw_h(unsigned char* pi, int x, int y, int X_start, int Y_start, int X_finish, int Y_finish, double thickness, int brightness, int color, int check, double gamma)
{
	for (int i = X_start; i < X_finish; i++)
	{
//		cout << brightness << " ";
		if (gamma == 0)
			pi[Y_start * check * x + i * check] = sRGB_correction(pi[Y_start * check * x + i * check]);
		else
			pi[Y_start * check * x + i * check] = gamma_correction_to_linear(pi[Y_start * check * x + i * check], gamma);
		pi[Y_start * check * x + i * check] = brightness;
		if (gamma == 0)
			pi[Y_start * check * x + i * check] = from_sRGB_to_RGB(pi[Y_start * check * x + i * check]);
		else
			pi[Y_start * check * x + i * check] = gamma_correction_fr_linear(pi[Y_start * check * x + i * check], gamma);
	}
}

void draw_line_ends(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, int brightness_start, int brightness_finish, int color, int check, double gamma)
{
	int Y_start_int = floor(Y_start), Y_finish_int = floor(Y_finish);
	int X_start_int = int(X_start), X_finish_int = int(X_finish);
	pi[x * check * Y_start_int + X_start_int * check] = 255.0 * ((pi[x * check * Y_start_int + X_start_int * check] / 255.0) + (brightness_start / 255.0 - pi[x * check * Y_start_int + X_start_int * check] / 255.0) * (1 - color / 255.0));
	pi[x * check * Y_finish_int + X_finish_int * check] = 255.0 * ((pi[x * check * Y_finish_int + X_finish_int * check] / 255.0) + (brightness_finish / 255.0 - pi[x * check * Y_finish_int + X_finish_int * check] / 255.0) * (1 - color / 255.0));
}



void draw_vertical_line_cop(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double dist, new_p, r_p, cop_br, pix, old_p;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	for (int i = 0; i < x * y * check; i++)
	{
//		if ((i / (x * check) >= Y_start)&&(i / (x * check) <= Y_finish) && ((i % x) >= X_start - thickness * 1.0) && ((i % x) <= X_start + thickness * 1.0))
		if ((i / (x * check) >= Y_start)&&(i / (x * check) <= Y_finish))
		{
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			dist = abs((i % x) * 1.0 + 0.5 - 1.0 * X_start);
//			cout << dist << " " << i % x << " " << i / x << endl;
			if (dist <= (thickness * 1.0 - 1.0) / 2.0)
				pi[i] = brightness;
			else
			if (dist <= (thickness * 1.0 - 1.0) / 2.0 + 1)
			{
//				cout << 1.0 - dist + (thickness - 1) / 2.0;
				new_p = (1.0 - dist + (thickness - 1) / 2.0);
				old_p = pix;
				r_p = new_p;
				r_p = alpha_blending(old_p, cop_br, new_p);
				if (gamma == 0)
					pi[i] = from_sRGB_to_RGB(r_p);
				else
					pi[i] = gamma_correction_fr_linear(r_p, gamma);
			}
		}
	}
}


void draw_horisontal_line_cop(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double dist, new_p, r_p, cop_br, pix, old_p;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	for (int i = 0; i < x * y * check; i++)
	{
		if ((i % (x * check) >= X_start)&&(i % (x * check) <= X_finish))
		{
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			dist = abs((i / x) * 1.0 + 0.5 - Y_start);
			if (dist <= (thickness * 1.0 - 1.0) / 2.0)
				pi[i] = brightness;
			else
			if (dist <= (thickness * 1.0 - 1.0) / 2.0 + 1)
			{
				new_p = (1.0 - dist + (thickness - 1) / 2.0);
				old_p = pix;
				r_p = new_p;
				r_p = alpha_blending(old_p, cop_br, new_p);
				if (gamma == 0)
					pi[i] = from_sRGB_to_RGB(r_p);
				else
					pi[i] = gamma_correction_fr_linear(r_p, gamma);
			}
		}
	}
}











void draw_vertical_line(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double dist, new_p, r_p, cop_br, pix, old_p;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	if (thickness > 1)
	{
		draw_horisontal_line_cop(pi, x, y, floor(X_start) - thickness / 2.0, ceil(Y_start), floor(X_finish) + thickness / 2.0, floor(Y_start), 1.0, brightness, check, gamma);
		draw_horisontal_line_cop(pi, x, y, floor(X_start) - thickness / 2.0, ceil(Y_finish), floor(X_finish) + thickness / 2.0, floor(Y_finish), 1.0, brightness, check, gamma);
	}
	for (int i = 0; i < x * y * check; i++)
	{
//		if ((i / (x * check) >= Y_start)&&(i / (x * check) <= Y_finish) && ((i % x) >= X_start - thickness * 1.0) && ((i % x) <= X_start + thickness * 1.0))
		if ((i / (x * check) >= Y_start)&&(i / (x * check) <= Y_finish))
		{
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			dist = abs((i % x) * 1.0 + 0.5 - 1.0 * X_start);
//			cout << dist << " " << i % x << " " << i / x << endl;
			if (dist <= (thickness * 1.0 - 1.0) / 2.0)
				pi[i] = brightness;
			else
			if (dist <= (thickness * 1.0 - 1.0) / 2.0 + 1)
			{
//				cout << 1.0 - dist + (thickness - 1) / 2.0;
				new_p = (1.0 - dist + (thickness - 1) / 2.0);
				old_p = pix;
				r_p = new_p;
				r_p = alpha_blending(old_p, cop_br, new_p);
				if (gamma == 0)
					pi[i] = from_sRGB_to_RGB(r_p);
				else
					pi[i] = gamma_correction_fr_linear(r_p, gamma);
			}
		}
	}
}


void draw_horisontal_line(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double dist, new_p, r_p, cop_br, pix, old_p;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	if (thickness > 1)
	{
		draw_vertical_line_cop(pi, x, y, ceil(X_start), floor(Y_start - thickness / 2.0), ceil(X_start), Y_finish + thickness / 2.0, 1.0, brightness, check, gamma);
		draw_vertical_line_cop(pi, x, y, ceil(X_finish), floor(Y_start - thickness / 2.0), ceil(X_finish), Y_finish + thickness / 2.0, 1.0, brightness, check, gamma);
	}
	for (int i = 0; i < x * y * check; i++)
	{
		if ((i % (x * check) >= X_start)&&(i % (x * check) <= X_finish))
		{
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			dist = abs((i / x) * 1.0 + 0.5 - Y_start);
			if (dist <= (thickness * 1.0 - 1.0) / 2.0)
				pi[i] = brightness;
			else
			if (dist <= (thickness * 1.0 - 1.0) / 2.0 + 1)
			{
				new_p = (1.0 - dist + (thickness - 1) / 2.0);
				old_p = pix;
				r_p = new_p;
				r_p = alpha_blending(old_p, cop_br, new_p);
				if (gamma == 0)
					pi[i] = from_sRGB_to_RGB(r_p);
				else
					pi[i] = gamma_correction_fr_linear(r_p, gamma);
			}
		}
	}
}

void less_than_45_cop(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
	double k = (Y_finish - Y_start) / (X_finish - X_start);
	double b = Y_start - X_start * k;
	double k_perp = -1 / k;
	double b_perp_start = Y_start - k_perp * X_start;
	double b_perp_finish = Y_finish - k_perp * X_finish;
	A_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	A_y = k_perp * A_x + b_perp_start;
	B_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	B_y = k_perp * B_x + b_perp_start;
	C_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	C_y = k_perp * C_x + b_perp_finish;
	D_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	D_y = k_perp * D_x + b_perp_finish;
//		cout << "A_x=" << A_x << " A_y=" << A_y << " B_x=" << B_x << " B_y=" << B_y << " C_x=" << C_x << " C_y=" << C_y << " D_x=" << D_x << " D_y=" << D_y << endl;
	double dist;
	double new_p, old_p, r_p, cop_br;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	for (int i = 0; i < x * y; i++)
	{
		dist = abs(k * ((i % x) * 1.0) + b * 1.0 - i * 1.0 / x * 1.0) / (sqrt(k * 1.0 * k + 1 * 1.0));
		if ((i % x >= A_x) && (i % x <= D_x) && (i / x >= C_y) && (i / x <= B_y) && (i % x >= floor(((floor(i / x)) - b_perp_start) * 1.0 / k_perp * 1.0)) && (i % x <= ceil((floor(i / x) - b_perp_finish) * 1.0 / k_perp * 1.0)))
		{
			double pix, new_p;
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			if (dist <= thickness / 2.0)
			{
				pi[i] = brightness;
			}
			else
				if (dist <= thickness / 2.0 + 1)
				{
					old_p = pix;
					new_p = (1.0 - dist + thickness / 2.0);
					r_p = new_p;
					r_p = alpha_blending(old_p, cop_br, new_p);
					if (gamma == 0)
						pi[i] = from_sRGB_to_RGB(r_p);
					else
						pi[i] = gamma_correction_fr_linear(r_p, gamma);
				}
		}
	}
}

void more_than_45_cop(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, double brightness, int check, double gamma)
{
	double A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
	double k = (Y_finish - Y_start) / (X_finish - X_start);
	double b = Y_start - X_start * k;
	double k_perp = -1 / k;
	double b_perp_start = Y_start - k_perp * X_start;
	double b_perp_finish = Y_finish - k_perp * X_finish;
	A_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	A_y = k_perp * A_x + b_perp_start;
	B_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	B_y = k_perp * B_x + b_perp_start;
	C_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	C_y = k_perp * C_x + b_perp_finish;
	D_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	D_y = k_perp * D_x + b_perp_finish;
	//	cout << "A_x=" << A_x << " A_y=" << A_y << " B_x=" << B_x << " B_y=" << B_y << " C_x=" << C_x << " C_y=" << C_y << " D_x=" << D_x << " D_y=" << D_y << endl;
	double dist;
	double new_p, old_p, r_p, cop_br;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	for (int i = 0; i < x * y; i++)
	{
		dist = abs(k * ((i % x)) + b * 1.0 - i * 1.0 / x * 1.0) / (sqrt(k * 1.0 * k + 1.0));
		if ((i % x >= B_x) && (i % x <= C_x) && (i / x >= A_y) && (i / x <= D_y) && (i % x >= floor(((floor(i / x) + 1) - b_perp_start) * 1.0 / k_perp * 1.0 )) && (i % x <= ceil((floor(i / x) - 1 - b_perp_finish) * 1.0 / k_perp)))
		{
			double pix, new_p;
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			if (dist <= thickness / 2.0)
			{
				pi[i] = brightness;
			}
			else
				if (dist < thickness / 2.0 + 1)
				{
					old_p = pix;
					
//					cout << i % x << " " << i / x << " ";
					new_p = (1.0 - dist + thickness / 2.0);
					r_p = alpha_blending(pi[i], cop_br, new_p);
					pi[i] = r_p;
//					cout << r_p;
//					cout << " x=" << i % x << " y=" << i / x << " newPixel="<< r_p << " new_p=" << new_p << " dist=" << dist << endl;
					if (gamma == 0) 
						pi[i] = from_sRGB_to_RGB(r_p);
					else
						pi[i] = gamma_correction_fr_linear(r_p, gamma);
				}
		}
	}
}




void more_than_45(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, double brightness, int check, double gamma)
{
	double A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
	double k = (Y_finish - Y_start) / (X_finish - X_start);
	double b = Y_start - X_start * k;
	double k_perp = -1 / k;
	double b_perp_start = Y_start - k_perp * X_start;
	double b_perp_finish = Y_finish - k_perp * X_finish;
	A_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	A_y = k_perp * A_x + b_perp_start;
	B_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	B_y = k_perp * B_x + b_perp_start;
	C_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	C_y = k_perp * C_x + b_perp_finish;
	D_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	D_y = k_perp * D_x + b_perp_finish;
//		cout << "A_x=" << A_x << " A_y=" << A_y << " B_x=" << B_x << " B_y=" << B_y << " C_x=" << C_x << " C_y=" << C_y << " D_x=" << D_x << " D_y=" << D_y << endl;
	double dist;
	double new_p, old_p, r_p, cop_br;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	if (thickness > 1)
	{
		less_than_45_cop(pi, x, y, B_x, B_y, A_x, A_y, 0, brightness, check, gamma);
		less_than_45_cop(pi, x, y, D_x, D_y, C_x, C_y, 0, brightness, check, gamma);
	}
	for (int i = 0; i < x * y; i++)
	{
		dist = abs(k * ((i % x)) + b * 1.0 - i * 1.0 / x * 1.0) / (sqrt(k * 1.0 * k + 1.0));
		if ((i % x > floor(B_x)) && (i % x < ceil(C_x)) && (i / x > floor(A_y)) && (i / x < ceil(D_y)) && (i % x > floor(((floor(i / x) + 1) - b_perp_start) * 1.0 / k_perp * 1.0 )) && (i % x < ceil((floor(i / x) - 1 - b_perp_finish) * 1.0 / k_perp)))
		{
			double pix, new_p; 
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			if (dist <= thickness / 2.0)
			{
				pi[i] = brightness;
			}
			else
				if (dist < thickness / 2.0 + 1)
				{
					old_p = pix;
					
//					cout << i % x << " " << i / x << " ";
					new_p = (1.0 - dist + thickness / 2.0);
					r_p = alpha_blending(pi[i], cop_br, new_p);
					pi[i] = r_p;
//					cout << r_p;
//					cout << " x=" << i % x << " y=" << i / x << " newPixel="<< r_p << " new_p=" << new_p << " dist=" << dist << endl;
					if (gamma == 0) 
						pi[i] = from_sRGB_to_RGB(r_p);
					else
						pi[i] = gamma_correction_fr_linear(r_p, gamma);
				}
		}
	}
}

void less_than_45(unsigned char* pi, int x, int y, double X_start, double Y_start, double X_finish, double Y_finish, double thickness, int brightness, int check, double gamma)
{
	double A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
	double k = (Y_finish - Y_start) / (X_finish - X_start);
	double b = Y_start - X_start * k;
	double k_perp = -1 / k;
	double b_perp_start = Y_start - k_perp * X_start;
	double b_perp_finish = Y_finish - k_perp * X_finish;
	A_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	A_y = k_perp * A_x + b_perp_start;
	B_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_start - b) / (k - k_perp);
	B_y = k_perp * B_x + b_perp_start;
	C_x = (+(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	C_y = k_perp * C_x + b_perp_finish;
	D_x = (-(thickness / 2) * sqrt(k * k + 1) + b_perp_finish - b) / (k - k_perp);
	D_y = k_perp * D_x + b_perp_finish;
//		cout << "A_x=" << A_x << " A_y=" << A_y << " B_x=" << B_x << " B_y=" << B_y << " C_x=" << C_x << " C_y=" << C_y << " D_x=" << D_x << " D_y=" << D_y << endl;
	double dist;
	double new_p, old_p, r_p, cop_br;
	if (gamma == 0)
		cop_br = sRGB_correction(brightness);
	else
		cop_br = gamma_correction_to_linear(brightness, gamma);
	if (thickness > 1)
	{
		more_than_45_cop(pi, x, y, A_x, A_y, B_x, B_y, 0, brightness, check, gamma);
		more_than_45_cop(pi, x, y, C_x, C_y, D_x, D_y, 0, brightness, check, gamma);
	}
	for (int i = 0; i < x * y; i++)
	{
		dist = abs(k * ((i % x) * 1.0) + b * 1.0 - i / x * 1.0) / (sqrt(k * 1.0 * k + 1 * 1.0));
		if ((i % x >= floor(A_x)) && (i % x <= ceil(D_x)) && (i / x >= floor(C_y)) && (i % x > floor(((floor(i / x) * 1.0) - b_perp_start) * 1.0 / k_perp * 1.0)) &&  (i % x < ceil(( (i / x) * 1.0 - b_perp_finish) * 1.0 / k_perp * 1.0)))
		{
			double pix, new_p;
			if (gamma == 0)
				pix = sRGB_correction(pi[i]);
			else
				pix = gamma_correction_to_linear(pi[i], gamma);
			if (dist <= thickness / 2.0)
				pi[i] = brightness;
			else
				if (dist <= thickness / 2.0 + 1)
				{
					old_p = pix;
					new_p = (1.0 - dist + thickness / 2.0);
					r_p = new_p;
					r_p = alpha_blending(old_p, cop_br, new_p);
					if (gamma == 0)
						pi[i] = from_sRGB_to_RGB(r_p);
					else
						pi[i] = gamma_correction_fr_linear(r_p, gamma);
				}
		}
	} 
}

int main(int argc, char* argv[])
{
	char s[100];
	int i, x, y, z, ch;
	int brightness;
	double thickness, X_start, Y_start, X_finish, Y_finish, gamma;
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
	brightness = atoi(argv[3]);
	thickness = strtod(argv[4], NULL);
	X_start = strtod(argv[5], NULL);
	Y_start = strtod(argv[6], NULL);
	X_finish = strtod(argv[7], NULL);
	Y_finish = strtod(argv[8], NULL);
	if (argc > 10)
		gamma = strtod(argv[9], NULL);
	else
		gamma = 0;
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
	if (X_start == X_finish)
	{
		if (Y_start > Y_finish)
			swap(Y_start, Y_finish);
		draw_vertical_line(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
	}
	else
	{
		if (Y_start == Y_finish)
		{
			if (X_start > X_finish)
				swap(X_start, X_finish);
			draw_horisontal_line(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
		}
	}
	if ((X_start != X_finish) && (Y_start != Y_finish))
	{
		thickness = thickness - 1.0;
		if (thickness < 0)
			brightness = (thickness + 1) * brightness;
		if (X_start > X_finish)
		{
			swap(X_start, X_finish);
			swap(Y_start, Y_finish);
		}
		if (Y_start > Y_finish)
		{
			if (thickness > 0)
				less_than_45(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
			else
				less_than_45_cop(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
//			cout << "HERE";
		}
		else
		{
			if (thickness > 0)
				more_than_45(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
			else
				more_than_45_cop(pi, x, y, X_start, Y_start, X_finish, Y_finish, thickness, brightness, check, gamma);
//			cout << "TERE";
		}
	}
	fprintf(fo, "%s\n%i %i\n%i\n", s, x, y, z);
	fwrite(pi, sizeof(unsigned char), x * y * check, fo);
	delete[] pi;
	delete[] cpypi;
	fclose(fo);
	return 0;
}

