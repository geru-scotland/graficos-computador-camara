#include "headers/shared_defines.h"

/***********************************************************************
 *                                                                     *
 *                          DEBUG Y LOGGING                            *
 *                                                                     *
 ***********************************************************************/

/***********************************************************************
 * Este archivo contiene funciones para el debugging y el registro,
 * incluyendo el mostrar datos de matrices, transformaciones, cámara,
 * banderas/flags de escena activas y vectores. Estas funciones son de
 * bastante utilidad para el seguimiento y análisis de datos en tiempo
 * de ejecución.
 ***********************************************************************/


/**
 * Imprime una matriz 4x4 pasada como un arreglo bidimensional.
 *
 * @param matrix Matriz 4x4 que se imprimirá.
 */
void print_matrix(double matrix[4][4]) {
    printf("Matrix:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * Imprime todas las matrices de transformación almacenadas en la lista
 * de matrices.
 *
 * @param sel_ptr Puntero al objeto que contiene la lista de matrices.
 */
void print_transformations(const triobj* sel_ptr) {
    // Verifica si hay matrices en la lista
    if (!sel_ptr || !sel_ptr->mptr) {
        printf("\nNo hay transformaciones para mostrar.\n");
        return;
    }

    mlist *aux = sel_ptr->mptr;
    int count = 0;

    while (aux) {
        count++;
        aux = aux->hptr;
    }

    mlist *matrices[count];
    aux = sel_ptr->mptr;
    for (int i = 0; i < count; i++) {
        matrices[i] = aux;
        aux = aux->hptr;
    }

    // Ahora imprimimos las matrices de forma horizontal
    printf("\n\n Historial de Transformaciones: \n\n");

    // Para cada fila de las matrices
    for (int row = 0; row < 4; row++) {
        // Imprimimos la fila de cada matriz en orden, empezando por la más reciente
        for (int i = 0; i < count; i++) {
            printf("[");
            for (int col = 0; col < 4; col++) {
                printf("%.2f", matrices[i]->m[row * 4 + col]);
                if (col != 3) {
                    printf(", ");
                }
            }
            printf("]");
            if (i != count - 1) {
                printf(" | ");
            }
        }
        printf("\n");
    }
}

/**
 * Imprime los datos de una cámara, incluyendo posición, orientación y matriz de vista.
 *
 * @param camera Puntero a la estructura de la cámara que contiene los datos a imprimir.
 */
void print_camera_data(const Camera* camera) {
    printf("\n\n CAMERA DATA \n\n");
    printf("Eye Position: (%f, %f, %f)\n", camera->eye_position.x, camera->eye_position.y, camera->eye_position.z);
    printf("Look At: (%f, %f, %f)\n", camera->look_at.x, camera->look_at.y, camera->look_at.z);
    printf("Vector Up: (%f, %f, %f)\n", camera->vector_up.x, camera->vector_up.y, camera->vector_up.z);
    printf("Vector Forward: (%f, %f, %f)\n", camera->vector_forward.x, camera->vector_forward.y, camera->vector_forward.z);
    printf("Vector Right: (%f, %f, %f)\n", camera->vector_right.x, camera->vector_right.y, camera->vector_right.z);

    printf("View Matrix:\n");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%f ", camera->view->matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * Imprime los estados / flags activos en la máscara de escena,
 * indicando modos y ejes seleccionados.
 *
 * @param scene_mask Puntero a la máscara de escena a imprimir.
 */
void print_scene_mask(const int* scene_mask) {
    printf("\n ################## \n");
    if (*scene_mask & MODO_ESCALADO) {
        printf("MODO_ESCALADO\n");
    }
    if (*scene_mask & MODO_ROTACION) {
        printf("MODO_ROTACION\n");
    }
    if (*scene_mask & MODO_TRASLACION) {
        printf("MODO_TRASLACION\n");
    }
    if (*scene_mask & EJE_LOCAL) {
        printf("EJE_LOCAL\n");
    }
    if (*scene_mask & EJE_X_POSITIVO) {
        printf("EJE_X_POSITIVO\n");
    }
    if (*scene_mask & EJE_X_NEGATIVO) {
        printf("EJE_X_NEGATIVO\n");
    }
    if (*scene_mask & EJE_Y_POSITIVO) {
        printf("EJE_Y_POSITIVO\n");
    }
    if (*scene_mask & EJE_Y_NEGATIVO) {
        printf("EJE_Y_NEGATIVO\n");
    }
    if (*scene_mask & EJE_Z_POSITIVO) {
        printf("EJE_Z_POSITIVO\n");
    }
    if (*scene_mask & EJE_Z_NEGATIVO) {
        printf("EJE_Z_NEGATIVO\n");
    }
    if (*scene_mask & MODO_CAMARA) {
        printf("MODO_CAMARA\n");
    }
    if (*scene_mask & MODO_OBJETO) {
        printf("MODO_OBJETO\n");
    }
    if (*scene_mask & CAMARA_ANALISIS) {
        printf("CAMARA_ANALISIS\n");
    }
    if (*scene_mask & CAMARA_VUELO) {
        printf("CAMARA_VUELO\n");
    }
    printf("################## \n");
}

/**
 * Imprime las componentes de un vector 3D.
 *
 * @param vector Puntero al vector 3D a imprimir.
 */
void print_vector(const Vector3* vector) {
    printf("Vector: (x: %.2f, y: %.2f, z: %.2f)\n", vector->x, vector->y, vector->z);
}