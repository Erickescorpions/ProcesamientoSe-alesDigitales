#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/wait.h> 


#define N 1000
#define PI M_PI
#define SNR 3

float* genera_sen(int longitud, int amplitud, int frecuencia);
float* genera_cos(int longitud, int amplitud, int frecuencia);
void fn_a_archivo(float* fn, char* nombre_archivo, int longitud);
void fn_entero_a_archivo(int* fn, char* nombre_archivo, int longitud);
float calcular_potencia_fn(float* fn, int longitud);
float calcular_snr(float potencia_ruido, float potencia_fn);
void generar_ruido_en_fn(float* fn, int longitud);
float* obtener_submuestras(float* fn, int periodo, int frecuencia_muestreo);
int* cuantizacion(float* x, int longitud, int qi, int redondeo, int amplitud_original);


int main() {
    int numero_equipo = 2;
    fputs("Ingresa tu numero de equipo: ", stdout);
    scanf("%d", &numero_equipo);

    int amplitud = 2 * numero_equipo;
    int frecuencia = 2 * numero_equipo;

    float* fn = NULL;
    int razon_muestreo;
    int qi1 = 0;
    int qi2 = 0;

    if(numero_equipo % 2 == 0) { // par 
        fn = genera_cos(N, amplitud, frecuencia);
        razon_muestreo = 2;
        qi1 = 5;
        qi2 = 12;
    } else { // impar
        fn = genera_sen(N, amplitud, frecuencia);
        razon_muestreo = 3;
        qi1 = 6;
        qi2 = 13; 
    }

    generar_ruido_en_fn(fn, N);
    fn_a_archivo(fn, "fn.dat", N);
    
    int frecuencia_muestreo = razon_muestreo * numero_equipo;

    float* submuestras = obtener_submuestras(fn, N, frecuencia_muestreo);
    fn_a_archivo(submuestras, "submuestras.dat", N);

    // cuantizamos por redondeo 5 bits
    int* cuantizada_redondeo5 = cuantizacion(fn, N, qi1, 1, amplitud);
    // cuantizamos por truncamiento 5 bits
    int* cuantizada_truncamiento5 = cuantizacion(fn, N, qi1, 0, amplitud);

    fn_entero_a_archivo(cuantizada_redondeo5, "redondeo5.dat", N);
    fn_entero_a_archivo(cuantizada_truncamiento5, "truncamiento5.dat", N);

    // cuantizamos por redondeo 12 bits
    int* cuantizada_redondeo12 = cuantizacion(fn, N, qi2, 1, amplitud);
    // cuantizamos por truncamiento 12 bits
    int* cuantizada_truncamiento12 = cuantizacion(fn, N, qi2, 0, amplitud);

    fn_entero_a_archivo(cuantizada_redondeo12, "redondeo12.dat", N);
    fn_entero_a_archivo(cuantizada_truncamiento12, "truncamiento12.dat", N);

    // creamos un proceso hijo del programa para ejecutar por separado gnuplot
    if (fork() == 0) {
        execlp("gnuplot", "gnuplot", "-p", "grafica.gp", NULL);
        perror("Error al ejecutar Gnuplot para grafica.gp");
        exit(1);
    }

    // Segunda llamada a Gnuplot
    if (fork() == 0) {
        // Este es el proceso hijo
        execlp("gnuplot", "gnuplot", "-p", "cuantizacion.gp", NULL);
        perror("Error al ejecutar Gnuplot para cuantizacion.gp");
        exit(1);
    }

    // Esperar a que ambos procesos hijos terminen
    wait(NULL);
    wait(NULL);

    free(fn);

    return 0;
}


float* genera_sen(int longitud, int amplitud, int frecuencia) {
    float* seno = malloc(longitud * sizeof(float));
    for(int n = 0; n < longitud; n++) {
        seno[n] = amplitud * sin(2 * PI * frecuencia * n / N);
    }

    return seno;
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
        indice_submuestreo +=1;

        if(indice_submuestreo==frecuencia_muestreo){
            submuestreo[n] = fn[n];
            indice_submuestreo = 0;
        } else{
            submuestreo[n] = 0;
        }


    }

    return submuestreo;
}


int* cuantizacion(float* x, int longitud, int qi, int redondeo, int amplitud_original) {
    int* senial_cuantizada = malloc(longitud * sizeof(int)); // Cambiado a int16_t

    float aumento_por_redondeo = redondeo == 1 ? 0.5 : 0.0; // si se activa la bandera de redonde, se suma 0.5
    int qi_sin_signo = qi - 1;

    int rango_decimal = pow(2, qi_sin_signo) - 1;
    printf("El rango para qi=%d bits es: %d \n", qi, rango_decimal);

    float factor_de_normalizacion = 1.0 / amplitud_original;
    printf("Factor de normalizacion = 1.0 / %d = %f\n", amplitud_original, factor_de_normalizacion);

    for(int i = 0; i < N; i++) {
        // Normalizamos la muestra
        float muestra_normalizada = x[i] * factor_de_normalizacion;
        
        // elevamos a 2^qi el numero y lo pasamos a entero
        int parte_decimal = floor((muestra_normalizada * pow(2, qi_sin_signo)) + aumento_por_redondeo);
        
        // revisamos que la parte decimal no se pase de su rango
        if(parte_decimal < -rango_decimal || parte_decimal > rango_decimal) {
            if(parte_decimal < 0) {
                parte_decimal = -rango_decimal; // Corregir rango si es menor que el mínimo
            } else {
                parte_decimal = rango_decimal; // Corregir rango si es mayor que el máximo
            }
        } 

        senial_cuantizada[i] = parte_decimal;
    }

    return senial_cuantizada;
}