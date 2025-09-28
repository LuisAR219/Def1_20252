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
unsigned char* descompresionLZ78(const unsigned char* desencriptado, int longitud, int &tamDinamico);
unsigned char* descompresionRLE(unsigned char* desencriptado, int longitud, int &totalSalida);
void actualizacionArreglo(unsigned char*& buffer, int& tamaño, int& capacidad, unsigned char valor);



int main() {
    const char* rutaBase = "D:\\Informatica 2\\Desafios\\Def1_20252\\Def1_20252\\";

    int numCasos = 0;
    cout << "Cuantos casos de prueba hay?: ";
    cin >> numCasos;

    if (numCasos <= 0) {
        return 1;
    }

    for (int caso = 1; caso <= numCasos; caso++) {
        char archivoEncriptado[256];
        char archivoPista[256];
        char archivoResultado[256];

        sprintf(archivoEncriptado, "%sEncriptado%d.txt", rutaBase, caso);
        sprintf(archivoPista, "%spista%d.txt", rutaBase, caso);
        sprintf(archivoResultado, "%sresultado%d.txt", rutaBase, caso);

        int tamEnc = 0, tamPista = 0;

        unsigned char* encriptado = leerEncriptado(archivoEncriptado, tamEnc);
        unsigned char* pista = leerPista(archivoPista, tamPista);

        if (!encriptado || !pista) {
            if (encriptado) {
                delete[] encriptado;
                encriptado = nullptr;
            }
            if (pista) {
                delete[] pista;
                pista = nullptr;
            }
            continue;
        }

        int nDetectado = 0, metodoDetectado = 0, posicion = 0;
        unsigned char KDetectado = 0;

        if (encontrarParametrosPorFormato(pista, tamPista, encriptado, tamEnc,
                                          nDetectado, KDetectado, posicion, metodoDetectado)) {
            unsigned char* desencriptado = new unsigned char[tamEnc];
            desencriptarTexto(encriptado, tamEnc, desencriptado, nDetectado, KDetectado);

            unsigned char* textoFinal = nullptr;
            int tamFinal = 0;

            if (metodoDetectado == 1) {
                textoFinal = descompresionRLE(desencriptado, tamEnc, tamFinal);
            } else {
                textoFinal = descompresionLZ78(desencriptado, tamEnc, tamFinal);
            }

            if (textoFinal) {
                ofstream resultadoFile(archivoResultado);
                if (resultadoFile.is_open()) {
                    resultadoFile << "=== PARAMETROS DE DESENCRIPTACION ===" << endl;
                    resultadoFile << "n (rotación): " << nDetectado << endl;
                    resultadoFile << "K (clave): " << (int)KDetectado << endl;
                    resultadoFile << "Metodo de compresion: " << (metodoDetectado == 1 ? "RLE" : "LZ78") << endl;
                    resultadoFile << "======================================" << endl;
                    resultadoFile << endl;

                    resultadoFile << "=== TEXTO ORIGINAL ===" << endl;
                    for (int i = 0; i < tamFinal; i++) {
                        resultadoFile << textoFinal[i];
                    }
                    resultadoFile.close();
                }

                delete[] textoFinal;
                textoFinal = nullptr;
            }

            delete[] desencriptado;
            desencriptado = nullptr;

        } else {
            if (encontrarParametros(pista, tamPista, encriptado, tamEnc, nDetectado, KDetectado, posicion)) {
                unsigned char* desencriptado = new unsigned char[tamEnc];
                desencriptarTexto(encriptado, tamEnc, desencriptado, nDetectado, KDetectado);

                unsigned char* textoFinal = nullptr;
                int tamFinal = 0;

                textoFinal = descompresionRLE(desencriptado, tamEnc, tamFinal);
                if (textoFinal) {
                    bool pistaEncontrada = false;
                    if (tamFinal >= tamPista) {
                        for (int i = 0; i <= tamFinal - tamPista; i++) {
                            bool coincide = true;
                            for (int j = 0; j < tamPista; j++) {
                                if (textoFinal[i + j] != pista[j]) {
                                    coincide = false;
                                    break;
                                }
                            }
                            if (coincide) {
                                pistaEncontrada = true;
                                break;
                            }
                        }
                    }

                    if (pistaEncontrada) {
                        ofstream resultadoFile(archivoResultado);
                        if (resultadoFile.is_open()) {
                            resultadoFile << "=== PARAMETROS DE DESENCRIPTACION ===" << endl;
                            resultadoFile << "n (rotación): " << nDetectado << endl;
                            resultadoFile << "K (clave): " << (int)KDetectado << endl;
                            resultadoFile << "Metodo de compresion: RLE" << endl;
                            resultadoFile << "======================================" << endl;
                            resultadoFile << endl;
                            resultadoFile << "=== TEXTO ORIGINAL ===" << endl;
                            for (int i = 0; i < tamFinal; i++) {
                                resultadoFile << textoFinal[i];
                            }
                            resultadoFile.close();
                        }
                    }
                    delete[] textoFinal;
                    textoFinal = nullptr;
                } else {
                    textoFinal = descompresionLZ78(desencriptado, tamEnc, tamFinal);
                    if (textoFinal) {
                        ofstream resultadoFile(archivoResultado);
                        if (resultadoFile.is_open()) {
                            resultadoFile << "=== PARAMETROS DE DESENCRIPTACION ===" << endl;
                            resultadoFile << "n (rotación): " << nDetectado << endl;
                            resultadoFile << "K (clave): " << (int)KDetectado << endl;
                            resultadoFile << "Metodo de compresion: LZ78" << endl;
                            resultadoFile << "======================================" << endl;
                            resultadoFile << endl;
                            resultadoFile << "=== TEXTO ORIGINAL ===" << endl;
                            for (int i = 0; i < tamFinal; i++) {
                                resultadoFile << textoFinal[i];
                            }
                            resultadoFile.close();
                        }
                        delete[] textoFinal;
                        textoFinal = nullptr;
                    }
                }

                delete[] desencriptado;
                desencriptado = nullptr;
            }
        }

        delete[] encriptado;
        encriptado = nullptr;

        delete[] pista;
        pista = nullptr;
    }

    return 0;
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

                    cout << "Parametros encontrados por formato:" << endl;
                    cout << "n=" << n << ", K=" << K << ", Metodo=" << (metodo == 1 ? "RLE" : "LZ78") << endl;
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

