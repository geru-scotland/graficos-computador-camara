#ifndef SHARED_DEFINES_H
#define SHARED_DEFINES_H

#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>

/***********************************************************************
 * Este archivo de cabecera, define una serie de estructuras de datos,
 * constantes y macros utilizadas en todo el proyecto.
 *
 * Incluye configuraciones para transformaciones básicas como rotación,
 * traslación y escalado, además de definir modos de transformación y
 * direcciones de ejes. También establece parámetros para diferentes modos
 * de cámara y proyecciones.
 *
 * Incluye estructuras para puntos, triángulos, objetos de triángulos, vectores 3D,
 * cámaras y sus matrices de vista.
 *
 * También se definen constantes matemáticas y parámetros de proyección.
 ***********************************************************************/

// Configuración de transformaciones base.
#define ROTACION_ANGULO        10.0
#define ROTACION_ANGULO_CAMARA 1.0
#define TRASLACION_PIXELS      10.0
#define ESCALADO_ESCALA        2.0

// Modos de transformación existentes
#define MODO_ESCALADO      (1 << 0)  // 0b00000001
#define MODO_ROTACION      (1 << 1)  // 0b00000010
#define MODO_TRASLACION    (1 << 2)  // 0b00000100
#define EJE_LOCAL          (1 << 9)  // 0b0000000100000000

// Direcciones de los ejes existentes
#define EJE_X_POSITIVO  (1 << 3)  // 0b00001000
#define EJE_X_NEGATIVO  (1 << 4)  // 0b00010000
#define EJE_Y_POSITIVO  (1 << 5)  // 0b00100000
#define EJE_Y_NEGATIVO  (1 << 6)  // 0b01000000
#define EJE_Z_POSITIVO  (1 << 7)  // 0b10000000
#define EJE_Z_NEGATIVO  (1 << 8)  // 0b0000000100000000

// Modos de cámara
#define MODO_CAMARA     (1 << 10) // 0b0000010000000000
#define MODO_OBJETO     (1 << 11) // 0b0000100000000000
#define CAMARA_ANALISIS (1 << 12) // 0b0001000000000000
#define CAMARA_VUELO    (1 << 13) // 0b0010000000000000

// Proyecciones
#define PROJECTION_PERSPECTIVE  (1 << 14)  // 0b0100000000000000
#define PROJECTION_ORTOGRAPHIC  (1 << 15)  // 0b1000000000000000

// Mostrar los vectores normales
#define NORMAL_VECTORS          (1 << 16)  // 0b000000010000000000000000
#define BACK_CULLING            (1 << 17)

#define EJE_LIMPIAR_MASK_EJES (EJE_X_POSITIVO | EJE_X_NEGATIVO | EJE_Y_POSITIVO | EJE_Y_NEGATIVO | EJE_Z_POSITIVO | EJE_Z_NEGATIVO)
#define EJE_LIMPIAR_MASK_TRANSFORMACION (MODO_ESCALADO | MODO_ROTACION | MODO_TRASLACION)
#define EJE_LIMPIAR_MASK_CAMARA (MODO_CAMARA | MODO_OBJETO | CAMARA_ANALISIS | CAMARA_VUELO)

#define PUNTO_INICIAL 0
#define PUNTO_FINAL 1

#define PERSPECTIVE_FACTOR 500

typedef struct mlist
{
    double m[16];
    struct mlist *hptr;
} mlist;


typedef struct Punto
{
    float x, y, z, u, v, w;
} Punto;

typedef struct Triangulo
{
    Punto p1,p2,p3;
} Triangulo;

typedef struct triobj
{
    Triangulo *triptr;
    int num_triangles;
    mlist *mptr;
    struct triobj *hptr;
} triobj;

// Estructura para un vector de tres componentes.
typedef struct {
    float x;
    float y;
    float z;
} Vector3;

typedef struct {
    int x;
    int y;
    int z;
} Vector3Index;

typedef struct {
    double matrix[4][4];
} View;

typedef struct {
    Vector3 eye_position, look_at;
    Vector3 vector_forward, vector_right, vector_up;

    View* view;
} Camera;

typedef struct {
    const Vector3 EYE_POSITION;
    const Vector3 UP_VECTOR;
    const Vector3 LOOK_AT;
} CameraConst;

typedef struct {
    float near_plane;
    float far_plane;
    float left;
    float right;
    float bottom;
    float top;
} ProjectionConst;

#define PI 3.14159265358979323846

#endif // SHARED_DEFINES_H