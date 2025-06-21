#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    Pixel **pixeles;
    int ancho;
    int alto;
    int imagenCargada;
    char nombreOriginal[256];
} Imagen;

void inicializarImagen(Imagen *imagen);
int cargarImagenBMP(Imagen *imagen, const char *rutaArchivo);
int guardarImagenEnCarpeta(Imagen *imagen, const char *nombreFiltro, const char *sufijo);
int guardarImagenBMP(Imagen *imagen, const char *rutaArchivo);
Pixel obtenerPixel(Imagen *imagen, int x, int y);
void establecerPixel(Imagen *imagen, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void liberarImagen(Imagen *imagen);
void aplicarFiltroGrises(Imagen *imagen);
void aplicarFiltroInversion(Imagen *imagen);
void aplicarFiltroBrillo(Imagen *imagen, int factorBrillo);
void mostrarMenu();
void aplicarMultiplesFiltros(Imagen *imagen);
void crearDirectorio(const char *ruta);
void extraerNombreArchivo(const char *rutaCompleta, char *nombreSinExtension);

void inicializarImagen(Imagen *imagen) {
    imagen->pixeles = NULL;
    imagen->ancho = 0;
    imagen->alto = 0;
    imagen->imagenCargada = 0;
    strcpy(imagen->nombreOriginal, "");
}

void extraerNombreArchivo(const char *rutaCompleta, char *nombreSinExtension) {
    const char *ultimaBarra = strrchr(rutaCompleta, '/');
    if (ultimaBarra == NULL) {
        ultimaBarra = strrchr(rutaCompleta, '\\');
    }
    if (ultimaBarra == NULL) {
        ultimaBarra = rutaCompleta;
    } else {
        ultimaBarra++;
    }
    
    strcpy(nombreSinExtension, ultimaBarra);
    
    char *ultimoPunto = strrchr(nombreSinExtension, '.');
    if (ultimoPunto != NULL) {
        *ultimoPunto = '\0';
    }
}

void crearDirectorio(const char *ruta) {
    char comando[1024];
    sprintf(comando, "mkdir \"%s\" 2>nul", ruta);
    system(comando);
}

int cargarImagenBMP(Imagen *imagen, const char *rutaArchivo) {
    char rutasIntento[5][512];
    sprintf(rutasIntento[0], "%s", rutaArchivo);
    sprintf(rutasIntento[1], "./%s", rutaArchivo);
    sprintf(rutasIntento[2], "../%s", rutaArchivo);
    
    const char *nombreSolo = strrchr(rutaArchivo, '\\');
    if (!nombreSolo) nombreSolo = strrchr(rutaArchivo, '/');
    if (!nombreSolo) nombreSolo = rutaArchivo;
    else nombreSolo++;
    
    sprintf(rutasIntento[3], "img/%s", nombreSolo);
    sprintf(rutasIntento[4], "img\\%s", nombreSolo);
    
    FILE *archivo = NULL;
    const char *rutaFinal = NULL;
    
    for (int i = 0; i < 5; i++) {
        archivo = fopen(rutasIntento[i], "rb");
        if (archivo) {
            rutaFinal = rutasIntento[i];
            printf("Archivo encontrado: %s\n", rutaFinal);
            break;
        }
    }
    
    if (!archivo) {
        printf("Error: No se pudo abrir el archivo BMP\n");
        printf("Intentando rutas:\n");
        for (int i = 0; i < 5; i++) {
            printf("  %s\n", rutasIntento[i]);
        }
        return 0;
    }
    
    unsigned char header[54];
    if (fread(header, 1, 54, archivo) != 54) {
        printf("Error: Archivo BMP invalido\n");
        fclose(archivo);
        return 0;
    }
    
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Error: No es un archivo BMP valido\n");
        fclose(archivo);
        return 0;
    }
    
    imagen->ancho = *(int*)&header[18];
    imagen->alto = *(int*)&header[22];
    int bitsPerPixel = *(short*)&header[28];
    
    if (bitsPerPixel != 24) {
        printf("Error: Solo se soportan BMP de 24 bits\n");
        fclose(archivo);
        return 0;
    }
    
    extraerNombreArchivo(rutaFinal, imagen->nombreOriginal);
    
    imagen->pixeles = (Pixel**)malloc(imagen->alto * sizeof(Pixel*));
    for (int i = 0; i < imagen->alto; i++) {
        imagen->pixeles[i] = (Pixel*)malloc(imagen->ancho * sizeof(Pixel));
    }
    
    int padding = (4 - (imagen->ancho * 3) % 4) % 4;
    
    for (int y = imagen->alto - 1; y >= 0; y--) {
        for (int x = 0; x < imagen->ancho; x++) {
            unsigned char bgr[3];
            if (fread(bgr, 1, 3, archivo) != 3) {
                printf("Error leyendo pixeles\n");
                liberarImagen(imagen);
                fclose(archivo);
                return 0;
            }
            imagen->pixeles[y][x].r = bgr[2];
            imagen->pixeles[y][x].g = bgr[1];
            imagen->pixeles[y][x].b = bgr[0];
        }
        fseek(archivo, padding, SEEK_CUR);
    }
    
    fclose(archivo);
    imagen->imagenCargada = 1;
    printf("Imagen BMP cargada: %dx%d pixeles\n", imagen->ancho, imagen->alto);
    return 1;
}

