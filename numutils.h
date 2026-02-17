#ifndef CA_NUMUTILS_H
#define CA_NUMUTILS_H

//-------------------------------------------------------------------------------------------------
// Zaleznosci (naglowki uzyte w tym module):
//
// C++ / STL
//   <cmath>  -> fabs
//

#include <cmath>




namespace cans
{


//-------------------------------------------------------------------------------------------------
// Przyciecie liczby calkowitej do podanego zakresu
//
inline int ClampInt(int i, int lo, int hi)
{
    // Jesli podana liczba wykracza poza zakres, zwrocenie wartosci granicznej (dol/gora), ...
    if (i < lo) return lo;
    if (i > hi) return hi;
    // ... w przeciwnym razie zwrocenie podanej wartosci
    return i;
}


//-------------------------------------------------------------------------------------------------
// Przyciecie liczby rzeczywistej do podanego zakresu
//
inline double ClampDbl(double i, double lo, double hi)
{
    // Jesli podana liczba wykracza poza zakres, zwrocenie wartosci granicznej (dol/gora), ...
    if (i < lo) return lo;
    if (i > hi) return hi;
    // ... w przeciwnym razie zwrocenie podanej wartosci
    return i;
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy dwie wartosci sa rowne z tolerancja.
//
inline bool AlmostEqual(double a, double b, double epsilon = 1E-9)
{
    return fabs(a - b) <= epsilon;
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy podany znak ASCII to plus/minus [+-]
//
inline bool IsDigitSign(char ch)
{
    return (ch == '+' || ch == '-');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie czy podany znak ASCII to marker wykladnika [Ee]
//
inline bool IsExponentMarker(char ch)
{
    return (ch == 'E' || ch == 'e');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy podany znak ASCII reprezentuje cyfre rzymska [CDILMVX].
//
inline bool IsRomanDigit(char ch)
{
    switch (ch)
    {
        case 'I':   //    1
        case 'V':   //    5
        case 'X':   //   10
        case 'L':   //   50
        case 'C':   //  100
        case 'D':   //  500
        case 'M':   // 1000
            return true;

        default:
            return false;
    }
}


typedef size_t cardinal;


} // namespace cans


#endif // CA_NUMUTILS_H
