/*
Copyrights: DWP 2002/2003/2004
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include "resource.h"
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "c4dmain.h"
using namespace NTL;
using namespace std;

void sn(HWND hDlg);
// Definitions
uchar characters[16];
uchar valcorrect[] = {0xC,0xE,0xB,0xA,0x12,0xF,0x26,0x10,0xD,0x11,0xC,0xE,0xB,0xA,0x12,0xF};
mat_ZZ_p matrix1,matrix2,matrix3,matrix4;
vec_ZZ_p digits,letters,mod2,mod1,rsainput,crcvector;
ushort  crc;
int structcounter,nr;
snstruct snnumber[16];

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT message, WORD wParam, LONG lParam)
{
    switch (message) {
            case WM_INITDIALOG:
                    srand((unsigned int)time(NULL));
                    nr = rand()%9999;
                    SetDlgItemInt(hDlg,1001,nr,false);
                    return TRUE;
            case WM_COMMAND:
                    switch (wParam) {
                    case IDOK:
                            EndDialog( hDlg, TRUE );
                            break;
                    case IDCANCEL:
                            EndDialog( hDlg, TRUE );
                            break;
                    case IDC_BUTTON1:

                            sn(hDlg);
                            break;
                    }
            break;
    }

    return FALSE;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
        DialogBox( hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DLGPROC) MainDlgProc );
        return 0;
}
ushort crc16(ushort seed, uchar* buf, int size)
{
	ushort      crc = seed;
	int         i;

	do
	{
		crc ^= *buf++ << 8;
		for (i=8; i; --i)
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc <<= 1;
		}
	} while (--size);
	return (crc);
}

void initValues17() //Init values
{
	ZZ p;
	p = 17;
	ZZ_p::init(p);
	digits.SetLength(11);
	mod2.SetLength(11);
	matrix1.SetDims(11,11);
}
void initValues19() // Init values
{
	ZZ p;
	p = 19;
	ZZ_p::init(p);
	letters.SetLength(16);
	rsainput.SetLength(16);
	mod1.SetLength(15);
	matrix2.SetDims(15,15);
	matrix3.SetDims(16,16);
	matrix4.SetDims(16,16);
	uchar helper[11];
	for(int i =0;i<11;i++)
	{
			int help;
			conv(help,rep(digits[i]));
			helper[i] = (uchar) help;
	}
	crc = 0x000B;
	for (int i=0; i<sizeof(helper); i++)
	{
					crc = crc16(crc, (uchar*)helper+i, 1);
	}
}
ZZ_p initMatrix1() // Matrix for Mod2
{
    uchar *mult = NULL;
    for(int i = 0;i<11;i++)
	{
        mult = multipliers4[i];
        for(int j = 0;j<11;j++)
		{
                matrix1[i][j] = mult[j];
		}
    }
    matrix1 = transpose(matrix1);
    ZZ_p dim = determinant(matrix1);
    return dim;
}
ZZ_p initMatrix2() // Matrix for mod1
{
    uchar *mult = NULL;
    for(int i = 0;i<15;i++)
	{
        mult = multipliers2[i];
        for(int j = 0;j<15;j++)
		{
                matrix2[i][j] = mult[j];
		}
    }
    matrix2 = transpose(matrix2);
    ZZ_p dim = determinant(matrix2);
    return dim;
}
ZZ_p initMatrix3() // Matrix for Digits
{
    uchar *mult = NULL;
    for(int i = 0;i<16;i++)
	{
        mult = multipliers1[i];
        for(int j = 0;j<16;j++)
		{
                matrix3[i][j] = mult[j];
		}
    }
    matrix3 = transpose(matrix3);
    ZZ_p dim = determinant(matrix3);

    return dim;
}
ZZ_p initMatrix4() // Matrix for rsainput/modules
{
    uchar *mult = NULL;
    for(int i = 0;i<16;i++)
	{
        mult = modmult[i];
        for(int j = 0;j<16;j++)
		{
                matrix4[i][j] = mult[j];
		}
    }
    matrix4 = transpose(matrix4);
    ZZ_p dim = determinant(matrix4);
    return dim;
}
void crcupdate()
{
    uchar mod_2[11];
    for(int i =0;i<sizeof(mod_2);i++)
    {
            int help;
            conv(help,rep(mod2[i]));
            mod_2[i] = (uchar) help;
            crc = crc16(crc, mod_2+i, 1);
    }
}
void initCRCvector()
{
   crcvector.SetLength(4);
   int k = 3;
   for(int i = 0;i<4;i++)
   {
   crcvector[k] = (crc >> 4*i) & 0x0F;
   k--;
   }
}
void solveLGS1(ZZ_p dim1)
{
   solve(dim1,mod2,matrix1,digits);
}
void solveLGS2(ZZ_p dim2)
{
   solve(dim2,mod1,matrix2,mod2);
}
void solveLGS3(ZZ_p dim3)
{
   solve(dim3,letters,matrix3,mod1);
}
void solveLGS4(ZZ_p dim4)
{
   solve(dim4,letters,matrix4,rsainput);
}
void displayletters()
{
  int helper;
  int pointer = 0;
  char *cinema[] =
  {
    "Cinema 4D 8.0 XL (Win):",
    "Net Renderer (Win):",
    "Cinema 4D 8.0 XL (Mac):",
    "Net Renderer (Mac):",
    "BodyPaint3D R2 Module:",
    "Advanced Render:",
    "Thinking Particles:",
    "Pyrocluster:",
    "Mocca:",
    "Dynamics:",
    "BodyPaint3D R2 (Win):",
    "BodyPaint3D R2 (Mac):",
    "Cinema 4D 8.5 Update:",
    "Sketch and Toon:"
  };
  char *snall = new char[32];
  char c = '-';
  char sndigits[13];
  char sn[] = "0000000000000000000";
  char alphabet[] = "TBCDVFGHWJKLMNZPXRS";
  for(int i = 0;i <11;i++)
  {
     conv(helper,rep(digits[i]));
     sprintf(sndigits+i,"%01d",helper);
  }
  int sn0 = (int) sndigits[0]-0x30;
  int sn1 = (int) sndigits[1]-0x30;
  int sn2 = (int) sndigits[2]-0x30;
  int sn4 = (int) sndigits[4]-0x30;

  if (sn0 ==1 && sn1 == 0 && sn4 ==1) pointer = 0;
  else if(sn0 == 3 && sn1 == 0 && sn4 == 1) pointer = 1;
  else if(sn0 == 1 && sn1 == 0 && sn4 == 2) pointer = 2;
  else if(sn0 == 3 && sn1 == 0 && sn4 == 2) pointer = 3;
  else if(sn0 == 1 && sn1 == 1 && sn4 == 1) pointer = 10;
  else if(sn0 == 1 && sn1 == 1 && sn4 == 2) pointer = 11;
  else if(sn0 == 3 && sn1 == 8 && sn2 == 5) pointer = 12;
  else if(sn0 == 3 && sn1 == 7) pointer = 13;
  else pointer = sn1+3;

  for(int i = 0; i <16;i++)
  {
     conv(helper,rep(letters[i]));
     characters[i] = alphabet[helper];
  }
  for(int i = 0;i <19;i++)
  {
     int k = 0;
     if(i == 4 || i == 9 || i == 14)
     {sn[i] = c;}
     else if(i < 4) sn[i] = characters[i];
     else if(i >4 && i < 9) sn[i] = characters[i-1];
     else if(i >9 && i < 14) sn[i] = characters[i-2];
     else sn[i] = characters[i-3];
  }
  sprintf(snall,"%s%c%s",sndigits,c,sn);
  strcpy(snnumber[structcounter].Name,cinema[pointer]);
  strcpy(snnumber[structcounter].Key,snall);
  structcounter++;
  delete [] snall;
}
void c4dcalc(char serial[])
{
   initValues17();
   for (int i=0; i<11; i++)
   digits[i] = serial[i] - 0x30;
   ZZ_p dim = initMatrix1();
   solveLGS1(dim);
   initValues19();
   dim = initMatrix3();
   crcupdate();
   initCRCvector();
   dim = initMatrix2();
   mod2.SetLength(15);
   mod2[11] = crcvector[0];
   mod2[12] = crcvector[1];
   mod2[13] = crcvector[2];
   mod2[14] = crcvector[3];
   solveLGS2(dim);
   mod1.SetLength(16);
   mod1[15] = 9;
   solveLGS3(dim);
   displayletters();
}
void modulescalc(char serial[])
{
   int privatexp =  0x032BC117;
   int privatexp2 = 0x0396FE59;
   int publicexp =  0x000EE7DB;
   int publicexp2 = 0x0013D509;
   int modulo =  0x03D153ED;
   int modulo2 = 0x03D5C6EF;
   int firstnumber = 0;   // 1stnumber + 27 + 2ndnumber + 37
   int secondnumber = 0; // 2ndnumber + 37
   int output = 0;
   char part1[6];
   char part2[5];
   char outputpart1[9];
   char outputpart2[9];
   memset(part1,0,6);
   memset(part2,0,5);
   memset(outputpart1,0,9);
   memset(outputpart2,0,9);
   for(int i =0;i<11;i++) digits[i] = serial[i]-0x30;
   for(int i =0;i<6;i++) part1[i] = serial[i];
   for(int i =6;i<11;i++) part2[i-6] = serial[i];
   firstnumber = atoi(part1);
   secondnumber = atoi(part2);
   firstnumber += 27+secondnumber+37;
   secondnumber += 37;
   ZZ   mod,y;
   mod = modulo;
   ZZ_p::init(mod);
   ZZ_p encrypted,x;
   x = firstnumber;
   y = privatexp;
   encrypted = power(x,y);
   conv(output,rep(encrypted));
   sprintf(outputpart1,"%.8d",output);
   mod = modulo2;
   ZZ_p::init(mod);
   x = secondnumber;
   y = privatexp2;
   encrypted = power(x,y);
   output = 0;
   conv(output,rep(encrypted));
   sprintf(outputpart2,"%.8d",output);
   initValues19();
   ZZ_p dim = initMatrix4();
   for(int i =0;i<8;i++)
   rsainput[i] = 0x13+outputpart1[i]-valcorrect[i]-0x30;
   for(int i =8;i<16;i++)
   rsainput[i] = 0x13+outputpart2[i-8]-valcorrect[i]-0x30;
   solveLGS4(dim);
   displayletters();
}
void c4d85calc(char serial[])
{
	// Bad attempt for a serial, trivial to solve

	unsigned int val1 = 0;
	unsigned int val2 = 0;
	sscanf(serial,"%6d%5d",&val1,&val2);
	unsigned int eax,ebx,ecx,edx;
	unsigned int xorarray[] =
	{
			0x243F6A88,0xA1A7ECFD,0x23DA828C,0x4F887DA0,0x4F887DA0,0x604975FE,0xDE16E06F,0x9D1D516C,
			0x9D1D516C,0x3B54CB25,0x9CB86970,0xB8BD4D9A,0xC8666484,0x669DDE3D,0x56F4C753,0x775ED69B,
			0x15FB3850,0xB432B209,0x15965054,0x526A2BC2,0x41444B68,0xCFD2AE37,0xCFD2AE37,0x7DB33EDA,
			0x8ED91F34,0x8E743738,0x8E743738,0x2CABB0F1,0xBA22324C,0x697F8C5F,0x48B0951B,0xE6E80ED4,
			0x07B70618,0x94C89F77,0x07521E1C,0xA58997D5,0x33001930,0x43C1118E,0xC18E7BFF,0x8094ECFC,
			0x8094ECFC,0xFDFD6F71,0x80300500,0xABDE0014,0xABDE0014,0xBC9EF872,0x3A6C62E3,0x5AD6722B,
			0xF972D3E0,0x97AA4D99,0xF90DEBE4,0x1512D00E,0x24BBE6F8,0xC2F360B1,0xB34A49C7,0x612ADA6A,
			0x7250BAC4,0x71EBD2C8,0x71EBD2C8,0xAEBFAE36,0x9D99CDDC,0x2C2830AB,0x2C2830AB,0xDA08C14E
	};
	char outputarray[] = "BZDFGHKLMNPRSTWX";
	eax = val2;
	edx = val1;
	int k = 0;
	for(int i = 0;i<32;i++)
	{
			ebx = eax << 4;
			ecx = eax >> 5;
			ebx ^=ecx;
			ebx +=eax;
			ebx ^=xorarray[k];
			edx+=ebx;
			k++;
			ebx = edx << 4;
			ecx = edx >> 5;
			ebx ^=ecx;
			ebx +=edx;
			ebx ^=xorarray[k];
			eax+=ebx;
			k++;
	}
	char output[17];
	memset(output,0,17);
	sprintf(output,"%.8X%.8X",edx,eax);
	for(int i = 0;i<16;i++)
	{
	if(output[i] > 0x40) output[i] -= 0x37;
	if(output[i] >= 0x30) output[i] -= 0x30;
	output[i] = outputarray[output[i]];
	}
	char serial85[20];
	memset(serial85,0,20);
	for(int i = 0;i <19;i++)
	{
		if(i == 4 || i == 9 || i == 14)
		{serial85[i] = '-';}
		else if(i < 4) serial85[i] = output[i];
		else if(i >4 && i < 9) serial85[i] = output[i-1];
		else if(i >9 && i < 14) serial85[i] = output[i-2];
		else serial85[i] = output[i-3];
	}
	char *snall = new char[32];
	sprintf(snall,"%s-%s",serial,serial85);
	strcpy(snnumber[structcounter].Name,"Cinema 4D 8.5 Update:");
	strcpy(snnumber[structcounter].Key,snall);
	structcounter++;
	delete [] snall;
}
void sn(HWND hDlg)
{
   char output[1024];
   char helper[64];
   memset(helper,0,64);
   memset(output,0,1024);
   char serial[12]="000";
   char version = '8'; //Version
   char os[] ="01"; // OS (01 = windows, 02 = macos) // Not necessary
   char hardcoded[] = "00"; // Type / Count // Unknown ?
   structcounter = 0;
   int nr = GetDlgItemInt(hDlg,1001,FALSE,FALSE);
   sprintf(serial,"%s%s%s%.4i",serial,os,hardcoded,nr%10000);
   // Here was the big and famous Buffer Overflow, PDX found, 
   // I was too lazy to limit the number to 4 digits, since I
   // actually never thought anyone would try to enter more than
   // 4 digits in a dialogbox for 4 digits. Here it did crash and
   // here it did generate only serial numbers which are multiples of two
   // Great reasons for proper...
   // Now I did a %10000 to get it down to 4 digits...
   // And yes even though the Number at the end of the serial can be 5 digits , i still didn't fix that

   serial[0] = '1';
   serial[2] = version;
   memset(characters,0,16);

   for(int i = 1;i<3;i++)
   {
   serial[0] = '1';
   serial[1] = '0';
   serial[4] = (char) i+0x30;
   serial[5] = hardcoded[0];
   c4dcalc(serial); // C4d Mac/Win
   serial[1] = '1';
   serial[2] = '9';
   c4dcalc(serial); // BP2 Win/Mac
   serial[0] = '3';
   serial[1] = '0';
   serial[2] = '8';
   serial[5] = hardcoded[0];
   c4dcalc(serial); // NetRender
   }
   serial[1] = '1';
   serial[2] = '9';
   serial[3] = '0';
   serial[4] = '0';
   serial[5] = hardcoded[0];
   c4dcalc(serial);// BP2 Module

   serial[0] = '3';
   serial[1] = '8';
   serial[2] = '5';
   serial[3] = '0';
   serial[5] = '0';
   serial[6] = '0';
   c4d85calc(serial); // C4D 8.5

   serial[0] = '3';
   serial[1] = '1';
   serial[2] = '0';
   serial[3] = '0';
   serial[4] = '0';
   for(int i = 2;i <8;i++)
   {
   serial[1] = (char) i+0x30;
   modulescalc(serial); // Modules
   }
   for(int i = 0;i<structcounter;i++)
   {
           sprintf(helper,"%s\r\n%s\r\n",snnumber[i].Name,snnumber[i].Key);
           strcat(output,helper);
   }
   SetDlgItemText(hDlg,1015,output);
}