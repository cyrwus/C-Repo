#ifndef CA_STRCONVERTERS_H
#define CA_STRCONVERTERS_H

//-------------------------------------------------------------------------------------------------
// Zaleznosci (naglowki uzyte w tym module):
//
// C++ / STL
//   <cstdio>     -> snprintf()
//   <cstdlib>    -> strtol(), strtod()
//   <cerrno>     -> errno, ERANGE
//   <climits>    -> INT_MIN, INT_MAX
//   <cmath>      -> isfinite()
//   <string>     -> std::string
//
// Repository
//   "numutils.h" -> cardinal, ClampInt()
//   "strutils.h" -> IsAsciiUpperAlpha(), ReplaceCharInPlace()
//

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <cmath>
#include <string>

#include "numutils.h"
#include "strutils.h"




namespace cans
{
    using std::string;


///////////////////////////////////////////////////////////////////////////////////////////////////
// Dzial: Konwersje tekstu na wartosc
// Warstwa: Model / Utilities
//-------------------------------------------------------------------------------------------------
// Cel:
//   Elementarne funkcje do konwersji liczba na tekst / tekst na liczbe.
//   Zaklada sie wejscie tekstowe ze znakami 8-bit.
//

#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif


//-------------------------------------------------------------------------------------------------
// Konwersja liczby calkowitej na tekst.
//
inline string IntToStr(int value)
{
    const cardinal kSize = 24;
    // Alokacja bufora wynikowego
    char bufsz[kSize] = { 0 };

    // Konwersja podanej liczby na lancuch znakow
    snprintf(bufsz, kSize, "%d", value);
    // Wpisanie terminatora na krancu bufora (funkcja snprintf moze czasem nie domknac lancucha)
    bufsz[kSize -1] = '\0';

    // Zwrocenie tekstu wynikowego
    return string(bufsz);
}


//-------------------------------------------------------------------------------------------------
// Konwersja tekstu na liczbe calkowita.
// Uwaga: Funkcja niskopoziomowa - wykonuje bardzo ograniczona walidacje tekstu wejsciowego.
//        Oczekuje znakow: [+-]?[0-9] i nic poza tym.
//        Jesli ciag znakow jest niepoprawny, funkcja zwraca false i oddaje przez referencje
//        nieokreslona wartosc.
//
inline bool StrToInt(const string& input, int& out)
{
    // Jezeli brak tresci, zakonczenie negatywne
    if (input.empty()) return false;

    const char* b = input.c_str();
    char* e = NULL;
    errno = 0;
    // Konwersja tekstu na liczbe calkowita
    const long v = ::strtol(b, &e, 10);
    // Po konwersji:
    // - wskaznik poczatku i konca musza roznic sie 
    // - koncowy znak musi byc terminatorem
    // - funkcja strtol nie powinna zwrocic blednu w errno (np. over-/-underflow zakresu long)
    // Jezeli cos z ww. poszlo nie tak, zakonczenie negatywne
    if ((e == b) || (*e != '\0') || (errno == ERANGE)) return false;

    // Jezeli wynik konwersji nie miesci sie w docelowym typie int, zakonczenie negatywne
    if (v < (long)INT_MIN || (long)INT_MAX < v) return false;

    out = static_cast<int>(v);
    // Oddanie wyniku przez referencje i zakonczenie pozytywne
    return true;
}




//-------------------------------------------------------------------------------------------------
// Konwersja liczby rzeczywistej na tekst.
// Wynik w formacie round-trip z 17 cyframi znaczacymi, a wiec zasadniczo notacja ogolna,
// ale przy duzym wykladniku nastepuje samoczynna zmiana na notacje wykladnicza.
//
inline string DblToStr(double value)
{
    const cardinal kSize = 32;
    // Alokacja bufora wynikowego
    char bufsz[kSize] = { 0 };

    // Konwersja podanej liczby na lancuch znakow (format z 17 cyframi znaczacymi)
    snprintf(bufsz, kSize, "%.17g", value);
    // Wpisanie terminatora na krancu bufora (funkcja snprintf moze czasem nie domknac lancucha)
    bufsz[kSize -1] = '\0';

    string str(bufsz);
    // Gdyby ustawienia lokalne daly przecinek, podmiana na kropke
    ReplaceCharInPlace(str, ',', '.');

    // Zwrocenie tekstu wynikowego
    return str;
}


//-------------------------------------------------------------------------------------------------
// Konwersja liczby rzeczywistej na tekst.
// Wynik w formacie fixed z podana liczba miejsc dziesietnych.
//
inline string DblToStrFixed(double value, short decimals = 8)
{
    const cardinal kSize = 128;
    // Alokacja bufora wynikowego
    char bufsz[kSize] = { 0 };

    // Auto-korekta blednie podanej precyzji
    ClampInt(decimals, 0, 16);
    // Konwersja podanej liczby na lancuch znakow (format z zadana liczba miejsc dziesietnych)
    snprintf(bufsz, kSize, "%.*f", decimals, value);
    // Wpisanie terminatora na krancu bufora (funkcja snprintf moze czasem nie domknac lancucha)
    bufsz[kSize -1] = '\0';

    string str(bufsz);
    // Gdy ustawienia lokalne daly przecinek, podmiana na kropke
    ReplaceCharInPlace(str, ',', '.');

    // Zwrocenie tekstu wynikowego
    return str;
}


//-------------------------------------------------------------------------------------------------
// Helper: isFinite dla C++98
//
static bool IsFiniteDbl(double value)
{
#if defined(_MSC_VER)
    // MSVC (stare i nowe) ma _finite
    return _finite(value) != 0;

#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang zwykle maja isfinite jako makro/funkcje w cmath/math.h
    return std::isfinite(value);
#else
    // Fallback: wykrycie NaN i +-Inf w sposob uniwersalny
    // NaN: (v != v)
    if (value != value) return false;

    // INFO: Dla wielu platform dziala porownanie z ogromna wartoscia, ale to jest przyblizenie.
    //       Jesli platforma nie ma isfinite/_finite, to zwykle i tak system to przyjmie.
    const double BIG = 1.0E308;
    return (v > -BIG && value < BIG);
#endif
}


//-------------------------------------------------------------------------------------------------
// Konwersja tekstu na liczbe rzeczywista.
// Uwaga: Funkcja niskopoziomowa - wykonuje bardzo ograniczona walidacje tekstu wejsciowego.
//        Oczekuje znakow:  [+-]?(digits)?(.digits)?([eE][+-]?digits)? i nic poza tym.
//        Jesli ciag znakow jest niepoprawny, funkcja zwraca false i oddaje przez referencje
//        nieokreslona wartosc.
//
inline bool StrToDbl(const string& input, double& out)
{
    // Jezeli brak tresci, zakonczenie negatywne
    if (input.empty()) return false;

    const char* b = input.c_str();
    char* e = NULL;
    errno = 0;
    // Konwersja tekstu na liczbe rzeczywista
    const double v = strtod(b, &e);
    // Po konwersji:
    // - wskaznik poczatku i konca musza roznic sie 
    // - koncowy znak musi byc terminatorem
    // - funkcja strtod nie powinna zwrocic blednu w errno (np. over-/-underflow zakresu double)
    // Jezeli cos poszlo nie tak, zakonczenie z bledem
    if ((e == b) || (*e != '\0') || (errno == ERANGE)) return false;

#if defined(_MSC_VER)
    // Dodatkowa ochrona: odrzucenie +/-inf (np. po overflow).
    if (!IsFiniteDbl(v)) return false;
#endif

    out = v;
    // Oddanie wyniku przez referencje i zakonczenie pozytywne
    return true;
}




#define ALPHA_MAX 321272406  // gorna granica numeracji literowej ("ZZZZZZ")


//-------------------------------------------------------------------------------------------------
// Konwersja indeksu 1-based na tekst numeracji literowej pozycyjnej.
// Jest to typowy "bijective base-26 system", czyli pozycyjny system o podstawie 26, z alfabetem
// od 'A' do 'Z'. Zakres obslugiwanej numeracji to: [1..321272406] co odpowiada ["A".."ZZZZZZ"].
// Dla wartosci spoza zakresu zwracany jest pusty teskt.
//
inline string IntToAlphaNumStr(int value)
{
    // Jezeli podano wartosc spoza zakresu, zwrocenie pustego wyniku
    if (value < 1 || ALPHA_MAX < value) return string();

    cardinal n = 0;
    // Ustalenie liczby znakow potrzebnych do zapisania wyniku (tj. cyfr w systemie bb-26)
    for (int i = value; i > 0; i = (i -1) / 26) n++;
    // Prealokacja tekstu wynikowego
    string str(n, '\0');

    int v = value;
    // Rozklad podanej wartosci na 'cyfry' systemu bb-26 (poczawszy od najmniej znaczacej), tj. ...
    for (; n > 0; n--) {
        // ... przeksztalcenie pozycyjne liczby na znak z alfabetu systemu [A-Z]
        // i zapisanie go w buforze wynikowym, ...
        str[n -1] = char('A' + (v -1) % 26);
        // ... przeliczenie na nastepna pozycje systemu
        v = (v -1) / 26;
    };

    // Zwrocenie tekstu wynikowego
    return str;
}


//-------------------------------------------------------------------------------------------------
// Konwersja tekstu numeracji literowej pozycyjnej na indeks 1-based.
// Jest to typowy "bijective base-26 system", czyli pozycyjny system o podstawie 26, z alfabetem
// od 'A' do 'Z'. Zakres obslugiwanej numeracji to: ["A".."ZZZZZZ"] co odpowiada [1..321272406].
// Uwaga: Funkcja niskopoziomowa! Wykonuje bardzo ograniczona walidacje tekstu wejsciowego.
//        Dopuszczalne sa tylko znaki ASCII [A-Z]. Jesli dane sa bledne (lub ich brak),
//        funkcja zwraca false a wartosc oddawana przez referencje jest nieokreslona.
//
inline bool AlphaNumStrToInt(const string& input, int& out)
{
    // Ustalenie liczby znakow w podanym tekscie, ...
    cardinal n = input.size();
    // ... jesli brak, zakonczenie negatywne
    if (n == 0) return false;

    // Jezeli ilosc znakow jest nadmierna, zakonczenie negatywne
    // (mitygacja ryzyka przekroczenia 32-bit wyniku)
    if (n > 6) return false;

    out = 0;

    int weight = 1;
    // Poczawszy od 'cyfry' najmlodszej o wadze 1 (znak na koncu tekstu wejsciowego),
    // iterowanie po znakach tekstu ...
    for (; n > 0; n--)
    {
        // ... pobranie znaku, ...
        const char ch = input[n -1];
        // ... jezeli wykracza on poza alfabet systemu [A-Z], zakonczenie negatywne
        if (!IsAsciiUpperAlpha(ch)) return false;

        // ... jesli Ok, wyliczenie wartosci pozycyjnej znaku i doliczenie go do wyniku, ...
        out += static_cast<int>((ch - 'A') +1) * weight;
        // ... obliczenie wagi nastepnej pozycji systemu
        weight *= 26;
    }

    // Oddanie wyniku przez referencje i zakonczenie pozytywne
    return true;
}




#define ROMAN_MAX 3999   // gorna granica numeracji rzymskiej ("MMMCMXCIX")


//-------------------------------------------------------------------------------------------------
// Liczba rzymska
//
struct TRomanNumber
{
    const char* symbol;
    const short value;