int guardarImagenBMP(Imagen *imagen, const char *rutaArchivo) {
    if (!imagen->imagenCargada) {
        printf("Error: No hay imagen cargada\n");
        return 0;
    }
    
    FILE *archivo = fopen(rutaArchivo, "wb");
    if (!archivo) {
        printf("Error: No se pudo crear %s\n", rutaArchivo);
        return 0;
    }
    
    int padding = (4 - (imagen->ancho * 3) % 4) % 4;
    int imageSize = (imagen->ancho * 3 + padding) * imagen->alto;
    int fileSize = 54 + imageSize;
    
    unsigned char header[54] = {0};
    
    header[0] = 'B';
    header[1] = 'M';
    *(int*)&header[2] = fileSize;
    *(int*)&header[10] = 54;
    *(int*)&header[14] = 40;
    *(int*)&header[18] = imagen->ancho;
    *(int*)&header[22] = imagen->alto;
    *(short*)&header[26] = 1;
    *(short*)&header[28] = 24;
    *(int*)&header[34] = imageSize;
    
    fwrite(header, 1, 54, archivo);
    
    for (int y = imagen->alto - 1; y >= 0; y--) {
        for (int x = 0; x < imagen->ancho; x++) {
            unsigned char bgr[3];
            bgr[0] = imagen->pixeles[y][x].b;
            bgr[1] = imagen->pixeles[y][x].g;
            bgr[2] = imagen->pixeles[y][x].r;
            fwrite(bgr, 1, 3, archivo);
        }
        for (int p = 0; p < padding; p++) {
            fputc(0, archivo);
        }
    }
    
    fclose(archivo);
    printf("Imagen guardada en: %s\n", rutaArchivo);
    return 1;
}

int guardarImagenEnCarpeta(Imagen *imagen, const char *nombreFiltro, const char *sufijo) {
    if (!imagen->imagenCargada) {
        printf("Error: No hay imagen cargada\n");
        return 0;
    }
    
    char rutaCarpetaFiltro[512];
    char rutaCarpetaC[512];
    sprintf(rutaCarpetaFiltro, "%s", nombreFiltro);
    sprintf(rutaCarpetaC, "%s\\c", nombreFiltro);
    
    crearDirectorio(rutaCarpetaFiltro);
    crearDirectorio(rutaCarpetaC);
    
    char nombreBMP[512];
    char rutaBMP[1024];
    sprintf(nombreBMP, "%s_%s%s.bmp", imagen->nombreOriginal, nombreFiltro, sufijo);
    sprintf(rutaBMP, "%s\\%s", rutaCarpetaC, nombreBMP);
    
    return guardarImagenBMP(imagen, rutaBMP);
}

Pixel obtenerPixel(Imagen *imagen, int x, int y) {
    Pixel pixelVacio = {0, 0, 0};
    if (x >= 0 && x < imagen->ancho && y >= 0 && y < imagen->alto) {
        return imagen->pixeles[y][x];
    }
    return pixelVacio;
}

