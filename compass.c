/** W celu uzycia kompasu wystarczy dolaczyc plik naglowkowy compass.h
	Zainicjalizowac kompas wykorzystujac funkcje  eCompass_init()
	Teraz wystarczy uzyc funkcji eCompass() ktora zwraca kat odchylenia od polnocy
*/


/** \file compass.h
	\brief Biblioteka zawierajaca definicje funkcji
*/
#include "compass.h"

/** \file math.h
	\brief Biblioteka zawierajaca funkcje matematyczne
*/
#include "math.h"


/** \def DegToRad 0.01745329251994F
	\brief Stala do konwersji stopni na radiany
*/
#define DegToRad 0.01745329251994F

/** \def  RadToDeg 57.2957795130823F
	\brief Stala do konwersji radianow na stopnie
*/
#define RadToDeg 57.2957795130823F	

/** \var float Psi, The, Phi
	\brief Zmienne odchylenia, pochylenia oraz przechylenia
*/
float Psi, The, Phi; 

/** \var float Vx, Vy, Vz
	\brief Zmienne wspolczynniki kalibracji
*/
float Vx, Vy, Vz; /* wspólczynniki kalibracji */

/** \fn void eCompass_init(void)
	\brief Funkcja inicjalizujaca kompas
*/
void eCompass_init(void)
{
	mag_init();
	accel_init();
}

/** \fn int eCompass (void)
	\brief Funkcja zwracajaca kat odchylenia od polnocy w zakresie od 0-359 stopni - WYMAGA KALIBRACJI
	KALIBRACJA krok po kroku
	0. KAZDY POMIAR NALEZY WYKONAC GDY PLYTKA JEST POLOZONA PLASKO NA BIURKU.
	1. Nalezy przerobic funkcje tak by zwracala wartosc iBx
	   Wartosc iBx jest ujemna i powinna byc maksymalna gdy plytka obrocona jest na polnoc, 
	   a minimalna gdy obrocona jest na poludnie. 
	2. Nalezy znalezc wartosc max oraz min obracajac plytka o 360 stopni 
	   Uwaga! Wartosci max i min moga byc odczytane +/- 10
	3. Nalezy policzyc wartosc Vx ze wzoru Vx=0.5*(iBx(max)+iBx(min))
	4. Zastapic wartosc Vx przez obliczona wartosc 
	
	5. 	!! POWTORZYC POMIARY DLA iBy oraz iBz !!
	
	Uwaga! Wartosc iBy powinna byc dodatnia oraz maksymalna gdy plytka wskazuje wschod a najmniejsza gdy wskazuje zachod
		   Wartosc iBz powinna byc dodatnia oraz praktycznie nie zmieniac
	6. Po znalezieniu wartosci max oraz min na wszystkich osiach pamietac aby odkomentowac linijke iBy=-iBy;
	7. Pamietac by zmienic funkcje by zwracala wartosc Psi
	8. Teraz kompas powinien byc poprawnie skalibrowany
*/
int eCompass (void)
{
	
	float sinAngle, cosAngle; /* sinus, cosinus */
	//float fBfx, fBfy, fBfz; /* skalibrowana wartosc pola w uT po kompensacji */
	
	int iBx =0, iBy =0, iBz =0, iGx=0, iGy =0, iGz = 0;
	float fBx =0, fBy =0, fBz =0, fGx = 0, fGy =0, fGz = 0;
	float Bfx = 0, Bfy = 0; //Bfz = 0; 
	
	  iGx = iAcclReadX();
      iGy = iAcclReadY();
      iGz = iAcclReadZ();
	
	  iBz = iMagReadZ();
      iBy = iMagReadY();
      iBx = iMagReadX();
	
	iBy=-iBy; // !! Odkomentowac po zczytaniu wartosci iBy 
	
	fBx = (float) iBx;
	fBy = (float) iBy;
	fBz = (float) iBz;

	fGx = (float) iGx;
	fGy = (float) iGy;
	fGz = (float) iGz;
	
	Vx = -1235; // Vx = 0.5*(iBx(max)+iBx(min))
	Vy = 345; // Vy = 0.5*(iBy(max)+iBy(min))
	Vz = 3550; // Vz = 0.5*(iBz(max)+iBz(min))
	
	/* odejmij wplyw magnetykow ciezkich (rownanie 9) */
	fBx -= Vx;
	fBy += Vy; 
	fBz -= Vz;
 
	/* oblicz kat przechylenia Phi (-180, 180) i sin, cos */
	Phi = atan2(fGy, fGz) * RadToDeg; /* Equation 2 */
	sinAngle = sin(Phi * DegToRad); /* sin(Phi) */
	cosAngle = cos(Phi * DegToRad); /* cos(Phi) */
 
	/* obroc wynik o kat przechylenia Phi */
	Bfy = fBy * cosAngle - fBz * sinAngle; /* skladowa y wzoru 5 */
	fBz = fBy * sinAngle + fBz * cosAngle; /* Bz=(By-Vy).sin(Phi)+(Bz-Vz).cos(Phi) */
	fGz = fGy * sinAngle + fGz * cosAngle; /* Gz=Gy.sin(Phi)+Gz.cos(Phi) */
 
	/* oblicz kat pochylenia Theta (-90, 90 i sin, cos*/
	The = atan(-fGx / fGz) * RadToDeg; /* wzor 3 */
	sinAngle = sin(The * DegToRad); /* sin(Theta) */
	cosAngle = cos(The * DegToRad); /* cos(Theta) */
 
	/* obroc wynik o kat pochylenia Theta */
	Bfx = fBx * cosAngle + fBz * sinAngle; /* skladowa x wzoru 5  */
	//Bfz = -fBx * sinAngle + fBz * cosAngle; /* skladowa z wzoru 5  */
 
	/* oblicz odchylenie - kierunek pokazywany przez kompas psi (-180, 180) */
	Psi = atan2(-Bfy, Bfx) * RadToDeg; /* wzor 7 */
 
	if(Psi<0){
		Psi += 360;

	}
	return Psi; // zwraca Psi
}
