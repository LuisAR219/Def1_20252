#include <iostream>

using namespace std;

unsigned char rotarIzquierda(unsigned char valor, int n);
unsigned char rotarDerecha(unsigned char valor, int n);

int main()
{

}


unsigned char rotarIzquierda(unsigned char valor, int n) {
    return (unsigned char)((valor << n) | (valor >> (8 - n)));
}
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (unsigned char)((valor >> n) | (valor << (8 - n)));
}

void encriptarArreglo(unsigned char* entrada, unsigned char* salida, int tam, int n, unsigned char K) {
    for (int i = 0; i < tam; i++) {
        unsigned char temp = rotarIzquierda(entrada[i], n);
        salida[i] = temp ^ K;
    }
}
