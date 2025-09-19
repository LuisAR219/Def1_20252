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
