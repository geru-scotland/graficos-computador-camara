#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "shared_defines.h"

/***********************************************************************
 * Este archivo de cabecera, define unac gama de operaciones y funciones
 * utilizadas en el proyecto. Incluye operaciones matriciales y vectoriales
 * fundamentales, funciones para la gestión de matrices y la realización de
 * transformaciones geométricas, así como operaciones de ordenación y
 * cálculos relacionados con puntos y vectores.
 *
 * También proporciona funciones específicas para la gestión de cámaras,
 * incluyendo la actualización y configuración de las vistas de la cámara,
 * y para el manejo de mallas y ejes en gráficos 3D. Incluye
 * funciones de debugging y registro para facilitar el seguimiento y
 * análisis durante el desarrollo.
 ***********************************************************************/


/***********************************************************************
 *                                                                     *
 *      OPERACIONES MATRICIALES, VECTORIALES Y DE ORDENACION           *
 *                                                                     *
 ***********************************************************************/

void mxp(Punto *pptr, double matriz_trans[16], Punto p);
void matrix_multiplication(double a[4][4], double b[4][4], double result[4][4]);
Vector3 vector3(float x, float y, float z);
Vector3 vector3_substract(Vector3 v1, Vector3 v2);
Vector3 vector3_cross_product(Vector3 v1, Vector3 v2);
float vector3_dot_product(Vector3 v1, Vector3 v2);
Vector3 normalizar_vector(Vector3 vector);
void copiar_punto(Punto* pptr, Punto* p);
void ordenar_puntos(Punto *p1, Punto *p2, Punto *p3);
void ordenar_puntos_x(Punto *p1, Punto *p2, Punto *p3);
void interpolacion_lineal(Punto *p1, Punto *p2, Punto *punto_corte);
Punto calcular_centroide(triobj *obj);
void point_to_vector(Punto point, Vector3* vector);
void scale_point(Punto *p, const float factor);
void apply_perspective_depth(Triangulo* triangulo, Punto* p1, Punto* p2, Punto* p3);
Vector3 compute_polygon_centroid(Triangulo* triangulo);
void obtain_normal_vector(const Triangulo* poligono, Vector3* normal_vector);
int should_draw_polygon(const Vector3 normal_vector, const Vector3  vector_forward);

/***********************************************************************
 *                                                                     *
 *                          DEBUG Y LOGGING                            *
 *                                                                     *
 ***********************************************************************/

void print_matrix(double matrix[4][4]);
void print_transformations(const triobj* sel_ptr);
void print_camera_data(const Camera* camera);
void print_scene_mask(const int* scene_mask);
void print_vector(Vector3* vector);

/***********************************************************************
 *                                                                     *
 *                        GESTIÓN DE MATRICES                          *
 *                                                                     *
 ***********************************************************************/

mlist* gestionar_nueva_matriz(triobj* sel_ptr);
void undo(triobj* sel_ptr);

/***********************************************************************
 *                                                                     *
 *                              MALLAS Y EJES                          *
 *                                                                     *
 ***********************************************************************/

void dibujar_ejes();
void dibujar_malla(float grid_size);
void dibujar_ejes_objeto(triobj *obj);
void draw_vector(Triangulo* triangulo, Vector3 normal_vector);

/***********************************************************************
 *                                                                     *
 *                              CÁMARA                                 *
 *                                                                     *
 ***********************************************************************/

void set_view_matrix(Camera* cam);
void copy_camera(Camera* src, Camera* dest);
void update_camera(Camera* cam, Vector3 eye_position, Vector3 look_at, Vector3 up_vector);
void camera_pipeline(Camera* main_camera, unsigned int scene_status_mask, Triangulo* triangulo_procesado, Triangulo* triangulo, double matriz_transformacion[16]);
void swap_camera(unsigned int scene_status_mask, triobj* sel_ptr, Camera* main_camera, Camera* secondary_camera);
void update_camera_position(Camera *main_camera);
void update_camera_vectors(Camera* camera, Vector3 look_at);
void update_camera_vectors_from_view_matrix(Camera* camera);

/***********************************************************************
 *                                                                     *
 *                          TRANSFORMACIONES                           *
 *                                                                     *
 ***********************************************************************/

void set_rotation_matrix(char eje, float theta, double matriz_rotacion[4][4]);
void rotate(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr);
void traslacion_local(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr);
void traslacion_orbita(char eje, int dir, Camera* camera, triobj* sel_ptr);
void transformar(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr);

#endif FUNCTIONS_H