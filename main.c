#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define N 1000
#define PI M_PI
#define SNR 3

float* genera_sen(int longitud, int amplitud, int frecuencia) {
    float* sen = malloc(longitud * sizeof(float));
    for(int n = 0; n < longitud; n++) {
        sen[n] = amplitud * sin(2 * PI * frecuencia * n / N);
    }

    return sen;
}

float* genera_cos(int longitud, int amplitud, int frecuencia) {
    float* coseno = malloc(longitud * sizeof(float));
    for(int n = 0; n < longitud; n++) {
        coseno[n] = amplitud * cos(2 * PI * frecuencia * n / N);
    }

    return coseno;
}

void fn_a_archivo(float* fn, char* nombre_archivo, int longitud) {
    FILE* archivo = fopen(nombre_archivo, "w");

    for(int i = 0; i < longitud; i++) {
        fprintf(archivo, "%.4f\n", fn[i]);
        //printf("%f - %d\n", fn[i], i);
    }

    fclose(archivo);
}

void fn_entero_a_archivo(int* fn, char* nombre_archivo, int longitud) {
    FILE* archivo = fopen(nombre_archivo, "w");

    for(int i = 0; i < longitud; i++) {
        fprintf(archivo, "%d\n", fn[i]);
        //printf("%f - %d\n", fn[i], i);
    }

    fclose(archivo);
}

float calcular_potencia_fn(float* fn, int longitud) {
    float potencia_fn = 0.0;

    for(int i = 0; i < longitud; i++) {
        potencia_fn += fn[i] * fn[i];
    }

    potencia_fn = potencia_fn / longitud;
    printf("La potencia de la senal es: %f\n", potencia_fn);

    return potencia_fn;
}

float calcular_snr(float potencia_ruido, float potencia_fn) {
    return 10 * log10(potencia_fn / potencia_ruido);
}

float calcular_potencia_ruido(float snr, float potencia_fn) {
    float potencia_ruido = 1; 
    float snr_calculado = calcular_snr(potencia_ruido, potencia_fn);
    while(snr_calculado >= snr) {
        potencia_ruido += 0.01;
        snr_calculado = calcular_snr(potencia_ruido, potencia_fn);
    }

    printf("La potencia del ruido es: %f\n", potencia_ruido);
    printf("El snr calculado es: %f \n", snr_calculado);
    return potencia_ruido;
}

// SNR = 10 * log10(Potencia de la senal / Potencia del ruido)
// No conocemos la potencia del ruido
// SNR < 3db
void generar_ruido_en_fn(float* fn, int longitud) {

    float potencia_fn = calcular_potencia_fn(fn, longitud);

    // Calculamos la potencia del ruido
    float potencia_ruido = calcular_potencia_ruido(SNR, potencia_fn);
    float desviacion_estandar_ruido = sqrt(potencia_ruido);
    
    srand(time(NULL));
    // Aplicamos el ruido a la senal 
    for(int i = 0; i < longitud; i++) {
        fn[i] = fn[i] + desviacion_estandar_ruido * ((float)rand() / RAND_MAX - 0.5);
    }
}

float* obtener_submuestras(float* fn, int periodo, int frecuencia_muestreo) {
    float* submuestreo = malloc(periodo * sizeof(float));

    // la razon de muestreo es la frecuencia con la que queremos muestrear
    // periodo = 1 / frecuencia
    // normalizamos el periodo respecto al periodo de la senal original
    int periodo_submuestreo = (1.0 / frecuencia_muestreo) * N;
    int indice_submuestreo = 0;

    for(int n = 0; n < periodo; n++) {
        if(n % (int )periodo_submuestreo == 0) {
            submuestreo[n] = fn[n];
        } else {
            submuestreo[n] = 0;
        }
    }

    return submuestreo;
}

int* cuantizacion(float* x, int longitud, int qe, int qi, int redondeo) {
    int* senial_cuantizada = malloc(longitud * sizeof(int));
    
    int tam_palabra = qe + qi;
    float aumento_por_redondeo = redondeo == 1 ? 0.5 : 0.0; 

    for(int i = 0; i < longitud; i++) {
        int parte_entera = (int) x[i] << (tam_palabra - qi);
        int parte_decimal = floor((pow(x[i], qi) + aumento_por_redondeo));
        senial_cuantizada[i] = parte_entera + parte_decimal;
    }

    return senial_cuantizada;
}

int main() {
    int numero_equipo = 2;
    fputs("Ingresa tu numero de equipo: ", stdout);
    scanf("%d", &numero_equipo);

    int amplitud = 2 * numero_equipo;
    int frecuencia = 2 * numero_equipo;

    float* fn = NULL;
    int razon_muestreo;
    int qe = 0;
    int qi = 0;

    if(numero_equipo % 2 == 0) { // par 
        fn = genera_cos(N, amplitud, frecuencia);
        razon_muestreo = 2;
        qe = 5;
        qi = 12;
    } else { // impar
        fn = genera_sen(N, amplitud, frecuencia);
        razon_muestreo = 3;
        qe = 6;
        qi = 13; 
    }

    generar_ruido_en_fn(fn, N);
    fn_a_archivo(fn, "fn.dat", N);
    
    int frecuencia_muestreo = razon_muestreo * numero_equipo;

    float* submuestras = obtener_submuestras(fn, N, frecuencia_muestreo);
    fn_a_archivo(submuestras, "submuestras.dat", N);

    // cuantizamos por redondeo
    int* cuantizada_redondeo = cuantizacion(fn, N, qe, qi, 1);
    // cuantizamos por truncamiento
    int* cuantizada_truncamiento = cuantizacion(fn, N, qe, qi, 0);

    fn_entero_a_archivo(cuantizada_redondeo, "redondeo.dat", N);
    fn_entero_a_archivo(cuantizada_truncamiento, "truncamiento.dat", N);

    int ret = execlp("gnuplot", "gnuplot", "-p", "grafica.gp", NULL);
        
    if (ret == -1) {
        perror("Error al ejecutar Gnuplot");
        exit(1);
    }

    free(fn);

    return 0;
}