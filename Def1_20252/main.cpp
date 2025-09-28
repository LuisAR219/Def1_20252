#include <iostream>
#include <fstream>


using namespace std;

unsigned char rotarIzquierda(unsigned char valor, int n);
unsigned char rotarDerecha(unsigned char valor, int n);
void encriptarArreglo(unsigned char* entrada, unsigned char* salida, int tam, int n, unsigned char K);
bool encontrarParametrosPorFormato(unsigned char* pista, int tamPista, unsigned char* textoCifrado, int tamCifrado,  int &nDetectado, unsigned char &KDetectado, int &pos, int &metodoDetectado);
void desencriptarTexto(unsigned char* textoCifrado, int tamañoCifrado, unsigned char* textoDesencriptado, int n, unsigned char K);
bool encontrarParametros(unsigned char* S, int M, unsigned char* C, int N, int &nDetectado, unsigned char &KDetectado, int &pos);
unsigned char* leerEncriptado(const char* ruta, int &tam);
unsigned char* leerPista(const char* ruta, int &tam);
void imprimirUTF8(unsigned char* texto, int tam, int limite);
unsigned char* descompresionLZ78(const unsigned char* desencriptado, int longitud, int &tamDinamico);
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

bool encontrarParametrosPorFormato(unsigned char* pista, int tamPista, unsigned char* textoCifrado, int tamCifrado, int &nDetectado, unsigned char &KDetectado, int &pos, int &metodoDetectado) {

    for (int n = 1; n < 8; n++) {
        for (int K = 0; K < 256; K++) {
            unsigned char* desencriptado = new unsigned char[tamCifrado];
            desencriptarTexto(textoCifrado, tamCifrado, desencriptado, n, (unsigned char)K);

            bool formatoValido = true;
            int metodo = 0;

            for (int i = 0; i < tamCifrado - 2; i += 3) {
                unsigned char byte1 = desencriptado[i];
                unsigned char byte2 = desencriptado[i + 1];
                unsigned char byte3 = desencriptado[i + 2];

                bool tercerByteValido = ((byte3 >= 'A' && byte3 <= 'Z') ||
                                         (byte3 >= 'a' && byte3 <= 'z') ||
                                         (byte3 >= '0' && byte3 <= '9'));

                if (!tercerByteValido) {
                    formatoValido = false;
                    break;
                }

                if (i == 0) {
                    bool pareceLZ78 = (byte1 == 0 && byte2 == 0);

                    if (pareceLZ78) {
                        metodo = 2;
                    } else {
                        metodo = 1;
                    }
                }
            }

            if (formatoValido && metodo != 0) {
                unsigned char* textoDescomprimido = nullptr;
                int tamDescomprimido = 0;
                bool pistaEncontrada = false;

                if (metodo == 1) {
                    textoDescomprimido = descompresionRLE(desencriptado, tamCifrado, tamDescomprimido);
                } else if (metodo == 2) {
                    textoDescomprimido = descompresionLZ78(desencriptado, tamCifrado, tamDescomprimido);
                }

                if (textoDescomprimido && tamDescomprimido >= tamPista) {
                    for (int i = 0; i <= tamDescomprimido - tamPista; i++) {
                        bool coincide = true;
                        for (int j = 0; j < tamPista; j++) {
                            if (textoDescomprimido[i + j] != pista[j]) {
                                coincide = false;
                                break;
                            }
                        }
                        if (coincide) {
                            pistaEncontrada = true;
                            pos = i;
                            break;
                        }
                    }
                }

                if (pistaEncontrada) {
                    nDetectado = n;
                    KDetectado = (unsigned char)K;
                    metodoDetectado = metodo;

                    delete[] desencriptado;
                    desencriptado = nullptr;

                    if (textoDescomprimido) {
                        delete[] textoDescomprimido;
                        textoDescomprimido = nullptr;
                    }

                    cout << "¡Parámetros encontrados por formato!" << endl;
                    cout << "n=" << n << ", K=" << K << ", Método=" << (metodo == 1 ? "RLE" : "LZ78") << endl;
                    return true;
                }

                if (textoDescomprimido) {
                    delete[] textoDescomprimido;
                    textoDescomprimido = nullptr;
                }
            }

            delete[] desencriptado;
            desencriptado = nullptr;
        }
    }

    metodoDetectado = 0;
    return false;
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

