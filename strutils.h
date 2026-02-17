#ifndef CA_STRUTILS_H
#define CA_STRUTILS_H

//-------------------------------------------------------------------------------------------------
// Zaleznosci (naglowki uzyte w tym module):
//
// C++ / STL
//   <cctype>     -> isspace()
//   <string>     -> std::string
//
// Repository
//   "numutils.h" -> cardinal
//

#include <cctype>
#include <string>

#include "numutils.h"




namespace cans
{
    using std::string;


//////////////////////////////////////////////////////////////////////////////////////////////////
// Dzial: Proste narzedzia analizy i obrobki tekstu (ASCII).
// Warstwa: Model / Utilities
//-------------------------------------------------------------------------------------------------
// Cel:
//   Niskopoziomowy toolkit do zastosowan ogolnych przy pracy na danych tekstowych. Funkcje operuja
//   wylacznie na znakach ASCII oraz prostych buforach C-string i string. Wykonuja podstawowe testy
//   znakow oraz elementarne transformacje tekstu (zamiana, usuwanie, obcinanie).
//   Modul stanowi fundament dla wyzszych warstw logiki (walidatorow, parserow, itp.)
//
// Zakres odpowiedzialnosci:
// - Operacje na pojedynczych znakach ASCII (klasyfikacja: cyfra, litera, bialy znak, itp.)
// - Proste modyfikacje tekstu in-place (zmiana wielkosci liter, podmiana znakow)
// - Proste transformacje jak wyzej, lecz zwracajace nowy tekst
// - Funkcje nie wykonuja walidacji semantycznej, parsowania ani interpretacji.
//
// Uwagi projektowe:
// * Modul obsluguje wylacznie 8-bitowe znaki (ASCII) i nie nadaje sie do tekstow Unicode.
// * Funkcje operujace na C-stringach nie moga przetwarzac znakow o wartosci 0 ('\0')
//

//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy bialy znak.
//
inline bool IsAsciiWhitespace(char ch)
{
    // Zwrocenie true dla bialych znakow: '\t', '\n', '\v', '\f', '\r', ' '
    return ::isspace(static_cast<unsigned char>(ch));
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy cyfra ASCII.
//
inline bool IsAsciiDigit(char ch)
{
    // Zwrocenie true dla znakow z zakresu [0-9]
    return ('0' <= ch && ch <= '9');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy kropka ASCII.
//
inline bool IsAsciiDot(char ch)
{
    // Zwrocenie true dla znaku [.]
    return (ch == '.');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy przecinek ASCII.
//
inline bool IsAsciiColon(char ch)
{
    // Zwrocenie true dla znaku [,]
    return (ch == ',');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy srednik ASCII.
//
inline bool IsAsciiSemicolon(char ch)
{
    // Zwrocenie true dla znaku [;]
    return (ch == ';');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy wielka litera ASCII.
//
inline bool IsAsciiUpperAlpha(char ch)
{
    // Zwrocenie true dla znakow z zakresu [A-Z]
    return ('A' <= ch && ch <= 'Z');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy mala litera ASCII.
//
inline bool IsAsciiLowerAlpha(char ch)
{
    // Zwrocenie true dla znakow z zakresu [a-z]
    return ('a' <= ch && ch <= 'z');
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy litera ASCII.
//
inline bool IsAsciiAlpha(char ch)
{
    // Zwrocenie true dla znakow z zakresu [A-Za-z]
    return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie wielkich liter ASCII na male (modyfikacja in-place).
//
inline void MakeLowercase(string& str)
{
    // Iteracja po znakach tekstu ...
    for (cardinal i = 0; i < str.size(); i++) {
        // ... z podmiana liter [A-Z] na [a-z]  (constant folding 'a' - 'A' = 32)
        char& ch = str[i];
        if (IsAsciiUpperAlpha(ch)) 
            ch += char('a' - 'A');
    }
    // Wynik oddany przez referencje
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie wielkich liter ASCII na male (zwraca nowy tekst).
//
inline string ToLowercase(const string& str)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    MakeLowercase(bufstr);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


//-------------------------------------------------------------------------------------------------
// Zamiana malej litery ASCII na wielka (zwraca nowy znak).
//
inline char ToLowerAlpha(const char ch)
{
    // Zwrocenie malej litery
    return IsAsciiUpperAlpha(ch) ? char(ch + ('a' - 'A')) : ch;
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie malych liter ASCII na wielkie (modyfikacja in-place).
//
inline void MakeUppercase(string& str)
{
    // Iteracja po znakach tekstu ...
    for (cardinal i = 0; i < str.size(); i++) {
        // ... z podmiana liter [a-z] na [A-Z]  (constant folding 'a' - 'A' = 32)
        char& ch = str[i];
        if (IsAsciiLowerAlpha(ch)) 
            ch -= char('a' - 'A');
    }
    // Wynik oddany przez referencje
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie malych liter ASCII na wielkie (zwraca nowy tekst).
//
inline string ToUppercase(const string& str)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    MakeUppercase(bufstr);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


//-------------------------------------------------------------------------------------------------
// Zamiana malej litery ASCII na wielka (zwraca nowy znak).
//
inline char ToUpperAlpha(const char ch)
{
    // Zwrocenie wielkiej litery
    return IsAsciiLowerAlpha(ch) ? char(ch - ('a' - 'A')) : ch;
}


// Style liter
enum LetterCase
{
    Uppercase,   // "STRING"
    Lowercase,   // "string"
    Capitalize   // "String"
};


//-------------------------------------------------------------------------------------------------
// Zastosowanie wybranego stylu literowego do tekstu (modyfikacja in-place).
//
inline void ApplyLetterCase(string& str, LetterCase style)
{
    // W zaleznosci od wybranego stylu ...
    switch (style)
    {
        case Uppercase:
            // ... transformacja calego napisu do wielkich liter
            MakeUppercase(str);
            break;

        case Lowercase:
            // ... transformacja calego napisu do malych liter
            MakeLowercase(str);
            break;

        case Capitalize:
            // ... najpierw obnizenie wszystkich liter, ...
            MakeLowercase(str);
            // ... i podniesienie pierwszego znaku, o ile napis nie jest pusty
            if (!str.empty())
                str[0] = ToUpperAlpha(str[0]);
            break;
    }
}


//-------------------------------------------------------------------------------------------------
// Zastosowanie wybranego stylu literowego do tekstu (zwraca nowy tekst).
//
inline string ToLetterCase(const string& str, LetterCase style)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    ApplyLetterCase(bufstr, style);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie wystapien znaku <from> na <to> (modyfikacja in-place).
//
inline void ReplaceCharInPlace(string& str, char from, char to)
{
    // Jezeli nie ma co zmieniac, zakonczenie
    if (from == to) return;

    // Iteracja po znakach tekstu ...
    for (cardinal i = 0; i < str.size(); i++) {
        // ... z podmiana wszystkich wystapien znaku 'from' na 'to
        if (str[i] == from)
            str[i] = to;
    }
    // Wynik oddany przez referencje
}


//-------------------------------------------------------------------------------------------------
// Zamiana w calym tekscie wystapien znaku <from> na <to> (zwraca nowy tekst).
//
inline string ReplaceChar(const string& str, char from, char to)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    ReplaceCharInPlace(bufstr, from, to);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


//-------------------------------------------------------------------------------------------------
// Usuniecie z tekstu wszystkich wystapien danego znaku (modyfikacja in-place).
//
inline void RemoveCharInPlace(string& str, char ch)
{
    cardinal j = 0;
    // Iteracja po znakach tekstu ...
    for (cardinal i = 0; i < str.size(); i++) {
        // ... z przepisywaniem in-place wszystkich znakow roznych od usuwanego
        if (str[i] != ch) 
            str[j++] = str[i];
    }
    // Ustawienie nowej dlugosci tekstu (odciecie usunietej koncowki) i oddanie go przez referencje
    str.resize(j);
}


//-------------------------------------------------------------------------------------------------
// Usuniecie z tekstu wszystkich wystapien danego znaku (zwraca nowy tekst).
//
inline string RemoveChar(const string& str, char ch)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    RemoveCharInPlace(bufstr, ch);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


//-------------------------------------------------------------------------------------------------
// Usuniecie bialych znakow z obu stron tekstu (zwraca nowy tekst).
//
inline string TrimStr(const string& str)
{
    // Ustalenie liczby znakow w tekscie
    const cardinal n = str.size();

    cardinal b = 0;
    // Wyszukanie pierwszego znaku innego niz znak bialy, ...
    while (b < n && IsAsciiWhitespace(str[b])) b++;
    // ... a jesli brak, to zwrocenie pustego lancucha i zakonczenie
    if (b == n) return string();

    cardinal e = n;
    // Wyszukanie pierwszego od konca znaku innego niz znak bialy
    while (e > b && IsAsciiWhitespace(str[e -1])) e--;

    // Jezeli znaleziono jakiekolwiek biale znaki ...
    if (e - b < n)
        // ... zwrocenie tekstu obustronnie oczyszczonego
        return str.substr(b, e - b);
    // ... w przeciwnym razie, wynik bez zmian
    return str;
}


//-------------------------------------------------------------------------------------------------
// Usuniecie bialych znakow z obu stron tekstu (modyfikacja in-place).
//
inline void TrimStrInPlace(string& str)
{
    // Przyciecie tekstu i oddanie wyniku przez referencje
    str = TrimStr(str);
}


//-------------------------------------------------------------------------------------------------
// Sprawdzenie, czy znak znajduje sie w podanym lancuchu <cstr>.
// Poszukiwany znak nie moze byc '\0' (ograniczenie wynika z natury C-String).
//
inline bool IsCharInCString(char ch, const char* cstr)
{
    // Iteracja po znakach lancucha (C-string zakonczony '\0') ...
    for (const char* c = cstr; *c; c++) {
        // ... i porownywanie, czy biezacy znak jest tym szukanym, ...
        if (*c == ch)
            // ... bo jesli tak, to zakonczenie z potwierdzeniem
            return true;
    }
    // Jezeli petla dobiegla konca, znak nie wystapil w lancuchu - zakonczenie negatywne
    return false;
}


//-------------------------------------------------------------------------------------------------
// Usuniecie z tekstu wszystkich wystapien znakow podanych w lancuchu <charset>, tj. w zbiorze
// znakow do usuniecia (modyfikacja in-place).
// Usuwanym znakiem nie moze byc '\0' (ograniczenie wynika z natury C-String).
//
inline void RemoveSetOfCharsInPlace(string& str, const char* charset)
{
    // Jezeli nie podano zbioru znakow do usuniecia lub jest on pusty, zakonczenie bez zmian
    if (!charset || !*charset) return;

    cardinal j = 0;
    // Iteracja po znakach tekstu, ...
    for (cardinal i = 0; i < str.size(); ++i) {
        // ... ze sprawdzaniem czy wystepuja w zbiorze znakow do usuniecia ...
        if (IsCharInCString(str[i], charset))
            // ... bo jesli tak, to pominiecie ich przepisywania
            continue;
        // ... reszta jest progresywnie przepisywana (tzw. "kompaktowanie w miejscu")
        str[j++] = str[i];
    }
    // Ustawienie nowej dlugosci tekstu (odciecie usunietej koncowki) i oddanie go przez referencje
    str.resize(j);
}


//-------------------------------------------------------------------------------------------------
// Usuniecie z tekstu wszystkich wystapien znakow podanych w lanuchu <charset>, tj. w zbiorze
// znakow do usuniecia (zwraca nowy tekst).
// Usuwanym znakiem nie moze byc '\0' (ograniczenie wynika z natury C-String).
//
inline string RemoveSetOfChars(const string& str, const char* charset)
{
    // Skopiowanie tekstu do bufora wynikowego ...
    string bufstr = str;
    // ... i modyfikacja in-place
    RemoveSetOfCharsInPlace(bufstr, charset);

    // Zwrocenie tekstu wynikowego
    return bufstr;
}


} // namespace cans


#endif // CA_STRUTILS_H
