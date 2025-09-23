#include <iostream>
#include <fstream>

using namespace std;

unsigned char rotarIzquierda(unsigned char valor, int n);
unsigned char rotarDerecha(unsigned char valor, int n);
void encriptarArreglo(unsigned char* entrada, unsigned char* salida, int tam, int n, unsigned char K);
bool encontrarParametros(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos);
void probarCaso(const char* nombre, unsigned char* S, int M, int nReal, unsigned char KReal);
void desencriptarTexto(unsigned char* textoCifrado, int tamañoCifrado, unsigned char* textoDesencriptado, int n, unsigned char K);
unsigned char* leerEncriptado(const char* ruta, int &tam);
unsigned char* leerPista(const char* ruta, int &tam);

int main() {

}

unsigned char* leerEncriptado(const char* ruta, int &tam) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo) {
        cout << "Error: no se pudo abrir " << ruta << endl;
        tam = 0;
        return nullptr;
    }

    archivo.seekg(0, ios::end);
    tam = archivo.tellg();
    archivo.seekg(0, ios::beg);

    unsigned char* buffer = new unsigned char[tam];
    archivo.read((char*)buffer, tam);
    archivo.close();

    return buffer;
}

unsigned char* leerPista(const char* ruta, int &tam) {
    ifstream archivo(ruta);
    if (!archivo) {
        cout << "Error: no se pudo abrir " << ruta << endl;
        tam = 0;
        return nullptr;
    }

    tam = 0;
    char c;
    while (archivo.get(c)) {
        tam++;
    }

    archivo.clear();
    archivo.seekg(0, ios::beg);

    unsigned char* buffer = new unsigned char[tam];
    for (int i = 0; i < tam; i++) {
        archivo.get(c);
        buffer[i] = (unsigned char)c;
    }

    archivo.close();
    return buffer;
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

void desencriptarTexto(unsigned char* textoCifrado, int tamañoCifrado, unsigned char* textoDesencriptado, int n, unsigned char K) {
    for (int i = 0; i < tamañoCifrado; i++) {
        unsigned char temp = textoCifrado[i] ^ K;

        textoDesencriptado[i] = rotarDerecha(temp, n);
    }
}

bool encontrarParametros(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos) {
    for (int i = 0; i <= N - M; i++) {
        for (int n = 1; n < 8; n++) {
            unsigned char K = rotarIzquierda(S[0], n) ^ C[i];
            bool verificado = true;

            int j = 0;
            while (j < M && verificado) {
                unsigned char esperado = rotarIzquierda(S[j], n) ^ K;
                if (esperado != C[i+j]) {
                    verificado = false;
                }
                j++;
            }

            if (verificado) {
                nDetectado = n;
                KDetectado = K;
                pos = i;
                return true;
            }
        }
    }
    return false;
}

void probarCaso(const char* nombre, unsigned char* S, int M, int nReal, unsigned char KReal) {
    cout << "=== " << nombre << " ===" << endl;

    unsigned char* C = new unsigned char[M];
    encriptarArreglo(S, C, M, nReal, KReal);

    cout << "Texto cifrado generado: ";
    for (int i = 0; i < M; i++) cout << hex << (int)C[i] << " ";
    cout << dec << endl;

    int n; unsigned char K; int pos;
    if (encontrarParametros(S, M, C, M, n, K, pos)) {
        cout << "Detectado n = " << n << ", K = " << (int)K << ", pos = " << pos << endl;
    } else {
        cout << "No se encontro coincidencia" << endl;
    }

    cout << endl;

    delete[] C;
    C = nullptr;
}