    // Porownuje biezacy fragment lancucha tekstowego z symbolem liczby. Gdy sa zgodne - przesuwa
    // wskaznik na kolejny fragment i zwraca true. Jesli nie - niczego nie zmienia i zwraca false.
    bool AdvanceIfMatches(const char*& text) const 
    {
        cardinal i = 0;
        // Iterowanie po znakach symbolu (aby porownac go z biezacym fragmentem lancucha) ...
        for (; symbol[i] != '\0'; i++) {
            // ... jezeli lancuch skonczyl sie wczesniej niz symbol, zakonczenie negatywne
            if (text[i] == '\0') return false;
            // ... jezeli biezacy znak lancucha nie pasuje, zakonczenie negatywne
            if (text[i] != symbol[i]) return false;
        }
        // Jest zgodnosc, wiec przesuniecie wskaznika na kolejny fragment
        text += i;
        // Oddanie wskaznika przez referencje i zakonczenie pozytywne
        return true;
    }
};


//-------------------------------------------------------------------------------------------------
// Tablica liczb rzymskich.
// Elementy celowo ulozone w kolejnosci malejacej - porzadek pod konwersje algorytmem "zachlannym".
//
static const TRomanNumber kRomanNumbers[] =
{
    {"M", 1000}, {"CM", 900}, {"D", 500}, {"CD", 400},
    {"C", 100},  {"XC", 90},  {"L", 50},  {"XL", 40},
    {"X", 10},   {"IX", 9},   {"V", 5},   {"IV", 4},
    {"I", 1}
};
const cardinal kRomanCount = 13;


//-------------------------------------------------------------------------------------------------
// Konwersja indeksu 1-based na tekst numeracji rzymskiej.
//
inline string IntToRomanNumStr(int value)
{
    // Jezeli podano wartosc spoza zakresu, zwrocenie pustego wyniku
    if (value < 1 || ROMAN_MAX < value) return string();
    
    const cardinal kSize = 16;
    string str;
    // Prealokacja tekstu wynikowego
    str.reserve(kSize);

    // Tworzenie zapisu liczby rzymskiej algorytmem "zachlannym" (wybor najwiekszych dopasowan), tj.:
    // iterowanie po elementach z tablicy liczb rzymskich - od najwiekszej do coraz mniejszych ...
    for (cardinal i = 0; i < kRomanCount; i++) 
    {
        const short rv = kRomanNumbers[i].value;
        const char* rs = kRomanNumbers[i].symbol;
        // ... i tak dlugo jak wybrana z tablicy liczba miesci sie w reszcie z podanej wartosci ...
        while (rv <= value) {
            // ... odejmowanie jej ...
            value -= rv;
            // ... i dopisywanie symbolu do tekstu wynikowego
            str.append(rs);
        }
        // ... az do wyzerowania reszty
        if (value == 0) break;
    }

    // Zwrocenie tekstu wynikowego
    return str;
}


//-------------------------------------------------------------------------------------------------
// Konwersja tekstu numeracji rzymskiej na indeks 1-based.
// Uwaga: Funkcja niskopoziomowa! Wykonuje bardzo ograniczona walidacje tekstu wejsciowego.
//        Oczekuje na wejsciu: [I,V,X,L,C,D,M] i nic poza tym. 
//        Dopuszczalne sa tylko znaki ASCII [A-Z]. Jesli dane sa bledne (lub ich brak),
//        funkcja zwraca false a wartosc oddawana przez referencje jest nieokreslona.
//
inline bool RomanNumStrToInt(const string& input, int& out)
{
    // Ustalenie liczby znakow w podanym tekscie, ...
    cardinal n = input.size();
    // ... jesli brak, zakonczenie negatywne
    if (n == 0) return false;

    // Jezeli ilosc znakow jest nadmierna, zakonczenie negatywne
    // (najdluzszy rozpoznawalny ciag to: "MMMDCCCLXXXVIII")
    if (n > 15) return false;

    out = 0;

    const char* p = input.c_str();
    // Dopoki lancuch znakow (podany tekst) nie zostanie w calosci przetworzony ...
    while (*p) 
    {
        bool matches = false;
        // ... iterowanie po elementach z tablicy liczb rzymskich ...
        for (cardinal i = 0; i < kRomanCount; i++)
        {
            // ... w poszukiwaniu dopasowania symbolu liczby do wskazanego fragmentu lancucha;
            // jezeli liczba rozpozna swoj symbol w tekscie, ...
            if (kRomanNumbers[i].AdvanceIfMatches(p)) {
                // ... to doliczenie jej wartosci do wyniku i zgloszenie dopasowania
                out += kRomanNumbers[i].value;
                matches = true;
                break; 
            }
        }

        // Jezeli nie udalo sie dopasowac zadnego symbolu, zakonczenie negatywne
        if (!matches) return false;
    }

    // Oddanie wyniku przez referencje i zakonczenie pozytywne
    return true;
}


} // namespace cans


#endif // CA_STRCONVERTERS_H
