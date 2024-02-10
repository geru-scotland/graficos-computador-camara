#include "headers/shared_defines.h"


/***********************************************************************
 *                                                                     *
 *      OPERACIONES MATRICIALES, VECTORIALES Y DE ORDENACION           *
 *                                                                     *
 ***********************************************************************/

/***********************************************************************
 * Este archivo implementa operaciones matriciales, vectoriales y de
 * ordenación utilizadas en el procesamiento de diferentes areas en nuestra
 * aplicación. Incluye funciones para la multiplicación de matrices,
 * operaciones con vectores como suma, producto cruz y normalización, así
 * como funciones para ordenar puntos y calcular centroides.
 ***********************************************************************/

/**
 * Multiplica una matriz de transformación por un punto en coordenadas homogéneas,
 * actualizando las coordenadas del punto resultante.
 * @param pptr Puntero al punto resultante.
 * @param matriz_trans Matriz de transformación.
 * @param p Punto original a transformar.
 */
void mxp(Punto *pptr, double matriz_trans[16], Punto p)
{
    // Coordenadas homogéneas del punto, ojo con el 1 que hace que persistan las traslaciones.
    double vec[4] = {p.x, p.y, p.z, 1.0};

    // Resultado de la multiplicación
    double res[4] = {0.0, 0.0, 0.0, 0.0};

#ifdef DEBUG
    // Imprime la matriz y el punto
    printf("Matriz:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.2f\t", matriz_trans[i * 4 + j]);
        }
        printf("\n");
    }
    printf("\nVector Punto: [%.2f, %.2f, %.2f, 1.0]\n\n", p.x, p.y, p.z);
#endif

    // Multiplicar matriz por vector
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res[i] += matriz_trans[i * 4 + j] * vec[j];
        }
    }

    // Guardo las coordenadas resultantes en pptr
    pptr->x = res[0];
    pptr->y = res[1];
    pptr->z = res[2];
    pptr->w = res[3];

#ifdef DEBUG
    printf("Resultado: [%.2f, %.2f, %.2f]\n", pptr->x, pptr->y, pptr->z);
#endif

    // Copio coordenadas de textura.
    pptr->u = p.u;
    pptr->v = p.v;
}

/**
 * Realiza la multiplicación de dos matrices 4x4.
 * @param a Matriz A.
 * @param b Matriz B.
 * @param result Matriz resultante de la multiplicación de A y B.
 */
void matrix_multiplication(double a[4][4], double b[4][4], double result[4][4]) {
    // Init, creo una matriz temporal para almacenar el resultado
    double temp[4][4] = {0};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                temp[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    // Trasladar el resultado de la matriz temporal a la matriz de resultado
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = temp[i][j];
        }
    }
}

/**
 * Crea y devuelve un nuevo vector tridimensional. Encapsulo ésta manera de crear
 * vectores, por comodidad.
 * @param x Componente en el eje x.
 * @param y Componente en el eje y.
 * @param z Componente en el eje z.
 * @return Vector3 estructura del vector creado.
 */