void establecerPixel(Imagen *imagen, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x >= 0 && x < imagen->ancho && y >= 0 && y < imagen->alto) {
        imagen->pixeles[y][x].r = r;
        imagen->pixeles[y][x].g = g;
        imagen->pixeles[y][x].b = b;
    }
}

void liberarImagen(Imagen *imagen) {
    if (imagen->pixeles != NULL) {
        for (int i = 0; i < imagen->alto; i++) {
            if (imagen->pixeles[i] != NULL) {
                free(imagen->pixeles[i]);
            }
        }
        free(imagen->pixeles);
        imagen->pixeles = NULL;
    }
    imagen->imagenCargada = 0;
}

void aplicarFiltroGrises(Imagen *imagen) {
    printf("Aplicando filtro de escala de grises...\n");
    
    for (int y = 0; y < imagen->alto; y++) {
        for (int x = 0; x < imagen->ancho; x++) {
            Pixel pixel = obtenerPixel(imagen, x, y);
            unsigned char gris = (unsigned char)(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
            establecerPixel(imagen, x, y, gris, gris, gris);
        }
    }
    
    printf("Filtro de grises aplicado correctamente\n");
    guardarImagenEnCarpeta(imagen, "grises", "");
}

void aplicarFiltroInversion(Imagen *imagen) {
    printf("Aplicando filtro de inversion de colores...\n");
    
    for (int y = 0; y < imagen->alto; y++) {
        for (int x = 0; x < imagen->ancho; x++) {
            Pixel pixel = obtenerPixel(imagen, x, y);
            establecerPixel(imagen, x, y, 255 - pixel.r, 255 - pixel.g, 255 - pixel.b);
        }
    }
    
    printf("Filtro de inversion aplicado correctamente\n");
    guardarImagenEnCarpeta(imagen, "inversion_colores", "");
}

void aplicarFiltroBrillo(Imagen *imagen, int factorBrillo) {
    printf("Aplicando filtro de brillo (factor: %d)...\n", factorBrillo);
    
    for (int y = 0; y < imagen->alto; y++) {
        for (int x = 0; x < imagen->ancho; x++) {
            Pixel pixel = obtenerPixel(imagen, x, y);
            
            int rNuevo = (int)pixel.r + factorBrillo;
            int gNuevo = (int)pixel.g + factorBrillo;
            int bNuevo = (int)pixel.b + factorBrillo;
            
            if (rNuevo < 0) rNuevo = 0;
            if (rNuevo > 255) rNuevo = 255;
            if (gNuevo < 0) gNuevo = 0;
            if (gNuevo > 255) gNuevo = 255;
            if (bNuevo < 0) bNuevo = 0;
            if (bNuevo > 255) bNuevo = 255;
            
            establecerPixel(imagen, x, y, rNuevo, gNuevo, bNuevo);
        }
    }
    
    printf("Filtro de brillo aplicado correctamente\n");
    const char *carpeta = factorBrillo > 0 ? "brillo+50" : "brillo-50";
    guardarImagenEnCarpeta(imagen, carpeta, "");
}

void mostrarMenu() {
    printf("\n==================================================\n");
    printf("    EDITOR DE IMAGENES - C\n");
    printf("==================================================\n");
    printf("1. Cargar imagen BMP\n");
    printf("2. Aplicar filtro de escala de grises\n");
    printf("3. Aplicar filtro de inversion de colores\n");
    printf("4. Aplicar filtro de brillo (+50)\n");
    printf("5. Aplicar filtro de brillo (-50)\n");
    printf("6. Aplicar multiples filtros\n");
    printf("7. Guardar imagen BMP\n");
    printf("8. Salir\n");
    printf("==================================================\n");
}

void aplicarMultiplesFiltros(Imagen *imagen) {
    if (!imagen->imagenCargada) {
        printf("Primero debe cargar una imagen\n");
        return;
    }
    
    printf("\nSeleccione los filtros a aplicar (numeros separados por espacios):\n");
    printf("1 - Escala de grises\n");
    printf("2 - Inversion de colores\n");
    printf("3 - Brillo (+50)\n");
    printf("4 - Brillo (-50)\n");
    
    printf("Filtros a aplicar: ");
    char linea[256];
    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        printf("Error al leer entrada\n");
        return;
    }
    
    char filtrosAplicados[512] = "";
    char *token = strtok(linea, " \n\t");
    int primerFiltro = 1;
    
    while (token != NULL) {
        int filtroNum = atoi(token);
        
        switch (filtroNum) {
            case 1:
                aplicarFiltroGrises(imagen);
                if (!primerFiltro) strcat(filtrosAplicados, "_");
                strcat(filtrosAplicados, "grises");
                primerFiltro = 0;
                break;
            case 2:
                aplicarFiltroInversion(imagen);
                if (!primerFiltro) strcat(filtrosAplicados, "_");
                strcat(filtrosAplicados, "inversion");
                primerFiltro = 0;
                break;
            case 3:
                aplicarFiltroBrillo(imagen, 50);
                if (!primerFiltro) strcat(filtrosAplicados, "_");
                strcat(filtrosAplicados, "brillo+50");
                primerFiltro = 0;
                break;
            case 4:
                aplicarFiltroBrillo(imagen, -50);
                if (!primerFiltro) strcat(filtrosAplicados, "_");
                strcat(filtrosAplicados, "brillo-50");
                primerFiltro = 0;
                break;
            default:
                if (filtroNum != 0) {
                    printf("Filtro '%d' no valido\n", filtroNum);
                }
        }
        
        token = strtok(NULL, " \n\t");
    }
    
    if (strlen(filtrosAplicados) > 0) {
        char sufijo[600];
        sprintf(sufijo, "_%s", filtrosAplicados);
        guardarImagenEnCarpeta(imagen, "multiples_filtros", sufijo);
    }
}

