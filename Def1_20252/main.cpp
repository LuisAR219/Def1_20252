#include <iostream>
#include <fstream>


using namespace std;

unsigned char rotarIzquierda(unsigned char valor, int n);
unsigned char rotarDerecha(unsigned char valor, int n);
void encriptarArreglo(unsigned char* entrada, unsigned char* salida, int tam, int n, unsigned char K);
bool encontrarParametros(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos);
bool encontrarParametrosBruto(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos);
void probarCaso(const char* nombre, unsigned char* S, int M, int nReal, unsigned char KReal);
void desencriptarTexto(unsigned char* textoCifrado, int tamañoCifrado, unsigned char* textoDesencriptado, int n, unsigned char K);
unsigned char* leerEncriptado(const char* ruta, int &tam);
unsigned char* leerPista(const char* ruta, int &tam);
void imprimirUTF8(unsigned char* texto, int tam, int limite);
unsigned char* descompresionLZ78(unsigned char* desencriptado, int longitud);
unsigned char* descompresionRLE(unsigned char* desencriptado, int longitud, int &totalSalida);
void actualizacionArreglo(unsigned char*& buffer, int& tamaño, int& capacidad, unsigned char valor);

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

bool encontrarParametrosBruto(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos) {
    if (M <= 0 || N < M) return false;

    for (int i = 0; i <= N - M; i++) {
        for (int n = 1; n < 8; n++) {
            for (int Kint = 0; Kint < 256; Kint++) {
                unsigned char K = (unsigned char)Kint;
                bool verificado = true;

                int j = 0;
                while (j < M && verificado) {
                    unsigned char esperado = (unsigned char)(( (unsigned char)((S[j] << n) | (S[j] >> (8 - n))) ) ^ K);
                    if (esperado != C[i + j]) {
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

void imprimirUTF8(unsigned char* texto, int tam, int limite = 200) {
    for (int i = 0; i < tam && i < limite; i++) {
        cout << (char)texto[i];
    }
    cout << endl;
}

void actualizacionArreglo(unsigned char*& buffer, int& tamaño, int& capacidad, unsigned char valor) {
    if (tamaño >= capacidad) {
        int nuevaCapacidad = (capacidad == 0) ? 16 : capacidad * 2;
        unsigned char* nuevo = new unsigned char[nuevaCapacidad];
        for (int i = 0; i < tamaño; ++i){
            nuevo[i] = buffer[i];
        }
        delete[] buffer;
        buffer = nuevo;
        capacidad = nuevaCapacidad;
    }
    buffer[tamaño++] = valor;
}

unsigned char* descompresionLZ78(const unsigned char* desencriptado, int longitud, int &tamDinamico) {
    tamDinamico = 0;
    if (!desencriptado || longitud <= 0) return nullptr;

    int numEntradas = longitud / 3 + 2;
    unsigned short* posiciones = new unsigned short[numEntradas];
    unsigned char* valores = new unsigned char[numEntradas];
    posiciones[0] = 0;
    valores[0] = 0;

    unsigned char* textoDescomprimido = nullptr;
    int posionActual = -1;
    int capacidad = 0;
    int tamaño = 0;

    for (int i = 0; i + 2 < longitud; i += 3) {
        unsigned int b1 = desencriptado[i];
        unsigned int b2 = desencriptado[i+1];
        unsigned char ascii = desencriptado[i+2];

        unsigned short posicion = b1 * 256 + b2;
        int actual = (i / 3) + 1;
        posiciones[actual] = posicion;
        valores[actual] = ascii;

        unsigned int posicionVariante = posiciones[actual];
        int *temp = new int[(actual > 0) ? actual : 1];
        int cnt = -1;
        while (posicionVariante > 0) {
            ++cnt;
            temp[cnt] = (int)posicionVariante;
            posicionVariante = posiciones[posicionVariante];
        }

        if (cnt > -1) {
            for (int k = cnt; k >= 0; --k) {
                actualizacionArreglo(textoDescomprimido, tamaño, capacidad, valores[temp[k]]);
                posionActual++;
            }
        }
        actualizacionArreglo(textoDescomprimido, tamaño, capacidad, valores[actual]);
        posionActual++;
        delete[] temp;
    }

    tamDinamico = tamaño;

    delete[] posiciones;
    delete[] valores;

    return textoDescomprimido;
}

unsigned char* descompresionRLE(unsigned char* desencriptado, int longitud, int &totalSalida) {
    totalSalida = 0;
    for (int i = 0; i < longitud; i += 3) {
        unsigned int cnt = (unsigned int) desencriptado[i + 1];
        totalSalida += cnt;
    }

    unsigned char* textoDescomprimido = new unsigned char[totalSalida];
    unsigned int posionActual = 0;
    for (int i = 0; i < longitud; i += 3) {
        unsigned int cnt = (unsigned int) desencriptado[i + 1];
        unsigned char ch = desencriptado[i + 2];
        for (unsigned int k = 0; k < cnt; ++k) {
            textoDescomprimido[posionActual++] = ch;
        }
    }
    return textoDescomprimido;
}