Vector3 vector3(float x, float y, float z){
    Vector3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

/**
 * Calcula y devuelve la resta de dos vectores tridimensionales.
 * @param v1 Primer vector.
 * @param v2 Segundo vector.
 * @return Vector3 resultado de la resta v2 - v1.
 */
Vector3 vector3_substract(Vector3 v1, Vector3 v2){
    Vector3 v;
    v.x = v2.x - v1.x;
    v.y = v2.y - v1.y;
    v.z = v2.z - v1.z;
    return v;
}

/**
 * Calcula el producto cruz o cross product entre dos vectores tridimensionales.
 * @param v1 Primer vector.
 * @param v2 Segundo vector.
 * @return Vector3 resultado del producto cruz.
 */
Vector3 vector3_cross_product(Vector3 v1, Vector3 v2) {
    Vector3 v;

    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;

    return v;
}

/**
 * Calcula el producto punto o dot product entre dos vectores tridimensionales.
 * @param v1 Primer vector.
 * @param v2 Segundo vector.
 * @return float resultado del producto punto.
 */
float vector3_dot_product(Vector3 v1, Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/**
 * Normaliza un vector tridimensional, de tres componentes.
 * @param vector Vector a normalizar.
 * @return Vector3 vector normalizado.
 */
Vector3 normalizar_vector(Vector3 vector) {
    // Calculamos el módulo primero.
    float modulo = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    // Si 0, fuera, lo evito.
    if (modulo == 0.0f) {
        modulo = 1.0f;
    }

    // Crear un nuevo vector para el resultado. Por valor, no merece la pena
    // memoria dinámica para esto.
    Vector3 normalized = {
            vector.x / modulo,
            vector.y / modulo,
            vector.z / modulo
    };

    return normalized;
}

/**
 * Copia las coordenadas y datos de textura de un punto a otro.
 * @param pptr Puntero al punto destino.
 * @param p Puntero al punto fuente.
 */
void copiar_punto(Punto* pptr, Punto* p)
{
    pptr->x = p->x;
    pptr->y = p->y;
    pptr->z = p->z;
    pptr->u = p->u;
    pptr->v = p->v;
}

/**
 * @brief Ordena tres puntos basándose en sus coordenadas y.
 *
 * Esta función reordena los puntos de entrada de manera que, después de la llamada,
 * p1 apunta al punto con la coordenada y más grande, p2 al siguiente y p3 al más pequeño.
 *
 * @param p1 Puntero al primer punto a ordenar.
 * @param p2 Puntero al segundo punto a ordenar.
 * @param p3 Puntero al tercer punto a ordenar.
 */
void ordenar_puntos(Punto *p1, Punto *p2, Punto *p3) {
    Punto temp;
    double m[16] = {0}; // Matriz de ceros como placeholder.

    // Comparar p1 con p2
    if(p1->y < p2->y) {
        copiar_punto(&temp, p1);
        copiar_punto(p1, p2);
        copiar_punto(p2, &temp);
    }

    // Comparar p1 con p3
    if(p1->y < p3->y) {
        copiar_punto(&temp, p1);
        copiar_punto(p1, p3);
        copiar_punto(p3, &temp);
    }

    // Comparar p2 con p3
    if(p2->y < p3->y) {
        copiar_punto(&temp, p2);
        copiar_punto(p2, p3);
        copiar_punto(p3, &temp);
    }
}
/**
 * Ordena tres puntos en función de su coordenada x (para corregir lo indicando
 * en el feedback de la primera entrega; discretización - triángulos con 3 vértices
 * a la misma altura).
 * @param p1 Primer punto.
 * @param p2 Segundo punto.
 * @param p3 Tercer punto.
 */
void ordenar_puntos_x(Punto *p1, Punto *p2, Punto *p3) {
    Punto temp;

    // Comparar p1 con p2
    if(p1->x > p2->x) {
        temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }

    // Comparar p1 con p3
    if(p1->x > p3->x) {
        temp = *p1;
        *p1 = *p3;
        *p3 = temp;
    }

    // Comparar p2 con p3
    if(p2->x > p3->x) {
        temp = *p2;
        *p2 = *p3;
        *p3 = temp;
    }
}

/**
 * @brief Realiza una interpolación lineal entre dos puntos para calcular las coordenadas del punto de corte.
 *
 * Esta función realiza una interpolación lineal basándose en las coordenadas de dos puntos conocidos.
 * Se utiliza para calcular las coordenadas del punto de corte, especialmente útil cuando sólo se conocen
 * las coordenadas de los vértices en la textura y se necesita obtener las coordenadas de los puntos intermedios.
 * Además, asegura una proporcionalidad en el mapeo de la textura, manteniendo una progresión adecuada en
 * la coordenada "y" que controla la rasterización.
 *
 * @param p1 Puntero al primer punto conocido para la interpolación lineal.
 * @param p2 Puntero al segundo punto conocido para la interpolación lineal.
 * @param punto_corte Puntero al punto de corte donde se almacenarán las coordenadas calculadas resultantes.
 */
void interpolacion_lineal(Punto *p1, Punto *p2, Punto *punto_corte) {
    punto_corte->x = p1->x + ((punto_corte->y - p1->y)*(p2->x - p1->x))/(p2->y - p1->y);

    /*
      Para mantener proporcionalidad, conforme se va modificando la coordenada "y" que indica o controla
      rasterización, quiero mantener esa misma progresión en el mapeo de la textura.
      Es decir, la coordenada y hace de controlador, para que tanto en el espacio/lienzo como en
      la textura, se esté procesando la misma cantidad o proporción. Esto es para que cuadren las dimensiones.
    */
    punto_corte->u = p1->u + ((punto_corte->y - p1->y)*(p2->u - p1->u))/(p2->y - p1->y);
    punto_corte->v = p1->v + ((punto_corte->y - p1->y)*(p2->v - p1->v))/(p2->y - p1->y);
}

/**
 * Calcula el centroide de un objeto tridimensional.
 * @param obj Objeto para el cual se calcula el centroide.
 * @return Punto centroide del objeto.
 */
Punto calcular_centroide(triobj* obj) {
    Punto centro = {0, 0, 0};
    int total_puntos = 0;

    for (int i = 0; i < obj->num_triangles; i++) {
        centro.x += obj->triptr[i].p1.x + obj->triptr[i].p2.x + obj->triptr[i].p3.x;
        centro.y += obj->triptr[i].p1.y + obj->triptr[i].p2.y + obj->triptr[i].p3.y;
        centro.z += obj->triptr[i].p1.z + obj->triptr[i].p2.z + obj->triptr[i].p3.z;
        total_puntos += 3;
    }

    centro.x /= total_puntos;
    centro.y /= total_puntos;
    centro.z /= total_puntos;

    return centro;
}

/**
 * Convierte un punto en un vector tridimensional. Esta función es útil para
 * transformar puntos en vectores para realizar operaciones vectoriales. Un
 * poco por comodidad y orden.
 * @param point Punto a convertir.
 * @param vector Puntero al Vector3 resultante.
 */
void point_to_vector(Punto point, Vector3* vector){
    Vector3 v = vector3(point.x, point.y, point.z);
    vector->x = v.x;
    vector->y = v.y;
    vector->z = v.z;
}

/**
 * Escala las coordenadas de un punto por un factor dado. La utilizo primordialmente
 * en operaciones de perspectiva para ajustar la escala de un punto en el espacio 3D.
 * @param punto Puntero al punto a escalar.
 * @param factor Factor de escala.
 */
void scale_point(Punto* punto, const float factor) {
    punto->x *= factor;
    punto->y *= factor;
    punto->z *= factor;
}

/**
 * Aplica corrección de profundidad de perspectiva a los vértices de un triángulo.
 * Básicamente ajusta las coordenadas de los vértices del triángulo basándose en
 * su componente 'w'.
 * @param triangulo Puntero al triángulo a modificar.
 * @param p1, p2, p3 Puntos que definen los vértices del triángulo.
 */
void apply_perspective_depth(Triangulo* triangulo, Punto* p1, Punto* p2, Punto* p3) {
    triangulo->p1.x = p1->x / p1->w;
    triangulo->p1.y = p1->y / p1->w;
    triangulo->p1.z = p1->z / p1->w;

    triangulo->p2.x = p2->x / p2->w;
    triangulo->p2.y = p2->y / p2->w;
    triangulo->p2.z = p2->z / p2->w;

    triangulo->p3.x = p3->x / p3->w;
    triangulo->p3.y = p3->y / p3->w;
    triangulo->p3.z = p3->z / p3->w;
}

/**
 * Calcula el centroide o baricentro de un triángulo. Hace la media de las
 * coordenadas de los tres vértices del triángulo para encontrar su punto central.
 * Por ahora la uso únicamente para calcular los vectores normales, el centroide
 * será el punto de nacimiento del vector normal.
 * @param triangulo Puntero al triángulo del cual calcular el centroide.
 * @return Vector3 que representa el centroide del triángulo.
 */
Vector3 compute_polygon_centroid(Triangulo* triangulo) {
    Vector3 baricentro;
    baricentro.x = (triangulo->p1.x + triangulo->p2.x + triangulo->p3.x) / 3.0;
    baricentro.y = (triangulo->p1.y + triangulo->p2.y + triangulo->p3.y) / 3.0;
    baricentro.z = (triangulo->p1.z + triangulo->p2.z + triangulo->p3.z) / 3.0;
    return baricentro;
}

/**
 * Calcula el vector normal a la superficie de un polígono (triángulo realmente). Por ahora
 * la utilizo para el back culling y para dibujar los propios vectores.
 * @param poligono Puntero al triángulo para calcular su vector normal.
 * @param normal_vector Puntero al Vector3 donde se almacenará el vector normal calculado.
 */
void obtain_normal_vector(const Triangulo* poligono, Vector3* normal_vector) {

    Vector3 v1, v2, v3;
    point_to_vector(poligono->p1, &v1);
    point_to_vector(poligono->p2, &v2);
    point_to_vector(poligono->p3, &v3);

    Vector3 edge1 = vector3_substract(v2, v1);
    Vector3 edge2 = vector3_substract(v3, v1);

    Vector3 cross_product = vector3_cross_product(edge1, edge2);

    // Normalizar el vector resultante
    *(normal_vector) = normalizar_vector(cross_product);
}

/**
 * Determina si un polígono debe ser dibujado basándose en la orientación de su
 * vector normal. Utilizo en back culling, para determinar si la cara ha de dibujarse,
 * y así omitir el polígono.
 * @param normal_vector Vector normal del polígono.
 * @param vector_forward Vector hacia adelante de la cámara o contexto de visualización.
 * @return 1 si el polígono debe ser dibujado, 0 en caso contrario.
 */
int should_draw_polygon(const Vector3 normal_vector, const Vector3 vector_forward) {
    const float dot_product = vector3_dot_product(normal_vector, vector_forward);
    printf("\n dot product value: %f\n", dot_product);
    return dot_product > 0 ? 0 : 1;
}