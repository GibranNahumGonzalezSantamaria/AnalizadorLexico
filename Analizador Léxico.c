#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Token
{
    char lexema[35];
    char *tipoToken;
    struct Token *siguiente;
} *inicio = NULL, *final = NULL, *auxiliar = NULL;

void agregarToken(char lexema[35], char *tipoToken); // Funcion para almacenar tokens en la tabla de simbolos
void imprimirTablaTokens();                          // Funcion para imprimir la tabla de simbolos
void analizarLexico(char *cadena);                   // Funcion para analizar la cadena

int main()
{
    FILE *archivo = fopen("Fichero.txt", "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo.");
        return 0;
    }

    printf("<<< ANALIZADOR LEXICO >>>\n\n");

    char linea[1024];
    char *token;

    while (fgets(linea, sizeof(linea), archivo))
    {
        linea[strcspn(linea, "\n")] = 0; // Eliminar el salto de linea

        // Separar la linea en tokens usando espacios, tabulaciones y saltos de linea
        token = strtok(linea, " \t\n");
        while (token != NULL)
        {
            analizarLexico(token);         // Analisis de cada token
            token = strtok(NULL, " \t\n"); // Siguiente token
        }
    }

    // Imprimir la tabla de simbolos al final del analisis
    imprimirTablaTokens();

    fclose(archivo);
    return 0;
}

// Funcion para almacenar tokens en la tabla de simbolos
void agregarToken(char lexema[10], char *tipoToken)
{
    if (inicio == NULL)
    {
        inicio = malloc(sizeof(struct Token));
        strcpy(inicio->lexema, lexema);
        inicio->tipoToken = tipoToken;
        inicio->siguiente = NULL;
        final = inicio;
    }
    else
    {
        auxiliar = malloc(sizeof(struct Token));
        strcpy(auxiliar->lexema, lexema);
        auxiliar->tipoToken = tipoToken;
        auxiliar->siguiente = NULL;
        final->siguiente = auxiliar;
        final = auxiliar;
    }
}

// Funcion para imprimir la tabla de simbolos
void imprimirTablaTokens()
{
    struct Token *actual = inicio;
    printf("\n--- Tabla de Tokens ---\n");
    printf("%-20s %-20s\n", "Lexema", "Tipo de Token");
    printf("-------------------------------\n");

    while (actual != NULL)
    {
        // Imprimir el lexema y el tipo de token
        printf("%-20s %-20s\n", actual->lexema, actual->tipoToken);
        actual = actual->siguiente;
    }
    printf("-------------------------------\n");
}

void analizarLexico(char *cadena)
{
    char *punteroCaracter, *palabrasReservadas[] = {"if", "else", "while", "return", "for", "int", "float", "void", NULL};
    int estado = 0, indiceCaracter = 0, longitudCadena = 0;

    punteroCaracter = cadena;
    longitudCadena = strlen(punteroCaracter);

    while (indiceCaracter <= longitudCadena)
    {
        switch (estado)
        {
        case 0: // Estado inicial
            if (isalpha(punteroCaracter[indiceCaracter]))
                estado = 1;
            else if (strchr("+-", punteroCaracter[indiceCaracter]))
                estado = 3;
            else if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 4;
            else if (strchr("&|", punteroCaracter[indiceCaracter]))
                estado = 10;
            else if (punteroCaracter[indiceCaracter] == '!')
                estado = 11;
            else if (strchr("><", punteroCaracter[indiceCaracter]))
                estado = 13;
            else if (punteroCaracter[indiceCaracter] == '=')
                estado = 14;
            else if (strchr("*/", punteroCaracter[indiceCaracter]))
                estado = 16;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 1: // Estado de identificador (ID)
            if (isalnum(punteroCaracter[indiceCaracter]))
                estado = 1;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            else
            {
                // Verificar si es palabra reservada
                for (int i = 0; palabrasReservadas[i] != NULL; i++)
                    if (strncmp(punteroCaracter, palabrasReservadas[i], indiceCaracter) == 0 && indiceCaracter == longitudCadena)
                    {
                        estado = 2;
                        break;
                    }
            }
            break;

        case 2: // Estado de palabra reservada (PR)
            if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 3: // Estado de operador aritmetico (OA)
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 4;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 4: // Estado de numero entero (NE)
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 4;
            else if (punteroCaracter[indiceCaracter] == '.')
                estado = 5;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 5:
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 6;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 6: // Estado de numero decimal (ND)
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 6;
            else if (punteroCaracter[indiceCaracter] == 'E')
                estado = 7;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 7:
            if (strchr("+-", punteroCaracter[indiceCaracter]))
                estado = 8;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 8:
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 9;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 9: // Estado de numero exponencial (NX)
            if (isdigit(punteroCaracter[indiceCaracter]))
                estado = 9;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 10:
            if (strchr("&|", punteroCaracter[indiceCaracter]))
                estado = 12;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 11: // Estado de operador logico (OL)
            if (punteroCaracter[indiceCaracter] == '=')
                estado = 15;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 12: // Estado de operador logico (OL)
            if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 13: // Estado de operador relacional (OR)
            if (punteroCaracter[indiceCaracter] == '=')
                estado = 15;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 14:
            if (punteroCaracter[indiceCaracter] == '=')
                estado = 15;
            else if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 15: // Estado de operador relacional (OR)
            if (indiceCaracter < longitudCadena)
                estado = -1;
            break;

        case 16: // Estado de operador aritmetico (OA)
            if (indiceCaracter < longitudCadena)
                estado = -1;
            break;
        }
        indiceCaracter++;
    }
    indiceCaracter--;

    if (indiceCaracter == longitudCadena)
    {
        char *tipoToken = ""; // Tipo de token para la tabla de simbolos
        switch (estado)
        {
        case 1:
            printf(" * La cadena \"%s\" es un identificador.", punteroCaracter);
            tipoToken = "ID";
            break;

        case 2:
            printf(" * La cadena \"%s\" es una palabra reservada.", punteroCaracter);
            tipoToken = "PR";
            break;

        case 4:
            printf(" * La cadena \"%s\" es un numero entero.", punteroCaracter);
            tipoToken = "NE";
            break;

        case 6:
            printf(" * La cadena \"%s\" es un numero decimal.", punteroCaracter);
            tipoToken = "ND";
            break;

        case 9:
            printf(" * La cadena \"%s\" es un numero exponencial.", punteroCaracter);
            tipoToken = "NX";
            break;

        case 11:
        case 12:
            printf(" * La cadena \"%s\" es un operador logico.", punteroCaracter);
            tipoToken = "OL";
            break;

        case 13:
        case 15:
            printf(" * La cadena \"%s\" es un operador relacional.", punteroCaracter);
            tipoToken = "OR";
            break;

        case 3:
        case 16:
            printf(" * La cadena \"%s\" es un operador aritmetico.", punteroCaracter);
            tipoToken = "OA";
            break;

        default:
            printf(" * La cadena \"%s\" no es un token valido.", punteroCaracter);
            tipoToken = "--"; // Token invalido
            break;
        }

        if (strcmp(tipoToken, "--") != 0)
            agregarToken(punteroCaracter, tipoToken); // Guardar en la tabla de simbolos
    }

    printf("\n");
}