int main() {
    Imagen imagen;
    inicializarImagen(&imagen);
    
    printf("Editor de Imagenes BMP\n\n");
    
    int opcion;
    char ruta[512];
    
    while (1) {
        mostrarMenu();
        printf("Seleccione una opcion: ");
        
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada invalida\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        switch (opcion) {
            case 1:
                printf("Ingrese la ruta del archivo BMP: ");
                if (fgets(ruta, sizeof(ruta), stdin) != NULL) {
                    size_t len = strlen(ruta);
                    if (len > 0 && ruta[len-1] == '\n') {
                        ruta[len-1] = '\0';
                    }
                    cargarImagenBMP(&imagen, ruta);
                }
                break;
            
            case 2:
                if (imagen.imagenCargada) {
                    aplicarFiltroGrises(&imagen);
                } else {
                    printf("Primero debe cargar una imagen\n");
                }
                break;
            
            case 3:
                if (imagen.imagenCargada) {
                    aplicarFiltroInversion(&imagen);
                } else {
                    printf("Primero debe cargar una imagen\n");
                }
                break;
            
            case 4:
                if (imagen.imagenCargada) {
                    aplicarFiltroBrillo(&imagen, 50);
                } else {
                    printf("Primero debe cargar una imagen\n");
                }
                break;
            
            case 5:
                if (imagen.imagenCargada) {
                    aplicarFiltroBrillo(&imagen, -50);
                } else {
                    printf("Primero debe cargar una imagen\n");
                }
                break;
            
            case 6:
                aplicarMultiplesFiltros(&imagen);
                break;
            
            case 7:
                if (imagen.imagenCargada) {
                    printf("Ingrese nombre para guardar: ");
                    if (fgets(ruta, sizeof(ruta), stdin) != NULL) {
                        size_t len = strlen(ruta);
                        if (len > 0 && ruta[len-1] == '\n') {
                            ruta[len-1] = '\0';
                        }
                        guardarImagenBMP(&imagen, ruta);
                    }
                } else {
                    printf("No hay imagen para guardar\n");
                }
                break;
            
            case 8:
                printf("Programa terminado\n");
                liberarImagen(&imagen);
                return 0;
            
            default:
                printf("Opcion no valida\n");
        }
    }
    
    return 0;
}